#include "MAX7219.h"
void New_Max7219(MAX7219Class *slf,MAX7219_cb cb,uint8_t num)
{
    slf->obj_cb = cb;
    slf->obj_num = num;
}
void Write7219_Byte(MAX7219Class *slf,uint8_t dat)
{
    char i;
    for(i=0;i<8;i++)
    {
        MAX7219_RESET_CLK();
			(dat&0x80)?MAX7219_SET_DATA():MAX7219_RESET_DATA();
        dat <<= 1;
        MAX7219_SET_CLK();
    }
}
void Write7219(MAX7219Class *slf,uint8_t addr,uint8_t dat,uint8_t pos)//pos:表示写第几级MAX7219
{
	MAX7219_RESET_LOAD(); 
    for(i=0;((slf->obj_num)-pos-i)>0;i++)//防止多次操作前级同一个地址时误触后一个地址。
	{
		Write7219_Byte(0x00);
		Write7219_Byte(0x00);
	}
    Write7219_Byte(slf,addr);
	Write7219_Byte(slf,dat);
	for(;num>1;num--)
	{
		Write7219_Byte(slf,0x00);
		Write7219_Byte(slf,0x00);
	}
	MAX7219_SET_LOAD();
}

//MAX7219初始化，设置MAX7219内部的控制寄存器
void MAX7219_Init(MAX7219Class *slf,,uint8_t num)            
{
    Write7219(slf,SHUT_DOWN,0x01,num);          //开启正常工作模式（0xX1）
    Write7219(slf,DISPLAY_TEST,0x00,num);       //选择工作模式（0xX0） 
    Write7219(DECODE_MODE,0xff,num);            //选用全译码模式
//	Write7219(DECODE_MODE,0x00,num);            //全不译码
    Write7219(slf,SCAN_LIMIT,0x07,num);         //8只LED全用 0xX0~0xX7
    Write7219(slf,INTENSITY,0x04,num);          //设置初始亮度  0xX0~0xXF   
}
/*
回调函数示例：
uint8_t Max7219_cb(MAX7219Class *slf,uint8_t msg)
{
    switch(msg)
    {
        case MAX7219_MSG_SET_LOAD:
            break;
        case MAX7219_MSG_RESET_LOAD:
            break;
        case MAX7219_MSG_SET_DATA:
            break;
        case MAX7219_MSG_RESET_DATA:
            break;
        case MAX7219_MSG_SET_CLK:
            break;
        case MAX7219_MSG_RESET_CLK:
            break;
        default:
            break;
    }
    return 0;
}
*/