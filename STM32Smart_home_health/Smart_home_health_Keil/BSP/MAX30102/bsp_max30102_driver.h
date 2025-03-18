#ifndef __MAX30102_H__
#define __MAX30102_H__

#include "stm32f2xx_hal.h"
#include "sys.h"

#define MAX30102_IIC_PORT           GPIOB
#define MAX30102_IIC_SCL_PIN        GPIO_PIN_10
#define MAX30102_IIC_SDA_PIN        GPIO_PIN_11
#define MAX30102_INT_PORT           GPIOB
#define MAX30102_INT_PIN            GPIO_PIN_1
#define MAX30102_INT                PBin(1)

#define I2C_WR  0
#define I2C_RD  1
#define I2C_WRITE_ADDR 0xAE
#define I2C_READ_ADDR 0xAF

#define true 1
#define false 0
#define FS 100
#define BUFFER_SIZE (FS * 5)
#define HR_FIFO_SIZE 7
#define MA4_SIZE 4
#define HAMMING_SIZE 5
#define min(x,y) ((x) < (y) ? (x) : (y))

#define max30102_WR_address 0xAE

// 寄存器地址定义保持不变
#define REG_INTR_STATUS_1 0x00
#define REG_INTR_STATUS_2 0x01
#define REG_INTR_ENABLE_1 0x02
#define REG_INTR_ENABLE_2 0x03
#define REG_FIFO_WR_PTR 0x04
#define REG_OVF_COUNTER 0x05
#define REG_FIFO_RD_PTR 0x06
#define REG_FIFO_DATA 0x07
#define REG_FIFO_CONFIG 0x08
#define REG_MODE_CONFIG 0x09
#define REG_SPO2_CONFIG 0x0A
#define REG_LED1_PA 0x0C
#define REG_LED2_PA 0x0D
#define REG_PILOT_PA 0x10
#define REG_MULTI_LED_CTRL1 0x11
#define REG_MULTI_LED_CTRL2 0x12
#define REG_TEMP_INTR 0x1F
#define REG_TEMP_FRAC 0x20
#define REG_TEMP_CONFIG 0x21
#define REG_PROX_INT_THRESH 0x30
#define REG_REV_ID 0xFE
#define REG_PART_ID 0xFF

// 硬件 I2C 函数
void MAX30102_IIC_Init(void);
void MAX30102_IIC_Write_One_Byte(uint8_t daddr, uint8_t addr, uint8_t data);
void MAX30102_IIC_Read_One_Byte(uint8_t daddr, uint8_t addr, uint8_t* data);
void MAX30102_IIC_WriteBytes(uint8_t WriteAddr, uint8_t* data, uint8_t dataLength);
void MAX30102_IIC_ReadBytes(uint8_t deviceAddr, uint8_t writeAddr, uint8_t* data, uint8_t dataLength);

// MAX30102 操作函数
void MAX30102_Init(void);
void MAX30102_Reset(void);
void MAX30102_Clear_FIFO(void);
uint8_t max30102_Bus_Write(uint8_t Register_Address, uint8_t Word_Data);
uint8_t max30102_Bus_Read(uint8_t Register_Address);
void max30102_FIFO_ReadWords(uint8_t Register_Address, uint16_t Word_Data[][2], uint8_t count);
void max30102_FIFO_ReadBytes(uint8_t Register_Address, uint8_t* Data);

void maxim_max30102_write_reg(uint8_t uch_addr, uint8_t uch_data);
void maxim_max30102_read_reg(uint8_t uch_addr, uint8_t *puch_data);
void maxim_max30102_read_fifo(uint32_t *pun_red_led, uint32_t *pun_ir_led);

// 心率血氧算法函数
void maxim_heart_rate_and_oxygen_saturation(uint32_t *pun_ir_buffer, int32_t n_ir_buffer_length, uint32_t *pun_red_buffer, int32_t *pn_spo2, int8_t *pch_spo2_valid,
                                            int32_t *pn_heart_rate, int8_t *pch_hr_valid);
void maxim_find_peaks(int32_t *pn_locs, int32_t *pn_npks, int32_t *pn_x, int32_t n_size, int32_t n_min_height, int32_t n_min_distance, int32_t n_max_num);
void maxim_peaks_above_min_height(int32_t *pn_locs, int32_t *pn_npks, int32_t *pn_x, int32_t n_size, int32_t n_min_height);
void maxim_remove_close_peaks(int32_t *pn_locs, int32_t *pn_npks, int32_t *pn_x, int32_t n_min_distance);
void maxim_sort_ascend(int32_t *pn_x, int32_t n_size);
void maxim_sort_indices_descend(int32_t *pn_x, int32_t *pn_indx, int32_t n_size);

#endif
