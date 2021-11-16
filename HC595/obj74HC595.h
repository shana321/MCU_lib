#ifndef _74HC595_
#define _74HC595_

#include "stdint.h"


#define MSG_SH_CP   40
#define MSG_ST_CP   41
#define MSG_ST_DS   42
#define MSG_HC_INIT 43

typedef unsigned int size_t;
typedef unsigned char bool;
#define false        0
#define true         !false	

typedef struct _hc5955_ SPI595_TypeStruct;


typedef uint8_t (*hc595_cb)(SPI595_TypeStruct *ser,uint8_t msg,uint8_t pt_r);
struct _hc5955_
{
	hc595_cb myhc595;
};



SPI595_TypeStruct* HAL_Init_SPI595(SPI595_TypeStruct * obj,hc595_cb	cb);
void HAL_OBJ595_Sendbytes(SPI595_TypeStruct * obj,uint8_t *pdate,size_t num,bool output);

#endif
