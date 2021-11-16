#ifndef _SMG_Display_H_
#define _SMG_Display_H_
#include "stdint.h"
#include "stm32f4xx.h"
/******************************************************************
*		功能：通用形数码管任意显示驱动
*		作者：Sauig
*		时间：2017-12-21
*
*
*		PS:底层接口用户实现回调。重写方法即可使用，
*		hash函数要求是一个散列函数，可以讲字符与数码管编码对应。
* 	SMG_Blank函数要求能消隐某一个数码管或者当前数码管
*		SendWei2SMG发送位码到数码管  可以是595串行，也可以是直接驱动的并行
*		SendDuan2SMG发送位码到数码管  可以是595串行，也可以是直接驱动的并行
*******************************************************************/
#define SMG_MSG_HASH   40
#define SMG_MSG_BLANK  41
#define SMG_MSG_WEI    42
#define SMG_MSG_DUAN   43
#define SMG_MSG_INIT   44

typedef struct _smg_ SMG_CLASS;
typedef uint16_t (*smg_cb)(SMG_CLASS *ser,uint8_t msg,void *pt_r);
struct _smg_
{
    uint8_t MAX_SMG_LONG;//数码管最大长度
    uint8_t SMG_L;			//数码管当前位置
    uint8_t *DisBuff;
    uint8_t str_l;
    uint8_t MAX_BUFF;

//    //需要重写的方法
//    uint8_t (*hash)(char cp);//输入字符返回段码,查询
//    void (*SMG_Blank)(uint8_t num);//消隐某位数码管
//    void (*SendWei2SMG)(uint8_t num);//输出数码管位置返回位码
//    void (*SendDuan2SMG)(uint8_t dat);//输出数码管位置返回位码
		smg_cb har_smg_cb;//回调接口

    //类的方法
};


SMG_CLASS* NewSMG(struct _smg_* s,
									uint8_t MAXLONG,
									uint8_t* disbuff,
									uint8_t MAX_buff,
									smg_cb mysmg);

void Put2Buff(struct _smg_* s,uint8_t c);//在指定位置数码管显示一个字符
void DisPlay(struct _smg_* s);						//显示
void SMG_gohome(struct _smg_* s);				//光标回到原点
void SMG_Clear(struct _smg_* s);

#endif
