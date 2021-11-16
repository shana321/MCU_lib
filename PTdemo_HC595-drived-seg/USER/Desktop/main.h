/*********************************
STC各代单片机
第一代STC51系列 12T

第二代 STC12系列

第三代 STC15系列

第四代 STC8系列
*********************************/
#ifndef _MAIN_H_
#define _MAIN_H_

//#include <reg52.h>
//#include <intrins.h>
//#include "io8051.h"
/***************和CPU相关*********************/

/***************将类型与CPU挂钩*********************/
//PS:这样代码移植的时候可以不改变任何代码长度在不同平台移植，例如STIM32的unsigned,int的长度就和51，AVR的不同。
typedef unsigned char uint8_t;
typedef unsigned int  uint16_t;
typedef unsigned long uint32_t;
typedef char int8_t;
typedef int  int16_t;
typedef long int32_t;
//typedef bit bool;
//typedef unsigned int size_t;

#define     __IO    volatile
#define   	__I     volatile const
#define     __O     volatile


#define false  0
#define true   !false
	
#define DISABLE 0
#define ENABLE  !0
#endif
