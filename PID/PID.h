#ifndef _PID_H_
#define _PID_H_

#ifdef __cplusplus
		extern "C"{
#endif
/*****************
			
			作者:Suaig
			2018-2
			版本1.0
			2018-4-27
			版本1.1
			修复一个BUG，因为变量未初始化导致输出异常
			修复了一个BUG，调用PID_SetSampleTime函数不改变ki,kd时间系数问题
			
			PS:移植来着arduino PID库
			网址:http://playground.arduino.cc/Code/PIDLibrary
			原库是C++写的，通过C面向对象改写，保留全部功能
******************/
			
			
//是否自动运算
#define AUTOMATIC	1 //到时间自动运算
#define MANUAL	0		//手动
			
#define DIRECT  0  //PID输出增加 控制量增加
#define REVERSE  1 //PID输出增加，控制量减少  参考空调制冷，输出功率增加，空调制冷增强，越冷
			
#define P_ON_M 0	//控制超调(不会出现超调，或者超调小，上升慢)
#define P_ON_E 1	//正常模式(有超调，上升更快)


extern unsigned long millis();
			
			
#define PIDmillis millis //获取系统时间

			
typedef double PID_type;//如需在程序切换PID方向，类型必须为有符号		
typedef unsigned char bool;      			
typedef unsigned long Timetick;//时间函数



#define false 0
#define true  1
			
typedef struct
{
	PID_type dispKp;				// * we'll hold on to the tuning parameters in user-entered 
	PID_type dispKi;				//   format for display purposes
	PID_type dispKd;				//
    
	PID_type kp;                  // * (P)roportional Tuning Parameter 比例环节系数
  PID_type ki;                  // * (I)ntegral Tuning Parameter 积分环节系数
  PID_type kd;                  // * (D)erivative Tuning Parameter 微分环节系数

	bool controllerDirection;		//DIRECT  0  : PID输出增加 控制量增加
															//REVERSE  1 : PID输出增加，控制量减少  参考空调制冷，输出功率增加，空调制冷增强，越冷
	bool pOn;

  PID_type *myInput;              // * Input, Output, and Setpoint 变量的指针
  PID_type *myOutput;             //   通过这三个指针紧密地联系了变量和PID
  PID_type *mySetpoint;           //   而不需要编程者持续关注变量的值
                                  //   使用指针就能取到了
	//Input: 采集到的量，作为PID输入 Output:PID计算得到的输出（如DA的D值，PWM中的定时器CCR值，用于控制输出） Setpoint：期望的输出值
	Timetick lastTime;							//上次进行完PID计算的时间点
	PID_type outputSum, lastInput;	//outputSum:用于存储积分环节（由于积分与历史值相关，需要存储）
																	//lastInput:用于存储上次采集值，用于计算△error
	Timetick SampleTime;						//PID计算周期（等同于有效采样频率）
	PID_type outMin, outMax;				//输出限幅（用于限制PID调节能力，防止过大的调节）
	
	bool inAuto, pOnE;							//inAuto:用于设置PID工作模式		AUTOMATIC(1):自动模式，由函数自动实现PID计算
																	//												 		MANUAL(0):手动模式 需要自己实现PID计算
																	//pOnE:设置是否开启超调        P_ON_M(0):控制超调(不会出现超调，或者超调小，上升慢)
																	//														P_ON_E(1):正常模式(有超调，上升更快)
}PID_Class;	
/*
//输入参数 1,pid对象指针，2,输入的指针 3,输出变量的指针 4.期望值指针 5，6，7是Kp，Ki，Kd的值，8.pon是控制类型，是否启用控制超调,9.PID控制输出方向
*/
void new_PID(PID_Class *mpid,PID_type* Input,PID_type* Output,PID_type* Setpoint,PID_type Kp,PID_type Ki,PID_type Kd,bool POn, bool ControllerDirection);//初始化一个新的PID类	
			
//设置工作模式，自动还是手动  1,pid对象指针 2.模式			
void PID_SetMode(PID_Class *mpid,bool Mode);			

//设置PID 的基本参数
void PID_SetTunings(PID_Class *mpid,PID_type Kp, PID_type Ki, PID_type Kd, bool POn);			

//设置PID的方向
void PID_SetControllerDirection(PID_Class *mpid,bool Direction);			
//设置PID的输出上下限
void PID_SetOutputLimits(PID_Class *mpid,PID_type Min, PID_type Max);			
//初始化PID
void PID_Initialize(PID_Class *mpid);

//计算PID，成功计算一次1返回true
bool PID_Compute(PID_Class *mpid);

//设置计算周期，单位是ms
void PID_SetSampleTime(PID_Class *mpid,Timetick NewSampleTime);


#ifdef __cplusplus
}
#endif

/*
使用举例
idata PID_Class adcpid;

void main( void )
{
	
	double adcinput = 0,adcoutput = 1,setpid = 2;
	TMOD |= 0x01;
	TH0 = (65536-1000)/256;
	TL0 = (65536-1000)%256;
	TR0 = 1;
	ET0 = 1;
	EA = 1;
	
	new_PID(&adcpid,&adcinput,&adcoutput,&setpid,1,1,1,P_ON_E,DIRECT);
	PID_SetMode(&adcpid,AUTOMATIC);
  while(1)
	{
		PID_Compute(&adcpid);
	}
	
	特别注意，必须完成获取系统时间的函数millis 因为PID是按时间去计算调用，Ki，Kp,Kd的值也和时间息息相关，具体可以去看PID的视频，
*/
#endif