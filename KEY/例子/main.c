#include "main.h"
#include "keyboard.h"


Keyscan_Type LED_A,LED_B;
GPIO_PinState stc8051_key_read(Keyscan_Type *myself)//按钮的读IO回调函数,用于返回IO口的状态
{
	if(myself == &LED_A)
	{
		return P1&0x01;
	}
	if(myself == &LED_B)
	{
		return P0&0x01;
	}
	return 0;
}

uint8_t stc8051_key_cb(Keyscan_Type *myself,Key_State pt_t)//按键扫描回调函数,你可以在这轻松的完成连按，长按(需要配合系统时钟)或者是组合按钮
{
	//下面展示了两种写法
	if(myself == &LED_B)//是按钮B按下
	{
		switch(pt_t)
		{
			case KEY_DOWNED://按钮已经按下
				break;
			case KEY_DOWNING://按钮刚刚按下
				P2 ^= 0x01;
				break;
			case KEY_UPED://按钮已经释放
				break;
			case KEY_UPING://按钮刚刚释放
				break;
		}
	}
	switch(pt_t)
	{
		case KEY_DOWNED://按钮已经按下
			break;
		case KEY_DOWNING://按钮刚刚按下
			if(myself == &LED_A){P3 ^= 0x01;}
			break;
		case KEY_UPED://按钮已经释放
			break;
		case KEY_UPING://按钮刚刚释放
			break;
	}
	return 0;
}
void main( void )
{
	KeyBoard_Init(&LED_A,stc8051_key_read,stc8051_key_cb);
	KeyBoard_Init(&LED_B,stc8051_key_read,stc8051_key_cb);//初始化一个按钮对象，这个按钮对象以初始化时候读到的电平为空闲电平，并以此为基准
  while(1)
	{
		KeyScan(&LED_A);//不停的按钮扫描
		KeyScan(&LED_B);//不停的按钮扫描
	}
}
