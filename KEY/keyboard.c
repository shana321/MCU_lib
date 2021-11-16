#include "keyboard.h"
void KeyBoard_Init(Keyscan_Type *myself,Key_Read_cb key_read_cb,Key_Callback key_cb)
{
	myself->key_read_cb = key_read_cb;
	myself->FirstPinState = myself->key_read_cb(myself);//上电读电平，作为初始按下的电平;
	myself->key_cb = key_cb;//回调
	myself->LastPinState = myself->FirstPinState;//默认是初始化的时候为释放状态
}
Key_State KeyScan(Keyscan_Type* myself)
{
	GPIO_PinState key = myself->key_read_cb(myself);
	Key_State returnkey;
	
	if(myself->LastPinState == key)//上一次的电平和这次一样
	{
		(key == myself->FirstPinState)?(returnkey=KEY_UPED):(returnkey=KEY_DOWNED);//判断是空闲电平还是动作电平,空闲电平返回已经释放，动作电平返回已经按下
	}
	else
	{
		(key == myself->FirstPinState)?(returnkey=KEY_UPING):(returnkey=KEY_DOWNING);////判断是空闲电平还是动作电平,空闲电平由动作电平转换到空闲电平瞬间，表示刚刚释放，反之
	}
	myself->LastPinState = key;
	myself->key_cb(myself,returnkey);
	return returnkey;
}
