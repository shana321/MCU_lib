#include "ds1302.h"
static uint8_t BCD2Dec(uint8_t dat)
{
	dat = (dat>>4)*10+(dat&0x0F);
	return dat;
}
static uint8_t Dec2BCD(uint8_t dat)
{
	dat=((dat/10)<<4)+dat%10;
	return dat;
}
/************************************
@函数功能:实例化一个DS1302对象，需要重写底层IO方法
@INPUT:obj:对象对应的结构体变量，cb:回调函数句柄
@OUTPUT:对象结构体变量
@
@
************************************/
DS1302Class* New_DS1302(DS1302Class* obj,ds1302_cb cb) REENTRANT
{
	obj->myds1302 = cb;
	obj->myds1302(obj,MSG_DS1302_INIT);//初始化
	return obj;
}

void DS1302_SendByte(DS1302Class* obj,uint8_t dat) REENTRANT
{
	uint8_t mask;
	for(mask = 0x01;mask !=0; mask <<= 1)
	{
		if(dat & mask)
			obj->myds1302(obj,MSG_SET_IO);
		else
			obj->myds1302(obj,MSG_RESET_IO);
		obj->myds1302(obj,MSG_RESET_SCLK);
		obj->myds1302(obj,MSG_SET_SCLK);
	}
}
/************************************
@函数功能:从1302接收单字节数据
@INPUT:NULL
@OUTPUT:uint8_t，返回接收到的单字节数据
@
@
************************************/
uint8_t DS1302_ReceiveByte(DS1302Class* obj) REENTRANT
{
	uint8_t mask,dat=0;
	for(mask = 0x01;mask != 0;mask <<= 1)
	{
		obj->myds1302(obj,MSG_SET_SCLK);
		obj->myds1302(obj,MSG_RESET_SCLK);
		if(obj->myds1302(obj,MSG_READ_IO))
			dat |= mask;
	}
	return dat;
}
/************************************
@函数功能:复位
@INPUT:NULL
@OUTPUT:NULL
@
@
************************************/
void DS1302_Reset(DS1302Class* obj) REENTRANT
{
	obj->myds1302(obj,MSG_RESET_CE);
	obj->myds1302(obj,MSG_RESET_SCLK);
}

//读操作前需要拉低IO，或外加10k上拉电阻
//有8组可读写的寄存器，前7组和时间有关，0-7作为参数，基址为0x80和0x81，奇读偶写
//写：(addr<<1)|0x80 读：(addr<<1)|0x81 addr:0-6
/************************************
@函数功能:写单字节数据到1302对应寄存器
@INPUT:addr:寄存器号 范围0~6 dat：要写的数据
@OUTPUT:NULL
@
@
************************************/
void DS1302_SingleWrite(DS1302Class* obj,uint8_t addr,uint8_t dat) REENTRANT
{
	obj->myds1302(obj,MSG_SET_CE);
	DS1302_SendByte(obj,(addr<<1)|0x80);
	DS1302_SendByte(obj,dat);
	DS1302_Reset(obj);
}
/************************************
@函数功能:从1302对应寄存器读单字节数据
@INPUT:addr:寄存器号 范围0~7
@OUTPUT:读到的寄存器数据
@
@
************************************/
uint8_t DS1302_SingleRead(DS1302Class* obj,uint8_t addr) REENTRANT
{
	uint8_t dat=0;
	obj->myds1302(obj,MSG_SET_CE);
	DS1302_SendByte(obj,(addr<<1)|0x81);
	dat = DS1302_ReceiveByte(obj);
	obj->myds1302(obj,MSG_RESET_IO);	//注意
	DS1302_Reset(obj);
	return dat;
}
/************************************
@函数功能:突发模式，连写多字节数据到1302寄存器
@INPUT:uint8_t的指针，传多字节
@OUTPUT:NULL
@
@
************************************/
void DS1302_BurstWrite(DS1302Class* obj,uint8_t* dat) REENTRANT
{
	uint8_t i;
	obj->myds1302(obj,MSG_SET_CE);
	DS1302_SendByte(obj,0xBE);	//突发模式写寄存器
	for(i=0;i<8;i++)
	{
		DS1302_SendByte(obj,*dat++);	//写完8组寄存器
	}
	DS1302_Reset(obj);
}
/************************************
@函数功能:突发模式，连读多个1302寄存器
@INPUT:revbuf：存数据的数组指针
@OUTPUT:
@
@
************************************/
void DS1302_BurstRead(DS1302Class* obj,uint8_t* revbuf) REENTRANT
{
	uint8_t i;
	obj->myds1302(obj,MSG_SET_CE);
	DS1302_SendByte(obj,0XBF);
	for(i=0;i<8;i++)
	{
		revbuf[i] = DS1302_ReceiveByte(obj);
	}
	obj->myds1302(obj,MSG_RESET_IO);	//注意
	DS1302_Reset(obj);
}
//ds1302类结构体成员对应了寄存器顺序（倒序）
/************************************
@函数功能:单次设定年月日等某个时间参数
@INPUT:addr：寄存器地址 0-6有意义 value：要设置的值
@OUTPUT:
@
@
************************************/
void DS1302_SetSingleTime(DS1302Class* obj,uint8_t addr,uint8_t value) REENTRANT
{
	value = Dec2BCD(value);
	DS1302_SingleWrite(obj,addr,value);
}
/************************************
@函数功能:突发模式设定年月日等所有时间
@INPUT:objtime：兼容time.h的时间结构体指针
@OUTPUT:
@
@
************************************/
void DS1302_SetRealTime(DS1302Class* obj,TimeTypeStruct* objtime) REENTRANT
{
	uint8_t buf[8];
	buf[7] = 0;
	buf[6] = Dec2BCD(objtime->tm_year);
	buf[5] = Dec2BCD(objtime->tm_wday);
	buf[4] = Dec2BCD(objtime->tm_mon);
	buf[3] = Dec2BCD(objtime->tm_mday);
	buf[2] = Dec2BCD(objtime->tm_hour);
	buf[1] = Dec2BCD(objtime->tm_min);
	buf[0] = Dec2BCD(objtime->tm_sec);
	DS1302_BurstWrite(obj,buf);
}
/************************************
@函数功能:单次读获取年月日等某一值
@INPUT:addr:寄存器号 0-6有意义
@OUTPUT:
@
@
************************************/
uint8_t DS1302_GetSingleTime(DS1302Class* obj,uint8_t addr) REENTRANT
{
	uint8_t dat=0;
	dat = DS1302_SingleRead(obj,addr);
	dat = BCD2Dec(dat);
	return dat;
}
/************************************
@函数功能:突发模式获取年月日等所有时间
@INPUT:objtime：兼容time.h的时间结构体指针
@OUTPUT:
@
@
************************************/
void DS1302_GetRealTime(DS1302Class* obj,TimeTypeStruct* objtime) REENTRANT
{
	uint8_t buf[8];
	DS1302_BurstRead(obj,buf);
	objtime->tm_year = BCD2Dec(buf[6]);
	objtime->tm_wday = BCD2Dec(buf[5]);
	objtime->tm_mon = BCD2Dec(buf[4]);
	objtime->tm_mday = BCD2Dec(buf[3]);
	objtime->tm_hour = BCD2Dec(buf[2]);
	objtime->tm_min = BCD2Dec(buf[1]);
	objtime->tm_sec = BCD2Dec(buf[0]);
}
//应用层：初始化
//DS1302初始化时一定要向7号寄存器的MSB写0以解除写保护

