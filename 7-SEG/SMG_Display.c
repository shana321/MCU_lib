#include "SMG_Display.h"
#include "string.h"


void Put2Buff(struct _smg_* s,uint8_t c)
{
    uint8_t i=0;
    while(s->DisBuff[i] != '\0') {
        i++;
    };
    s->DisBuff[i] = c;
}
void DisPlay(struct _smg_* s)
{
    uint16_t duan = 0;
    if(s->DisBuff[s->str_l] == '\0')
    {
				s->har_smg_cb(s,SMG_MSG_BLANK,&s->SMG_L);//先消隐指定位
        //s->SMG_Blank(s->SMG_L);
				s->har_smg_cb(s,SMG_MSG_WEI,&s->SMG_L);//送位码
        //s->SendWei2SMG(s->SMG_L);
				//(s->SMG_L >= (s->MAX_SMG_LONG-1)) ? (SMG_gohome(s)) : (s->SMG_L++);
				if((s->SMG_L) >= (s->MAX_SMG_LONG-1))
				{
					SMG_gohome(s);
				}
				else
				{
					(s->SMG_L++);
				}
        return;
    }
		duan = s->har_smg_cb(s,SMG_MSG_HASH,&s->DisBuff[s->str_l]);
    //duan = s->hash(s->DisBuff[s->str_l]);
    if(duan == 0xFFFF)
    {
        s->str_l++;
        return;//表中没有的元素
    }
    if(s->DisBuff[s->str_l+1] == '.')
    {
				duan &= 0x7F;
        duan |= ((s->har_smg_cb(s,SMG_MSG_HASH,"."))&0x80);
        s->str_l++;
    }

    s->har_smg_cb(s,SMG_MSG_BLANK,&s->SMG_L);//先消隐指定位
    s->har_smg_cb(s,SMG_MSG_WEI,&s->SMG_L);//送位码
		s->har_smg_cb(s,SMG_MSG_DUAN,&duan);//送段
    //s->(duan);//送段

    //((s->SMG_L) >= (s->MAX_SMG_LONG-1)) ? (SMG_gohome(s)) : (s->SMG_L++);
		if((s->SMG_L) >= (s->MAX_SMG_LONG-1))
		{
			SMG_gohome(s);
		}
		else
		{
			s->SMG_L++;
		}
    s->str_l++;
}
void SMG_gohome(struct _smg_* s)
{
    s->str_l = 0;
    s->SMG_L = 0;
}
void SMG_Clear(struct _smg_* s)
{
    uint8_t i=0;
    for(i=0; i<(s->MAX_BUFF); i++)
    {
        s->DisBuff[i]='\0';
    }
}

SMG_CLASS* NewSMG(struct _smg_* s,uint8_t MAXLONG,uint8_t* disbuff,uint8_t MAX_buff,smg_cb mysmg)
{
    s->MAX_SMG_LONG = MAXLONG;
    s->DisBuff = disbuff;
    s->MAX_BUFF = MAX_buff;
		s->har_smg_cb = mysmg;

    s->SMG_L = 0;
    s->str_l = 0;
	
		s->har_smg_cb(s,SMG_MSG_INIT,(void*)0);
    return  s;
}


