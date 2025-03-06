/*
 * @file      app_wifi_task.c
 *
 * @brief     连接wifi的任务 发送数据
 *
 * @version   V1.6
 *
 * @date      2025/1/18 星期六
 *
 * @auther    chenxuan_123
 *
 * @tips      1tab = 4spaces
 */

/****************System_Include***********************/
#include <string.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/****************BSP_Include*************************/
#include "bsp_esp8266_driver.h"
#include "bsp_led_driver.h"
#include "bsp_usart_driver.h"
#include "bsp_oled_driver.h"
#include "Cloud.h"
#include "Mqttkit.h"


/****************Task_Include*************************/
#include "app_wifi_task.h"
#include "app_aht20_task.h"
#include "app_air_task.h"
#include "app_lm2904_task.h"
#include "app_gy906_task.h"


/****************Semaphore Init*************************/
extern SemaphoreHandle_t xAht20ReadySemaphore;
extern SemaphoreHandle_t xLm2904ReadySemaphore;
extern SemaphoreHandle_t xAirReadySemaphore;
extern SemaphoreHandle_t xGy906ReadySemaphore;
extern SemaphoreHandle_t xRevDataReadySemaphore;

/****************Queue Init*************************/
extern QueueHandle_t xWeatherDataQueue;
extern QueueHandle_t xAirDataQueue;
extern QueueHandle_t xDbDataQueue;
extern QueueHandle_t xTemperatureDataQueue;

/*wifi串口接收缓冲区*/
extern unsigned char esp8266_rx_buffer[256];

/*
 * @brief   初始化 WiFi 和 MQTT 连接
 * @param   None
 * @retval  HAL_OK - 成功, HAL_ERROR - 失败
 */
static HAL_StatusTypeDef wifi_mqtt_init(void) {
    ESP8266_StatusTypeDef_t esp8266_status = ESP8266_ERROR;
    Cloud_Status_t cloud_statu;

    const char *devSubTopic[] = {"smart_home/subscribe"};

    
    esp8266_status = ESP8266_Connect();
    if (esp8266_status != ESP8266_OK) {
#if DEBUG
        DEBUG_LOG("WiFi Tcp连接失败!\n");
#endif
        return HAL_ERROR;
    }
#if DEBUG
    DEBUG_LOG("WiFi Tcp连接成功!\n");
#endif

    if (Cloud_DevLink() == 0) {
        cloud_statu = Cloud_Subscribe(devSubTopic, 1);
        if (cloud_statu == Cloud_Ok) {
#if DEBUG
            DEBUG_LOG("订阅成功: %s\n", *devSubTopic);
            DEBUG_LOG("Wifi连接/订阅成功,释放信号量 \n");
#endif
            xSemaphoreGive(xAht20ReadySemaphore);
            xSemaphoreGive(xAirReadySemaphore);
            xSemaphoreGive(xLm2904ReadySemaphore);
            xSemaphoreGive(xGy906ReadySemaphore);
            xSemaphoreGive(xRevDataReadySemaphore);
            return HAL_OK;
        } else {
#if DEBUG
            DEBUG_LOG("订阅失败!\n");
#endif
            return HAL_ERROR;
        }
    } else {
#if DEBUG
        DEBUG_LOG("MQTT Broker 连接失败!\n");
#endif
        return HAL_ERROR;
    }
}

/*
 * @brief   发送 MQTT 心跳包
 * @param   None
 * @retval  None
 */
static void send_mqtt_ping(void) {
    MQTT_PACKET_STRUCTURE pingPacket = {NULL, 0, 0, 0};
    if (MQTT_PacketPing(&pingPacket) == 0) {
        ESP8266_SendData((char*)pingPacket._data, pingPacket._len);
        MQTT_DeleteBuffer(&pingPacket);
    }
}

/*
 * @brief   检查 TCP 连接状态
 * @param   None
 * @retval  HAL_OK - 连接正常, HAL_ERROR - 连接断开
 */
static HAL_StatusTypeDef check_tcp_connection(void) {
    ESP8266_Clear();
    if (ESP8266_SendCmd("AT+CIPSTATUS\r\n", "STATUS:3", 1000) == ESP8266_OK) {
#if DEBUG
        //DEBUG_LOG("TCP 连接正常\n");
#endif
        return HAL_OK;
    }
#if DEBUG
    DEBUG_LOG("TCP 连接异常，缓冲区: %s\n", esp8266_rx_buffer);
#endif
    return HAL_ERROR;
}

/*
 * @brief   连接 WiFi 和 MQTT 服务器，订阅并发布数据
 * @param   *pvParameters 任务创建时传递的参数
 * @retval  None
 */
