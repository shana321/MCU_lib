#include "PT_OS.H"
Timetick ntimer=0;


unsigned int millis()
{
	unsigned int m;
	OS_EN();
	cli();
	m = timer;
	OS_Exit();
	return m;
}

void SystickCounter ()
{
	OS_EN();
	cli();
    ntimer++;
	OS_Exit();
}


#if FDDLAY > 0u
void delay(__IO Timetick t) //���Ż������־����������Ż�����ʱ��ᵼ����ʱʧЧ�����⡣
{
    Timetick tickstart= 0;
    tickstart = millis();
    while((millis() - tickstart) < t)
    {

    }
}
#endif