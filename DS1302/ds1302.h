#ifndef _DS1302_H_
#define _DS1302_H_

#ifdef __cplusplus
	extern "C"{
#endif
#define HAVE_TIME_H 0u

#if HAVE_TIME_H >0u
#include <time.h>

#else
//这里实现自己的time.h
typedef struct tm{
	int tm_sec;         /* 秒，范围从 0 到 59        */
	int tm_min;         /* 分，范围从 0 到 59        */
	int tm_hour;        /* 小时，范围从 0 到 23        */
	int tm_mday;        /* 一月中的第几天，范围从 1 到 31    */
	int tm_mon;         /* 月，范围从 0 到 11        */
	int tm_year;        /* 自 1900 年起的年数        */
	int tm_wday;        /* 一周中的第几天，范围从 0 到 6    */
//	int tm_yday;        /* 一年中的第几天，范围从 0 到 365    */
//	int tm_isdst;       /* 夏令时                */
}TimeTypeStruct;
//SingleRead时各时间量对应的寄存器号
#define _tm_sec 0
#define _tm_min 1
#define _tm_hour 2
#define _tm_mday 3
#define _tm_mon 4
#define _tm_wday 5
#define _tm_year 6
#endif
		
#define REENTRANT
//#define REENTRANT reentrant//一些特殊的处理器可能需要可重入标识，堆栈结构的这里就定义为空

#include <stdint.h>	
//#ifndef UINT
//#define UINT
//typedef unsigned char uint8_t;
//typedef unsigned int uint16_t;
//#endif
		
typedef struct _ds1302_ DS1302Class;
typedef uint8_t (*ds1302_cb)(DS1302Class *ser,uint8_t msg) REENTRANT;

struct _ds1302_
{
	ds1302_cb myds1302;
	//需要实现的方法：	MSG_SET_IO:设置IO引脚的电平为高
	//									MSG_RESET_IO:设置IO引脚的电平为低
	//									MSG_SET_SCLK:设置SCLK引脚的电平为高
	//									MSG_RESET_SCLK:设置SCLK引脚的电平为低
	//									MSG_SET_CE:设置CE引脚的电平为高
	//									MSG_RESET_CE:设置CE引脚的电平为低
	//									MSG_READ_IO:读取IO引脚的电平
	//									MSG_DS1302_INIT:DS1302的IO初始化
};

#define MSG_SET_SCLK 40
#define MSG_RESET_SCLK 41
#define MSG_SET_IO   42
#define MSG_RESET_IO   43
#define MSG_READ_IO  44
#define MSG_SET_CE   45
#define MSG_RESET_CE   46
#define MSG_DS1302_INIT 47

DS1302Class* New_DS1302(DS1302Class* obj,ds1302_cb cb) REENTRANT;

void DS1302_SendByte(DS1302Class* obj,uint8_t dat) REENTRANT;
uint8_t DS1302_ReceiveByte(DS1302Class* obj) REENTRANT;
void DS1302_Reset(DS1302Class* obj) REENTRANT;

void DS1302_SingleWrite(DS1302Class* obj,uint8_t addr,uint8_t dat) REENTRANT;
uint8_t DS1302_SingleRead(DS1302Class* obj,uint8_t addr) REENTRANT;
void DS1302_BurstWrite(DS1302Class* obj,uint8_t* dat) REENTRANT;
void DS1302_BurstRead(DS1302Class* obj,uint8_t* revbuf) REENTRANT;

void DS1302_SetSingleTime(DS1302Class* obj,uint8_t addr,uint8_t value) REENTRANT;
void DS1302_SetRealTime(DS1302Class* obj,TimeTypeStruct* objtime) REENTRANT;
uint8_t DS1302_GetSingleTime(DS1302Class* obj,uint8_t addr) REENTRANT;
void DS1302_GetRealTime(DS1302Class* obj,TimeTypeStruct* objtime) REENTRANT;
void DS1302_Init(DS1302Class* obj) REENTRANT;
#endif
