#include "PT_OS.H"
Timetick ntimer=0;


Timetick millis()
{
	unsigned int m;
	__PT_DISABLE_IRQ();
	m = ntimer;
	__PT_ENABLE_IRQ();
	return m;
}

void SystickCounter ()
{
	__PT_DISABLE_IRQ();
    ntimer++;
	__PT_ENABLE_IRQ();
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
