#include "SMG_Display.h"
#include "string.h"

void Put2Buff(struct _smg_* s,uint8_t c);
void DisPlay(struct _smg_* s);
void SMG_gohome(struct _smg_* s);
void SMG_Clear(struct _smg_* s);

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
    uint8_t duan = 0;
    if(s->DisBuff[s->str_l] == '\0')
    {
        s->SMG_Blank(s->SMG_L);//先消隐指定位
        s->SendWei2SMG(s->SMG_L);//送位码
				(s->SMG_L >= (s->MAX_SMG_LONG-1)) ? (SMG_gohome(s)) : (s->SMG_L++);
        return;
    }
    duan = s->hash(s->DisBuff[s->str_l]);
    if(duan == 255)
    {
        s->str_l++;
        return;//表中没有的元素
    }
    if(s->DisBuff[s->str_l+1] == '.')
    {
        duan |= s->hash('.');
        s->str_l++;
    }

    s->SMG_Blank(s->SMG_L);//先消隐指定位
    s->SendWei2SMG(s->SMG_L);//送位码
    s->SendDuan2SMG(duan);//送段

    (s->SMG_L >= (s->MAX_SMG_LONG-1)) ? (SMG_gohome(s)) : (s->SMG_L++);
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

SMG_CLASS* NewSMG(struct _smg_* s,uint8_t MAXLONG,uint8_t* disbuff,uint8_t MAX_buff,uint8_t (*hash)(char cp),void (*SMG_Blank)(uint8_t num),void (*SendWei2SMG)(uint8_t num),void (*SendDuan2SMG)(uint8_t dat))
{
    s->MAX_SMG_LONG = MAXLONG;
    s->DisBuff = disbuff;
    s->MAX_BUFF = MAX_buff;
    s->hash = hash;
    s->SMG_Blank = SMG_Blank;
    s->SendWei2SMG = SendWei2SMG;
    s->SendDuan2SMG = SendDuan2SMG;


    s->Put2Buff = Put2Buff;
    s->DisPlay = DisPlay;
    s->SMG_gohome = SMG_gohome;
    s->SMG_Clear = SMG_Clear;

    s->SMG_L = 0;
    s->str_l = 0;
    return  s;
}


