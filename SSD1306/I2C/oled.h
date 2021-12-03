//////////////////////////////////////////////////////////////////////////////////	 
//
//  文 件 名   : oled.h
//  版 本 号   : v1.0
//  作    者   : xq
//  生成日期   : 2021-11-05
//  最近修改   : 
//  功能描述   : SSD1306 OLED驱动
//              说明:   对象化封装，使用时提供I2C/SPI/8080/6080接口的回调函数用于重写方法
//                      因为I/O不同等原因，每个硬件对象都有独特的属性和方法，需要重写(最底处有例子/模板)
//              结构：user->OLED->I2C->GPIO
//              ----------------------------------------------------------------
//              GND   电源地
//              VCC   接5V或3.3v电源
//              SCL   I2C时钟线
//              SDA   接PB15（SDA） 
//              RES   接PB11 如果是用4针iic接口这个脚可以不接
//              ----------------------------------------------------------------
//Copyright(C) xq 2021/11/5
//All rights reserved
//////////////////////////////////////////////////////////////////////////////////
#ifndef _OLED_H_
#define _OLED_H_

#ifdef __cplusplus
	extern "C"{
#endif

#include <stdint.h>		
/*
	头文件的包含	
	#include <stdint.h>	
*/
//回调函数cb所使用的消息（用于调用不同底层接口）
typedef enum
{
    OLED_MSG_I2C_INIT,
    OLED_MSG_I2C_START,
    OLED_MSG_I2C_STOP,
    OLED_MSG_I2C_WAIT_ACK,
    OLED_MSG_I2C_WRITE_BYTE
}OLED_CB_MSG;

typedef struct _oled_t OLED_t;
typedef uint8_t (*oled_cb)(OLED_t *slf,OLED_CB_MSG msg, uint8_t data_byte);
struct _oled_t
{
    oled_cb cb;
};

//OLED类的实例化，即创建一个oled对象
//oled_cb: oled类包含操作底层I/O的方法，用户需重写该方法
void New_OLED(OLED_t *slf, oled_cb oled_cb);

//void OLED_WriteCmd(OLED_t *slf, uint8_t byte_to_write);

//void OLED_WriteData(OLED_t *slf, uint8_t byte_to_write);

void OLED_Clear(OLED_t *slf);
//OLED设置显示位置
void OLED_SetPos(OLED_t *slf, uint8_t x, uint8_t y);

void OLED_ON(OLED_t *slf);

void OLED_OFF(OLED_t *slf);

void fill_picture(OLED_t *slf, uint8_t fill_Data);

void OLED_ShowChar(OLED_t *slf, uint8_t x, uint8_t y, uint8_t chr, uint8_t Char_Size);

void OLED_ShowString(OLED_t *slf, uint8_t x, uint8_t y, char *str, uint8_t Char_Size);

void OLED_Init(OLED_t *slf);

#endif

/*
#include "Sofei2cMaster.h"	//注：调用了同样方式封装的软i2c库，用户可根据需求改成硬件I2C或自己的软I2C


步骤1： OLED_t my_oled;
步骤2： New_OLED(&my_oled, stm32_oled_cb);	//stm32_oled_cb是需要用户提供的回调函数API

//i2c底层操作回调
uint8_t stm32_i2c_cb(Sofei2cMaster *i2c, uint8_t msg)
{
	switch(msg)
	{
		case SI2C_MSG_INIT:					break;	//hal库已初始化IO
		case SI2C_MSG_SET_SDA:			OLED_I2C_SDA_GPIO_Port->ODR |= OLED_I2C_SDA_Pin;break;
		case SI2C_MSG_RESET_SDA:		OLED_I2C_SDA_GPIO_Port->ODR &= ~OLED_I2C_SDA_Pin;break;
		case SI2C_MSG_READ_SDA:			return OLED_I2C_SDA_GPIO_Port->IDR & OLED_I2C_SDA_Pin;
		case SI2C_MSG_OUTPUT_SDA:		break; 
		case SI2C_MSG_INPUT_SDA:		break;	

		case SI2C_MSG_OUTPUT_SCL:		break;
		case SI2C_MSG_SET_SCL:			OLED_I2C_SCL_GPIO_Port->ODR |= OLED_I2C_SCL_Pin;break;
		case SI2C_MSG_RESET_SCL:		OLED_I2C_SCL_GPIO_Port->ODR &= ~OLED_I2C_SCL_Pin;break;
//		case SI2C_MSG_BUF_DELAY:  	delay_ns(1300);break;	//停止信号与开始信号间的总线释放时间（SSD1306、TMP75为1.3us）
//		case SI2C_MSG_SUSTA_DELAY:	delay_ns(600);break;	//重复开始信号的建立时间（SCL=1的最短时间，SSD1306、TMP75为0.6us）
//		case SI2C_MSG_HDSTA_DELAY:	delay_ns(600);break;	//开始信号的保持时间（SDA拉低的最短时间，在这之后产生第一个时钟信号，SSD1306、TMP75为0.6us）
//		case SI2C_MSG_SUSTO_DELAY:	delay_ns(600);break;	//停止信号的建立时间（SDA拉高前SCL=1，SDA=0的保持时间，SSD1306、TMP75为0.6us）
//		case SI2C_MSG_HDDAT_DELAY:	break;								//数据保持时间(SCL=0后数据需要保持不变的时间，TMP75为4ns,很短,IO频率超过250M才需要?)
//		case SI2C_MSG_SUDAT_DELAY:	delay_ns(100);break;	//数据建立时间(SCL=1前SDA需要保持状态的时间，SSD1306、TMP75为100ns，在读写时序中被SCL延时覆盖，在ack中需使用，若频率低于10M则不需要?)
//		case SI2C_MSG_SCLL_DELAY:		delay_us(2);break;		//SCL低电平时间(读写时序中的主要延时，TMP75为1.3us min)
//		case SI2C_MSG_SCLH_DELAY:		delay_us(2);break;		//SCL高电平时间(读写时序中的主要延时, TMP75为0.6us min)
		default: break;//这里都不需要使用额外延时，2层回调 即使168M的STM32F4，调用回调的时间也已经够长了，当然不注释掉要使用延时也可以。
		//对于有些外设，读的时候需要适当慢一点，要不然外设反应不过来
	}
	return 0;
}
uint8_t stm32_oled_cb(OLED_t *slf, OLED_CB_MSG msg, uint8_t data_byte)
{
	static Sofei2cMaster oled_i2c;	//软i2c回调封装库步骤1：声明对象（C语言得声明）
	switch (msg)
	{
		case OLED_MSG_I2C_INIT:
		{
			New_Sofei2cMaster(&oled_i2c, stm32_i2c_cb);	//软i2c回调封装库步骤2：NEW(重写)对象
			Sofei2cMaster_init(&oled_i2c);
			break;
		}
		case OLED_MSG_I2C_START:
		{
			Sofei2cMaster_start(&oled_i2c);
			break;
		}
		case OLED_MSG_I2C_STOP:
		{
			Sofei2cMaster_stop(&oled_i2c);
			break;
		}
		case OLED_MSG_I2C_WAIT_ACK:
		{
			Sofei2cMaster_wait_ack(&oled_i2c);
			break;
		}
		case OLED_MSG_I2C_WRITE_BYTE:
		{
			Sofei2cMaster_send_byte(&oled_i2c, data_byte);
			break;
		}

		default: break;
	}
	return 0;
}

*/
