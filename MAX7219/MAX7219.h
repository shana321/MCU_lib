#ifndef _MAX7219_H_
#define _MAX7219_H_

#ifdef __cplusplus
	extern "C"{
#endif


#define C51 0u
//51
#if C51 > 0u
#include "stc89C5xRC.h"
sbit DIN=P1^3;
sbit CLK=P1^4;
sbit LOAD=P1^5;
#define MAX7219_SET_LOAD()      (LOAD=1)
#define MAX7219_RESET_LOAD()    (LOAD=0)
#define MAX7219_SET_DATA()      (DIN=1)
#define MAX7219_RESET_DATA()    (DIN=0)
#define MAX7219_SET_CLK()       (CLK=1)
#define MAX7219_RESET_CLK()     (CLK=0)
#else
#define MAX7219_SET_LOAD()      slf->obj_cb(slf,MAX7219_MSG_SET_LOAD)
#define MAX7219_RESET_LOAD()    slf->obj_cb(slf,MAX7219_MSG_RESET_LOAD)
#define MAX7219_SET_DATA()      slf->obj_cb(slf,MAX7219_MSG_SET_DATA)
#define MAX7219_RESET_DATA()    slf->obj_cb(slf,MAX7219_MSG_RESET_DATA)
#define MAX7219_SET_CLK()       slf->obj_cb(slf,MAX7219_MSG_SET_CLK)
#define MAX7219_RESET_CLK()     slf->obj_cb(slf,MAX7219_MSG_RESET_CLK)
#endif

#define UINT_DEF 1u
#if UINT_DEF > 0u
typedef unsigned char uint8_t;	
typedef unsigned int uint16_t;
#endif

#define DECODE_MODE  0x09   //解码控制寄存器
#define INTENSITY    0x0A   //亮度控制寄存器
#define SCAN_LIMIT   0x0B   //扫描界限寄存器
#define SHUT_DOWN    0x0C   //关断模式寄存器
#define DISPLAY_TEST 0x0F   //测试控制寄存器
#define DP 0x80
enum
{
	MAX7219_MSG_SET_LOAD,
	MAX7219_MSG_RESET_LOAD,
	MAX7219_MSG_SET_DATA,
	MAX7219_MSG_RESET_DATA,
	MAX7219_MSG_SET_CLK,
	MAX7219_MSG_RESET_CLK,
};
typedef struct _MAX7219 MAX7219Class;

typedef uint8_t (*MAX7219_cb)(MAX7219Class *obj,uint8_t msg);

struct _MAX7219
{
    MAX7219_cb	obj_cb;
	uint8_t		obj_num;	//级联个数,该芯片级联多少个就只能一次性写多少B 否则会出现问题
};

void Write7219_Byte(MAX7219Class *slf,uint8_t dat);
void Write7219(MAX7219Class *slf,uint8_t addr,uint8_t dat,uint8_t pos);//pos:表示写第几级MAX7219
void MAX7219_Init(MAX7219Class *slf,uint8_t num);
#endif
