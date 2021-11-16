#include "stm32f4xx.h"
#include "delay.h"
#include "stdint.h"
#include "SMG_Display.h"
#include "obj74HC595.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"

uint16_t find(uint8_t c);
uint16_t H595_SMG_cb(SMG_CLASS *ser,uint8_t msg,void *pt_r);


SMG_CLASS mysmg;
SPI595_TypeStruct smg_595;
uint8_t disbuff[16];

void Digitron_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
//使能GPIOF时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF,ENABLE);
	
//初始化GPIO口定义	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;//端口13|14|15
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;//普通输出
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;//推挽输出
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;//上拉
	GPIO_Init(GPIOF,&GPIO_InitStructure);
	
// 数码管对应引脚拉低，不亮
	GPIO_SetBits(GPIOF,GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15);

}
uint8_t stm32f4_595_cb(SPI595_TypeStruct *ser,uint8_t msg,uint8_t pt_r)
{
	switch(msg)
	{
		case MSG_SH_CP:
			if(pt_r == 1)
			{
				GPIO_SetBits(GPIOF,GPIO_Pin_13);
			}
			else
			{
				GPIO_ResetBits(GPIOF,GPIO_Pin_13);
			}
			break;
		case MSG_ST_CP:
			if(pt_r == 1)
			{
				GPIO_SetBits(GPIOF,GPIO_Pin_14);
			}
			else
			{
				GPIO_ResetBits(GPIOF,GPIO_Pin_14);
			}
			break;
		case MSG_ST_DS:
			if(pt_r == 1)
			{
				GPIO_SetBits(GPIOF,GPIO_Pin_15);
			}
			else
			{
				GPIO_ResetBits(GPIOF,GPIO_Pin_15);
			}
			break;
		case MSG_HC_INIT:Digitron_Init();break;
		default:return 0;
		
	}
	return 1;
	
}
int main(void)
{
	uint8_t i;
	uint8_t sendbuff[2] = {0xFE,0x06};
	uint8_t smg[]={"3.1415 6"};
	delay_init(16);
	//NewSMG(&mysmg,8,disbuff,16.)
	HAL_Init_SPI595(&smg_595,stm32f4_595_cb);
	HAL_OBJ595_Sendbytes(&smg_595,sendbuff,2,true);
	NewSMG(&mysmg,8,disbuff,16,H595_SMG_cb);
	for(i=0;i<sizeof(smg);i++)
	{
		Put2Buff(&mysmg,smg[i]);
	}
	
  while(1)
	{
		DisPlay(&mysmg);
		delay_ms(5);
	}
	//return 0;
}
uint16_t H595_SMG_cb(SMG_CLASS *ser,uint8_t msg,void *pt_r)
{
	static uint8_t buff[2];
	switch(msg)
	{
		case SMG_MSG_HASH:return find(*(uint8_t*)pt_r);//break;
		case SMG_MSG_BLANK:break;
		case SMG_MSG_WEI: buff[0] = ~(0x01<<*(uint8_t*)pt_r);break;
		case SMG_MSG_DUAN:buff[1] = *(uint8_t*)pt_r;HAL_OBJ595_Sendbytes(&smg_595,buff,2,true);break;
		default :return 0;
	}
	return 1;
}
uint16_t find(uint8_t c)
{
	const uint8_t table[]= {0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0x88,0x83,0xc6,0xa1,0x86,0x8e};
    switch(c)
    {
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
        return ~table[c-'0'];
        //break;

    case '.':
        return 0x80;
        //break;

    case ' ':
        return 0x00;
        //break;


    default:
        return 0xFFFF;//ASCII里面表示正文结束
    }
}
