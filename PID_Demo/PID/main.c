#include <reg51.h>

#include "PID.h"



typedef unsigned long uint32_t;	//建议使用的标准类型名
typedef unsigned int uint16_t;	//建议使用的标准类型名
typedef unsigned char uint8_t;	//建议使用的标准类型名

uint32_t millis();


uint32_t tick = 0;


idata PID_Class adout;	//idata，51架构特有，表示数据存在片内SRAM


uint8_t PWM = 0;
uint16_t Roll_Number = 0,Roll_Clone = 0;

void main()
{
	double input,output,set = 200;
	//AUXR &= 0x7F;		//定时器时钟12T模式
	TMOD &= 0xF0;		//设置定时器模式
	TMOD |= 0x01;		//设置定时器模式
	TL0 = 0x18;		//设置定时初值
	TH0 = 0xFC;		//设置定时初值
	TF0 = 0;		//清除TF0标志
	TR0 = 1;		//定时器0开始计时
	
	ET0 = 1;
	
	
	TMOD &= 0x0F;		//设置定时器模式
	TMOD |= 0x20;		//设置定时器模式
	TL1 = 0x06;		//设置定时初值
	TH1 = 0x06;		//设置定时重载值
	TF1 = 0;		//清除TF1标志
	TR1 = 1;		//定时器1开始计时
	
	ET1 = 1;
	
	
	IT0 = 1;
	EX0 = 1;
	
	
	EA = 1;
	
	/********************************
	首先，怎么确定计算周期，大概是系统从输出控制到上升到输出控制的60%-80%之间的时间(可以视系统不同而调节)
	eg:举例子:加热器:我输出一个控制 比如是X,经过无限长的时间，温度停留在Y，那么大概是取温度上升到0.6Y-0.8Y的这个时间，
	由于有些系统非线性程度很高是幂函数的变化，这个时候，要达到最好的控制效果，我会增加计算的时间间隔
	
	确定Kp,Ki，Kd，一般用不到Kd就可以有很好的控制效果，Kd我一般用于有大滞后的系统，例如控制温度，
	Kd可以在温度还没有到达设置值的时候就减少控制输出
	
	首先，把Ki，Kd设置成0，不断加大Kp直到产生振荡，作为启振点，
	然后:Ki慢慢加，加到开始振荡，取其80%-90%
	
	如果系统控制严重滞后，慢慢的增加Kd的值，使得第一次超调和第二次的峰比4比1是最理想的
	
	PID完整离散公式:OUT = kp * error + (ki * T)/Ti * ∑error + kd *Td/T * △error

	T:PID计算采样周期，Ti:积分时间 Td:微分时间 error:采样和期望值的差    ∑error:累积的error 

	△error:两次error的差	
	
	*********************************/
	new_PID(&adout,&input,&output,&set,1.4,0.1,0,P_ON_E,DIRECT);//正常模式，正相关
	PID_Initialize(&adout);//初始化
	
	PID_SetMode(&adout,AUTOMATIC);//自动计算模式
	
	PID_SetSampleTime(&adout,3000);//设置PID计算周期为5000ms
	while(1)
	{
		input = (double)Roll_Number;//采样转速周期
		if(PID_Compute(&adout) == true)//大概五次采样计算一次
		{
			PWM = (uint8_t)output;
		}
	}
}
uint32_t millis()	//给PID控制提供时间信息 tick 1ms自增一次（TIMER0中）
{
	return tick;
}
void EXIT0_IRQ() interrupt 0		//PID控制中的采样机构，通过外部中断采样电机转速
{
	Roll_Clone++;
}
void Timer1_IRQ() interrupt 3		//PID控制中的执行机构，i相当于STM32中的定时器计数CNT值，PWM相当于CCR值，比较产生PWM波，改PWM变量改变占空比
{
	static uint8_t i=0;
	if(PWM >= i)	//P2.0上产生一定占空比PWM
	{
		P2 &= ~(0x01);//P2.0低电平 电机减速
	}
	else
	{
		P2 |= 0x01;//P2.0高电平 电机加速
	}
	i++;	//计数值增
}
void Timer0_IRQ() interrupt 1	//1ms
{
	static uint16_t i = 0;
	TL0 = 0x18;		//设置定时初值
	TH0 = 0xFC;		//设置定时初值
	
	i++;
	tick++;
	if(i == 1000)	//1s采样一次
	{
		i=0;
		Roll_Number = Roll_Clone;	//1s转过圈数	
		Roll_Clone = 0;
	}
}