void wifi_connect_task(void *pvParameters) {
    HAL_StatusTypeDef wifi_status = HAL_ERROR;
    char PUB_BUF[512];
    const char devPubTopic[] = {"smart_home/publish"};
    uint32_t ping_counter = 0;

    weatherData_t receiveData_aht20 = {0.0f, 0.0f};
    airData_t receiveData_air = {0.0f, 0.0f, 0.0f, 0};
    dbData_t receiveData_db = {0.0f};
    temperatureData_t temperatureData = {0.0f,0.0f};

    while (wifi_status != HAL_OK) {
        wifi_status = wifi_mqtt_init();
        if (wifi_status != HAL_OK) {
#if DEBUG
            DEBUG_LOG("初始化失败，1秒后重试...\n");
#endif
            vTaskDelay(pdMS_TO_TICKS(1000));
        }
    }

    while (1) {
        /* 1. 接收 AHT20 数据 */
        if (xQueueReceive(xWeatherDataQueue, &receiveData_aht20, pdMS_TO_TICKS(500)) == pdTRUE) {
#if DEBUG
            DEBUG_LOG("wifi_task 温度：%.2fC 湿度：%.2f%%\n",
                      receiveData_aht20.temperature, receiveData_aht20.humidity);
#endif
        }

        /* 2. 接收空气质量数据 */
        if (xQueueReceive(xAirDataQueue, &receiveData_air, pdMS_TO_TICKS(500)) == pdTRUE) {
#if DEBUG
		DEBUG_LOG("wifi_task TVOC:%.2f CO2:%.2f HCHO:%.2f pm2.5:%d ug/m3\n",
				  receiveData_air.TVOC, receiveData_air.CO2, receiveData_air.HCHO, receiveData_air.PM25);
#endif
        }

        /* 3. 接收分贝数据 */
        if (xQueueReceive(xDbDataQueue, &receiveData_db, pdMS_TO_TICKS(500)) == pdTRUE) {
#if DEBUG
            DEBUG_LOG("wifi_task 分贝值:%.2f\n", receiveData_db.db_value);
#endif
        }

        /*4. 接收体温数据 */
        if(xQueueReceive(xTemperatureDataQueue,&temperatureData,pdMS_TO_TICKS(500) == pdTRUE))
        {
 #if DEBUG
         DEBUG_LOG("wifi_task 体温:%.2f\n",temperatureData.people_temp);
 #endif
        }

        /* . 检查 TCP 连接状态 */
        if (check_tcp_connection() != HAL_OK) {
            wifi_status = HAL_ERROR;
        }

        /* . 打包并发布数据 */
        int len = snprintf(PUB_BUF, sizeof(PUB_BUF),
                           "{\"Temp\":\"%.2f\",\"Humi\":\"%.2f\",\"TVOC\":\"%.2f\",\"CO2\":\"%.1f\",\"HCHO\":\"%.3f\",\"PM25\":\"%d\",\"DB\":\"%.2f\",\"people_temp\":\"%.2f\"}",
                           receiveData_aht20.temperature, receiveData_aht20.humidity,
                           receiveData_air.TVOC, receiveData_air.CO2, receiveData_air.HCHO,
                           receiveData_air.PM25, receiveData_db.db_value,
                           temperatureData.people_temp);
        if (len >= sizeof(PUB_BUF)) {
#if DEBUG
            DEBUG_LOG("PUB_BUF 溢出!\n");
#endif
            continue;
        }

        if (wifi_status == HAL_OK) {
            Cloud_Status_t public_status = Cloud_Publish(devPubTopic, PUB_BUF);
            if (public_status == Cloud_Ok) {
#if DEBUG
                DEBUG_LOG("数据上传成功\n");
#endif
            } else {
#if DEBUG
                DEBUG_LOG("数据上传失败\n");
#endif
                wifi_status = HAL_ERROR;
            }
        } else {
#if DEBUG
            DEBUG_LOG("连接断开，跳过发布\n");
#endif
        }

        /* . 发送心跳包，每 20 秒一次 */
        ping_counter++;
        if (ping_counter >= 20) {
            if (wifi_status == HAL_OK) {
                send_mqtt_ping();
            }
            ping_counter = 0;
        }

        /* . 检查连接状态并重连 */
        if (wifi_status != HAL_OK) {
            while (wifi_status != HAL_OK) {
                wifi_status = wifi_mqtt_init();
                if (wifi_status != HAL_OK) {
#if DEBUG
                    DEBUG_LOG("重连失败，1秒后重试...\n");
#endif
                    vTaskDelay(pdMS_TO_TICKS(1000));
                }
            }
#if DEBUG
            DEBUG_LOG("重连成功\n");
#endif
        }

        ESP8266_Clear();
//		char task_buffer[256];
//		vTaskList(task_buffer);
//		DEBUG_LOG("任务状态:\n%s\n", task_buffer);
//		DEBUG_LOG("堆剩余: %u\n", xPortGetFreeHeapSize());
//		DEBUG_LOG("wifi_connect 堆栈剩余: %lu\n", uxTaskGetStackHighWaterMark(NULL));
        vTaskDelay(pdMS_TO_TICKS(3000));
    }
	DEBUG_LOG("wifi_connect_task 意外退出\n");
}

