#ifndef _DHT11_H_
#define _DHT11_H_
//#include "main.h"
/*************************************************************************************************************************************
@模块功能：完成DHT11的驱动
@作者:Suaig
@时间:2018-2-1
@额外说明:本驱动在传统51上可能不能正常工作，由于51单片机主频低，且对于函数指针调用的独特方式致使
@				 其额外开销大(主要是时间上的)导致单总线这种需要严格时序的驱动不能正常驱动，实测在
@			 STC15 12M主频可以很好驱动 或者在AT89C51 120M主频下可以正常使用
@      stm32 avr都可
*************************************************************************************************************************************/
#define REENTRANT
//#define REENTRANT reentrant   //一些特殊的处理器可能需要可重入标识，堆栈结构的这里就定义为空

#ifdef __cplusplus //C++兼容
extern "C" {
#endif
	
#include <stdint.h>

enum{
	DHT11_MSG_2S_DELAY =40,
	DHT11_MSG_18MS_DELAY,
	DHT11_MSG_1MS_DELAY,
	DHT11_MSG_30US_DELAY,
	DHT11_MSG_READ_SDA,
	DHT11_MSG_SDA_HIGH,
	DHT11_MSG_SDA_LOW,
};


typedef struct DHT11_struct DHT11_t;
/*
	定义的回调函数，用于完成一些和硬件相关的操作
	DHT11_t *dht 指向DHT11对象的结构体指针
	uint8_t msg 消息，消息包括延时，IO操作等
*/
typedef uint8_t (*dht11_msg_cb)(DHT11_t *dht, uint8_t msg) REENTRANT;//由于51不是堆栈架构，所以，这里有一部分参数不能由于R0-R9进行传递，如需要使用模拟堆栈，这里即使用模拟堆栈


struct DHT11_struct
{
	uint8_t DHT11_RXD_Buff[5];
	dht11_msg_cb gpio_and_delay_cb;
	
};


/*
描述：DHT11对象初始化，传入回调函数和对象指针
参数:
			DHT11_t *dht 指向DHT11对象的结构体指针
			DHT11_msg_cb dht_cb 回掉函数的函数指针
			函数构造：uint8_t (*DHT11_msg_cb)(DHT11_t *dht, uint8_t msg)
*/
void DHT11_Init(DHT11_t *dht,dht11_msg_cb dht_cb);//DHT11对象初始化，传入回调函数和对象指针
uint8_t DHT11_Read(DHT11_t *dht);									//DHT11更新一次数据
uint8_t DHT11_Fget(DHT11_t *dht,float *Humi,float *Temp);//更新数据之后使用这个函数得到一个一位小数的浮点型数据，
/*
描述：更新数据之后使用这个函数得到一个int型数据，
参数:
			DHT11_t *dht 指向DHT11对象的结构体指针
			int16_t *Humi    用于返回湿度的16位长度的有符号整形的指针
			int16_t *Temp    用于返回温度的16位长度的有符号整形的指针
*/
uint8_t DHT11_Iget(DHT11_t *dht,int16_t *Humi,int16_t *Temp);//更新数据之后使用这个函数的到一个int型数据，

/****************************************************
*	回调函数示例
*完成最基本的IO的读写，以及三个延时函数既可完成移植
*uint8_t DHT11_8C51_cb(DHT11_t *dht, uint8_t msg)
{
	//这里未使用DHT11对象指针，因为我这里只需要去驱动一个DHT11
	switch(msg)
	{
	case DHT11_MSG_2S_DELAY://用于上电初始化稳定DHT11
		Delay2000ms();
	break;
	case DHT11_MSG_18MS_DELAY:
			Delay18ms();
	break;
	case DHT11_MSG_1MS_DELAY:
		Delay1ms();
	break;
	
	case DHT11_MSG_30US_DELAY:
		Delay30us();
	break;
	case DHT11_MSG_READ_SDA: 
		return (P3&0x01);
	break;
	case DHT11_MSG_SDA_HIGH:
				P3|=0x01;
	break;
	case DHT11_MSG_SDA_LOW:
				P3&=0xFE;
	break;
	default:return 0xFF;
	}
	return 0;
}
*
这里驱动两个多种多个，这是方式1，方式2，就是定义另一个名称不同的回掉函数，
uint8_t DHT11_8C51_cb(DHT11_t *dht, uint8_t msg)
{
	switch(msg)
	{
	case DHT11_MSG_2S_DELAY://用于上电初始化稳定DHT11
		Delay2000ms();
	break;
	case DHT11_MSG_18MS_DELAY:
			Delay18ms();
	break;
	case DHT11_MSG_1MS_DELAY:
		Delay1ms();
	break;
	
	case DHT11_MSG_30US_DELAY:
		Delay30us();
	break;
	case DHT11_MSG_READ_SDA: 
	if(dht == &hdht1)    //看这里,有硬件区别的地方都要加，包括下面写IO
	{
		return (P3&0x01);
	}
	else
	{
		return (P2&0x01);
	}
	break;
	case DHT11_MSG_SDA_HIGH:
				P3|=0x01;
	break;
	case DHT11_MSG_SDA_LOW:
				P3&=0xFE;
	break;
	default:return 0xFF;
	}
	return 0;
}
*****************************************************/
#ifdef __cplusplus
}
#endif
#endif