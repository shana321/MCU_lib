#include "obj74HC595.h"

/************************************
@函数功能:模拟SPI把数据发送到595的移位寄存器中
@INPUT:uint8_t的指针，可以传单个字，也可以传数组，多个字节,size_t 传递字节数,output 是否输出到锁存器
@OUTPUT:NULL
@
@
************************************/
SPI595_TypeStruct* HAL_Init_SPI595(SPI595_TypeStruct * obj,hc595_cb	cb)
{
	obj->myhc595 = cb;
	obj->myhc595(obj,MSG_HC_INIT,0);//初始化
	return obj;
}
void HAL_OBJ595_Sendbytes(SPI595_TypeStruct * obj,uint8_t *pdate,size_t num,bool output)
{
	uint8_t temp=0,i,j;
	//SH_CP = 1;//抬高总线，空闲时候为1,开始传输后会拉低总线
	
	obj->myhc595(obj,MSG_SH_CP,1);//SH_CP设置为高
	for(i  = 0;i < num;i++)
	{
		temp = *(pdate + i);
		for(j = 0;j < 8;j++)
		{
			obj->myhc595(obj,MSG_SH_CP,0);
			//51版
//			temp <<= 1;
//			CY == 1 ? obj->Set_DS() : obj->Reset_DS();
			//标准C版
			(temp&0x80) ? obj->myhc595(obj,MSG_ST_DS,1) : obj->myhc595(obj,MSG_ST_DS,0);
			 temp <<= 1;
			//GPIO_WritePin(obj->DS_Port,obj->DS_PIN,CY == 1 ? (HIGH):(LOW));
			obj->myhc595(obj,MSG_SH_CP,1);//SH_CP设置为高
		}
	}
	//SH_CP = 1;//释放总线
	obj->myhc595(obj,MSG_SH_CP,1);//SH_CP设置为高
	if(output == true)
	{
		obj->myhc595(obj,MSG_ST_CP,1);//SH_CP设置为高
		obj->myhc595(obj,MSG_ST_CP,0);//SH_CP设置为低
		obj->myhc595(obj,MSG_ST_CP,1);//SH_CP设置为高
	}
}
