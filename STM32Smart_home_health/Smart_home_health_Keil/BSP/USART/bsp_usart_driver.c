/*
 * @file      bsp_usart_driver.c
 *
 * @brief     初始化串口
 *
 * @version   V1.0
 *
 * @date     2025/1/16 星期四
 *
 * @auther   chenxuan_123
 */


#include "bsp_usart_driver.h"
#include "FreeRTOS.h"
#include "task.h"


/*串口2句柄 wifi DMA句柄*/
UART_HandleTypeDef huart2;
DMA_HandleTypeDef hdma_usart2_rx;
DMA_HandleTypeDef hdma_usart2_tx;

/*串口1句柄 DMA句柄  Debuge口*/
UART_HandleTypeDef huart1;
DMA_HandleTypeDef hdma_usart1_rx;
DMA_HandleTypeDef hdma_usart1_tx;

/*
 * @brief   UART1 WIFI 通信串口初hdma_usart2_rx始化 PA2->TX  PA3->RX
 * @param   None
 * @retval  None
 */
void wifi_usart2_Init(void)
{
    //1.打开UART1时钟 GPIOA时钟 DMA2时钟
    __HAL_RCC_USART2_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_DMA1_CLK_ENABLE();
    
    //2.配置GPIO
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = GPIO_PIN_2 | GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
    HAL_GPIO_Init(GPIOA,&GPIO_InitStruct);

    //3.配置UART1参数
    huart2.Instance = USART2;
    huart2.Init.BaudRate = 115200;
    huart2.Init.WordLength = UART_WORDLENGTH_8B;
    huart2.Init.StopBits = UART_STOPBITS_1;
    huart2.Init.Parity = UART_PARITY_NONE;               				/*不启用奇偶校验*/
    huart2.Init.Mode = UART_MODE_TX_RX;									/*设置 USART1 的模式为收发（TX 和 RX）*/
    huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;						/*不启用硬件流控*/
    huart2.Init.OverSampling = UART_OVERSAMPLING_16;					/*设置过采样比为16*/
    HAL_UART_Init(&huart2);


    //4.配置DMA搬运器 USART2_RX DMA1_Stream5  USART2_TX DMA1_Stream6
    /*DMA2_rx DMA接收*/
    hdma_usart2_rx.Instance = DMA1_Stream5;
    hdma_usart2_rx.Init.Channel = DMA_CHANNEL_4;
    hdma_usart2_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;				/*配置数据传输方向为从外设到内存*/
    hdma_usart2_rx.Init.PeriphInc = DMA_PINC_DISABLE;					/*设置外设地址递增模式为禁用*/
    hdma_usart2_rx.Init.MemInc = DMA_MINC_ENABLE;						/*设置内存地址递增模式为启用*/
    hdma_usart2_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;		/*配置外设数据对齐方式为字节*/
    hdma_usart2_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;			/*配置内存数据对齐方式为字节*/
    HAL_DMA_Init(&hdma_usart2_rx);
    __HAL_LINKDMA(&huart2,hdmarx,hdma_usart2_rx);						/*将DMA句柄与串口句柄关联起来*/

    /*DMA2_tx DMA发送*/
    hdma_usart2_tx.Instance = DMA1_Stream6;
    hdma_usart2_tx.Init.Channel = DMA_CHANNEL_4;
    hdma_usart2_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;				/*配置数据传输方向为从内存到外设*/
    hdma_usart2_tx.Init.PeriphInc = DMA_PINC_DISABLE;					/*设置外设地址递增模式为禁用*/
    hdma_usart2_tx.Init.MemInc = DMA_MINC_ENABLE;						/*设置内存地址递增模式为启用*/
    hdma_usart2_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;		/*配置外设数据对齐方式为字节*/
    hdma_usart2_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;			/*配置内存数据对齐方式为字节*/
    HAL_DMA_Init(&hdma_usart2_tx);
    __HAL_LINKDMA(&huart2,hdmatx,hdma_usart2_tx);						/*将DMA句柄与串口句柄关联起来*/

    //5.设置中断优先级和分组
    HAL_NVIC_SetPriority(USART2_IRQn,0,0);
    HAL_NVIC_EnableIRQ(USART2_IRQn);

    HAL_NVIC_SetPriority(DMA1_Stream5_IRQn,0,0);
    HAL_NVIC_EnableIRQ(DMA1_Stream5_IRQn);
    HAL_NVIC_SetPriority(DMA1_Stream6_IRQn,0,0);
    HAL_NVIC_EnableIRQ(DMA1_Stream6_IRQn);


}

/*
 * @brief  串口2 DMA 发送数据
 */
void wifi_usart2_Send(uint8_t *data,uint16_t len)
{
    HAL_UART_Transmit_DMA(&huart2,data,len);
}

/*
 * @brief  串口2 DMA 接收数据
 */
void wifi_usart2_Receive(uint8_t *data,uint16_t len)
{
    HAL_UART_Receive_DMA(&huart2,data,len);
}


/*
 * @brief  串口1 DEBUG 串口初始化
 * @param   None
 * @retval  None
 */
