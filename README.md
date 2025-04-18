# 🤩✨基于树莓派的智能家庭健康检测系统设计🤯

## 1.项目需求(功能)🐨

### STM32部分：

数据采集功能：

- 心率采集
- 血氧采集
- 人体温度采集
- 空气质量采集：比如吸烟  c02浓度  甲醛 pM2.5等危害人体健康
- 环境温湿度采集 ：如果室温过高打开空调或风扇 或开窗通风  
- 噪声采集

数据传输功能：

- 采集传感器的数据：USART,SPI,I2c,RS485等都可能用到 看传感器
- 数据上云(WIFI):通过MQTT协议将传感器的全部数据上传到MQTT Broker(Mosquitto)

### 树莓派部分(Qt 显示得到的数据 和进行数据的分析)：

- 数据接收：通过Qt  MQTT订阅STM32发布的主题，进行数据的接收。
- 数据存储：MySQL数据库
- 数据分析：将得到的数据通过Http协议上传至百度AI模型 通过分析的到合理的建议
- 数据显示：将得到的数据通过控件组件显示到Qt界面上

## 2.芯片以及传感器选型🐼

### 传感器选型：                                                                                      	

- 心率血氧：MAX30102模块                                      				                                                      I2C
- 体温度：GY906 DCC测温探头  10cm内测温                                                                             I2C
-  三合一空气传感器：VOC(有机性挥发气体例如天然气)/CO2/甲醛                                     USART
- PM2.5传感器：DC01红外PM2.5空气质量传感器模块                                                            USART
- 噪声传感器：LM2904传感器                                                                                        ADC/单GPIO口
- AHT20 温湿度

### 功能模块选型：

- 风扇：直流电机                                                                                                                                 PWM              接口：PH2.0-3Pin
- 加湿器模块：                                                                                                                                      USB或TypeC
- 舵机模拟开窗                                                                                                                                       PWM
- 蜂鸣器(有害气体报警)

### 测量的数据有 ：

​	心率值，血氧值，体温，VOC烟雾，CO2,甲醛，PM2.5粉尘，声音（分贝），环境温度，环境湿度。

### 反馈的动作：

​	有害气体浓度过高：风扇转动，舵机转动，蜂鸣器报警

​	环境温度高，湿度低：打开加湿器

### STM32芯片选型：

STM32F205RET6: 

FLASH:512K    移植FreeRTOS  

ADC:3组12位逐次逼近型

I2C:3组

串口：6组

TIM:14组

### 树莓派5   ：

- usb麦克风
- usb喇叭
- 7英寸显示屏

## 3.具体实现步骤 功能验证

### 3.1Qt部分

#### 登录界面：

需求：

- 实现基本的登录功能                   ✔
- 注册按钮，忘记密码等按钮       ✔
- 美化                                             ✔
- 软键盘实现                                 ✔

#### 注册界面：

需求：

- 账号，密码，确认密码等              ✔
- 身份信息：姓名，年龄，性别，身高，体重等  ✔
- 美化                                             ✔
- 软键盘实现                                  ✔

#### 显示界面 

Home界面需求：

- 心率血氧:心率数据显示
  				血氧数据显示
- 体温计：体温数据显示
- 空气质量传感器：TVOC数据显示
                                 CO2数据显示
                                 甲醛数据显示
- PM2.5传感器：数据显示
- 噪声传感器：分贝数据显示
- AHT20: 温度数据显示
               湿度数据显示
- 直流电机：开关
- 加湿器模块：开关
- 舵机：开关         
- 心率血氧折线图  

report 健康报告界面：

- AI健康报告
- AI对话

user 用户记录界面：

- 显示当前家庭的用户

### 3.2KEIL单片机部分

#### 每个传感器的裸机驱动代码验证：

空气质量传感器(TVOC,CO2,HCHO):                  ✔

蜂鸣器：																✔

舵机：																	✔

LM2904(噪声)：													✔

温湿度（AHT20）：											✔

体温探头：															✔

PM2.5:																	✔

继电器：																✔

WIFI连接MQTT:													✔

直流电机：															✔

心率血氧：															

雾化器：															  ✔	

#### FreeRTOS代码部分：

- 移植freeRTOS：                                                    ✔

- wifi任务：连接wifi 连接mqtt  其它任务将数据发送到wifi任务，由wifi任务组成数据包上传至云服务器：
  - 任务堆栈：128
  - 任务优先级：2

- aht20任务：测量环境的温湿度数据，进行数据滤波，将数据发送给wifi任务：
  - 任务堆栈：128
  - 任务优先级：3

- air任务：测量空气中的TVOC,CO2,甲醛（HCHO）:
  - 任务堆栈：128
  - 任务优先级：3
- LM2904:测量环境中的噪声（db）：
