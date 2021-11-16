#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_
/********************************
51键盘键盘扫描
状态机，识别单个按键按下，长按，弹起，已经释放


********************************/

#include "main.h"
typedef enum
{
	KEY_DOWNED,
	KEY_DOWNING,
	KEY_UPED,
	KEY_UPING
	
}Key_State;




#define KEY_READ_IO 40
#define KEY_CHANGED 41





typedef uint8_t GPIO_PinState;
typedef struct KeyBoard_t Keyscan_Type;


typedef uint8_t (*Key_Callback)(Keyscan_Type *myself,Key_State pt_t);
typedef GPIO_PinState (*Key_Read_cb)(Keyscan_Type *myself);

struct KeyBoard_t
{
	GPIO_PinState FirstPinState;
	GPIO_PinState LastPinState;
	
	Key_Read_cb key_read_cb;//回调函数,模块通过这个函数获取IO口状态，
	Key_Callback key_cb;
}; //按键对象结构体
void KeyBoard_Init(Keyscan_Type *myself,Key_Read_cb key_read_cb,Key_Callback key_cb);
Key_State KeyScan(Keyscan_Type *myself);                        //键盘扫描


#endif