void debug_usart1_Init(void)
{
        // 1.开启UART3时钟 GPIOB时钟 PA9->TX3 PA10->RX3
	__HAL_RCC_USART1_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_DMA2_CLK_ENABLE();

	
	

	
	// 2.配置GPIO口
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = GPIO_PIN_10 | GPIO_PIN_9;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
	HAL_GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	
	
	// 3.配置串口参数
	huart1.Instance = USART1;
	huart1.Init.BaudRate = 115200;
	huart1.Init.WordLength = UART_WORDLENGTH_8B;
	huart1.Init.StopBits = UART_STOPBITS_1;
	huart1.Init.Parity = UART_PARITY_NONE;               				/*不启用奇偶校验*/
	huart1.Init.Mode = UART_MODE_TX_RX;									/*设置 USART1 的模式为收发（TX 和 RX）*/
	huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;						/*不启用硬件流控*/
	huart1.Init.OverSampling = UART_OVERSAMPLING_16;					/*设置过采样比为16*/
	HAL_UART_Init(&huart1);
	
	//4.配置DMA搬运器 需要查看引脚映射表 USART3_RX DMA1_Stream1  USART3_TX DMA1_Stream3
	/*DMA1_rx DMA接收*/
	hdma_usart1_rx.Instance = DMA1_Stream5;
	hdma_usart1_rx.Init.Channel = DMA_CHANNEL_4;
	hdma_usart1_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;				/*配置数据传输方向为从外设到内存*/
	hdma_usart1_rx.Init.PeriphInc = DMA_PINC_DISABLE;					/*设置外设地址递增模式为禁用*/
	hdma_usart1_rx.Init.MemInc = DMA_MINC_ENABLE;						/*设置内存地址递增模式为启用*/
	hdma_usart1_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;		/*配置外设数据对齐方式为字节*/
	hdma_usart1_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;			/*配置内存数据对齐方式为字节*/
	hdma_usart1_rx.Init.Mode = DMA_NORMAL;							/*设置 DMA 模式为普通模式*/
	hdma_usart1_rx.Init.Priority = DMA_PRIORITY_LOW;					/*设置 DMA 的优先级为低*/
	hdma_usart1_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;				/*禁用 DMA 的 FIFO 模式*/
	hdma_usart1_rx.Init.MemBurst = DMA_MBURST_SINGLE;				/*设置内存突发传输为单次传输*/
	hdma_usart1_rx.Init.PeriphBurst = DMA_PBURST_SINGLE;				/*设置外设突发传输为单次传输*/

	/*初始化hdma_usart3_rx*/
	if(HAL_DMA_Init(&hdma_usart1_rx) != HAL_OK)
	{
	    while(1);
	}

	/*将已配置的 DMA 句柄 hdma_usart1_rx 链接到 USART 的接收 DMA 句柄 hdmarx*/
	__HAL_LINKDMA(&huart1,hdmarx,hdma_usart1_rx);
	
	/*DMA1_tx DMA发送*/
	hdma_usart1_tx.Instance = DMA1_Stream7;
	hdma_usart1_tx.Init.Channel = DMA_CHANNEL_4;
	hdma_usart1_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;				/*配置数据传输方向为从内存到外设*/
	hdma_usart1_tx.Init.PeriphInc = DMA_PINC_DISABLE;					/*设置外设地址递增模式为禁用*/
	hdma_usart1_tx.Init.MemInc = DMA_MINC_ENABLE;						/*设置内存地址递增模式为启用*/
	hdma_usart1_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;		/*配置外设数据对齐方式为字节*/
	hdma_usart1_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;			/*配置内存数据对齐方式为字节*/
	hdma_usart1_tx.Init.Mode = DMA_NORMAL;							/*设置 DMA 模式为普通模式*/
	hdma_usart1_tx.Init.Priority = DMA_PRIORITY_LOW;					/*设置 DMA 的优先级为低*/
	hdma_usart1_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;				/*禁用 DMA 的 FIFO 模式*/
	hdma_usart1_tx.Init.MemBurst = DMA_MBURST_SINGLE;				/*设置内存突发传输为单次传输*/
	hdma_usart1_tx.Init.PeriphBurst = DMA_PBURST_SINGLE;				/*设置外设突发传输为单次传输*/

	/*初始化hdma_usart3_tx*/
	if(HAL_DMA_Init(&hdma_usart1_tx) != HAL_OK)
	{
	    while(1);
	}

	/*将已配置的 DMA 句柄 hdma_usart1_tx 链接到 USART 的发送 DMA 句柄 hdmatx*/
	__HAL_LINKDMA(&huart1,hdmatx,hdma_usart1_tx);



	// 开启DMA中断
    //开启串口1中断
	HAL_NVIC_SetPriority(USART1_IRQn,0,0);
	HAL_NVIC_EnableIRQ(USART1_IRQn);
	/*DMA1 数据流5中断*/
	HAL_NVIC_SetPriority(DMA2_Stream5_IRQn,0,0);
	HAL_NVIC_EnableIRQ(DMA2_Stream5_IRQn);
	/*DMA1 数据流7中断*/
	HAL_NVIC_SetPriority(DMA2_Stream7_IRQn,0,0);
	HAL_NVIC_EnableIRQ(DMA2_Stream7_IRQn);


}
/*
 * @brief  串口1 发送数据
 * @param   None
 * @retval  None
 */
void Debug_usart1_Send(uint8_t *data,uint16_t len)
{
    HAL_UART_Transmit(&huart1,data,len,portMAX_DELAY);
}

/*
* @brief  将串口1重定向为printf 用于DEBUG 
 */
int fputc(int c, FILE* file)
{
    HAL_UART_Transmit(&huart1, (uint8_t*)&c, 1,portMAX_DELAY);
    return c;
}
