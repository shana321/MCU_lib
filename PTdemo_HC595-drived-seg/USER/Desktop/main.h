/*********************************
STC������Ƭ��
��һ��STC51ϵ�� 12T

�ڶ��� STC12ϵ��

������ STC15ϵ��

���Ĵ� STC8ϵ��
*********************************/
#ifndef _MAIN_H_
#define _MAIN_H_

//#include <reg52.h>
//#include <intrins.h>
//#include "io8051.h"
/***************��CPU���*********************/

/***************��������CPU�ҹ�*********************/
//PS:����������ֲ��ʱ����Բ��ı��κδ��볤���ڲ�ͬƽ̨��ֲ������STIM32��unsigned,int�ĳ��Ⱦͺ�51��AVR�Ĳ�ͬ��
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
