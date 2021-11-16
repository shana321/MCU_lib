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
void delay(__IO Timetick t) //不优化，发现经过编译器优化后，有时候会导致延时失效的问题。
{
    Timetick tickstart= 0;
    tickstart = millis();
    while((millis() - tickstart) < t)
    {

    }
}
#endif