void DS1302_Init(DS1302Class* obj) REENTRANT	//注意回调函数中的初始化是和平台相关的，这里的初始化不是
{
	DS1302_SingleWrite(obj,7,0x00);	//解除写保护
}
/*
回调函数示例
uint8_t stm32_ds1302_cb(DS1302Class* ser,uint8_t msg)
{
	uint8_t i;
	GPIO_InitTypeDef GPIO_InitStruct;
	switch(msg)
	{
		case MSG_SET_SCLK:
				HAL_GPIO_WritePin(DS1302_SCLK_PORT,DS1302_SCLK_PIN,GPIO_PIN_SET);
			break;
		
		case MSG_RESET_SCLK:
				HAL_GPIO_WritePin(DS1302_SCLK_PORT,DS1302_SCLK_PIN,GPIO_PIN_RESET);
			break;
		
		case MSG_SET_IO:
				HAL_GPIO_WritePin(DS1302_IO_PORT,DS1302_IO_PIN,GPIO_PIN_SET);
			break;
		
		case MSG_RESET_IO:
				HAL_GPIO_WritePin(DS1302_IO_PORT,DS1302_IO_PIN,GPIO_PIN_RESET);
			break;
			
		case MSG_READ_IO:
			DS1302_IO_PORT->CRH &= ~GPIO_CRH_MODE11;	//00
			DS1302_IO_PORT->CRH |= GPIO_CRH_CNF11_0;	//01
				i = HAL_GPIO_ReadPin(DS1302_IO_PORT,DS1302_IO_PIN);
			DS1302_IO_PORT->CRH |= GPIO_CRH_MODE11_0;//01
			DS1302_IO_PORT->CRH &= ~GPIO_CRH_CNF11;	//00
				return i;
		case MSG_SET_CE:
				HAL_GPIO_WritePin(DS1302_RST_PORT,DS1302_RST_PIN,GPIO_PIN_SET);
			break;
		
		case MSG_RESET_CE:
				HAL_GPIO_WritePin(DS1302_RST_PORT,DS1302_RST_PIN,GPIO_PIN_RESET);
			break;
		
		case MSG_DS1302_INIT:
			HAL_GPIO_WritePin(DS1302_SCLK_PORT, DS1302_SCLK_PIN, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(DS1302_IO_PORT, DS1302_IO_PIN, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(DS1302_RST_PORT, DS1302_RST_PIN, GPIO_PIN_RESET);
		
			GPIO_InitStruct.Pin = DS1302_SCLK_PIN;
			GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
			GPIO_InitStruct.Pull = GPIO_NOPULL;
			GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
			HAL_GPIO_Init(DS1302_SCLK_PORT, &GPIO_InitStruct);
		
			GPIO_InitStruct.Pin = DS1302_IO_PIN;
			GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
			GPIO_InitStruct.Pull = GPIO_NOPULL;
			GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
			HAL_GPIO_Init(DS1302_IO_PORT, &GPIO_InitStruct);
		
			GPIO_InitStruct.Pin = DS1302_RST_PIN;
			GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
			GPIO_InitStruct.Pull = GPIO_NOPULL;
			GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
			HAL_GPIO_Init(DS1302_RST_PORT, &GPIO_InitStruct);
			break;
		default: return 0;
	}
	return 1;
}
*/
