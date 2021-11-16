#include "main.h"
#include "DHT22.h"
#include "intrins.h"
//#define _nop_() asm("nop")
uint8_t DHT22_8C51_cb(dht22_t *dht, uint8_t msg);
void Delay2000ms();
dht22_t mydht;
dht22_t youdht;
void main( void )
{
	float hu,tm;
	int16_t hh,tmm;
	DHT22_Init(&mydht,DHT22_8C51_cb);
	DHT22_Init(&youdht,DHT22_8C51_cb);
  while(1)
	{
		DHT22_Read(&mydht);
		DHT22_Read(&youdht);
		P1 = mydht.DHT22_RXD_Buff[0];
		P2 = youdht.DHT22_RXD_Buff[1];
		DHT22_Fget(&mydht,&hu,&tm);
		DHT22_Fget(&youdht,&hu,&tm);
		DHT22_Iget(&mydht,&hh,&tmm);
		DHT22_Iget(&youdht,&hh,&tmm);
		Delay2000ms();
	}
}

void Delay2000ms()		//@120.000MHz
{
	unsigned char i, j, k;

	_nop_();
	i = 152;
	j = 254;
	k = 230;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	} while (--i);
}
void Delay18ms()		//@120.000MHz
{
	unsigned char i, j, k;

	_nop_();
	i = 2;
	j = 95;
	k = 43;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	} while (--i);
}
void Delay1ms()		//@120.000MHz
{
	unsigned char i, j;

	i = 20;
	j = 113;
	do
	{
		while (--j);
	} while (--i);
}

void Delay30us()		//@120.000MHz
{
	unsigned char i;

	_nop_();
	i = 147;
	while (--i);
}
uint8_t DHT22_8C51_cb(dht22_t *dht, uint8_t msg)
{
	switch(msg)
	{
	case DHT22_MSG_2S_DELAY://用于上电初始化稳定DHT22,如果不需要驱动内部上电稳定，这里可以留空，然后自己添加代码上电稳定
		Delay2000ms();
	break;
	case DHT22_MSG_18MS_DELAY:
			Delay18ms();
	break;
	case DHT22_MSG_1MS_DELAY:
		Delay1ms();
	break;
	
	case DHT22_MSG_30US_DELAY:
		Delay30us();
	break;
	case DHT22_MSG_READ_SDA: 
		if(dht == &mydht)  return (P3&0x01);
		if(dht == &youdht) return (P3&0x02);
	break;
	case DHT22_MSG_SDA_HIGH:
		if(dht == &mydht)(P3|=0x01);
		if(dht == &youdht)(P3|=0x02);
	break;
	case DHT22_MSG_SDA_LOW:
		if(dht == &mydht)(P3&=~0x01);
		if(dht == &youdht)(P3&=~0x02);
	break;
	default:return 0xFF;
	}
	return 0;
}