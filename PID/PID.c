#include "PID.h"
//新建一个PID对象，函数参数对应对象的属性
//mpid:PID对象
//Input:采集量
//Output:输出控制量
//Setpoint:设置期望值
//Kp:比例环节系数
//Ki:积分环节系数
//Kd:微分环节系数
//Pon:控制模式
//ControllerDirection:控制方向
//实际写程序的时候调PID，其实就是调Kp，Ki，Kd，sampletime四个参数
/********************************
	首先，怎么确定计算周期，大概是系统从输出控制到上升到输出控制的60%-80%之间的时间(可以视系统不同而调节)
	eg:举例子:加热器:我输出一个控制比如是X,经过无限长的时间，温度停留在Y，那么大概是取温度上升到0.6Y-0.8Y的这个时间，
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
void new_PID(PID_Class *mpid,PID_type* Input,PID_type* Output,PID_type* Setpoint,PID_type Kp,PID_type Ki,PID_type Kd,bool POn, bool ControllerDirection)
{
    mpid->myOutput = Output;//myOutput属性
    mpid->myInput = Input;	//myInput属性
    mpid->mySetpoint = Setpoint;//mySetpoint属性
    mpid->inAuto = false;//inAuto属性 默认为手动模式

    PID_SetOutputLimits(mpid,0,255);				//默认输出限幅，255是根据arduino单片机设置的

    mpid->SampleTime = 200;							//默认计算间隔0.2s（200ms），这里需要给PID提供1ms的计数变量，200才对应200ms，也就是实现PIDmillis()函数

    PID_SetControllerDirection(mpid,ControllerDirection);//设置控制方向
																													//DIRECT  0  : PID输出增加 控制量增加
																													//REVERSE  1 : PID输出增加，控制量减少  参考空调制冷，输出功率增加，空调制冷增强，越冷
    PID_SetTunings(mpid,Kp, Ki, Kd, POn);	//设置PID计算式各项系数以及控制模式是超调还是正常模式

    mpid->lastTime = PIDmillis()-(mpid->SampleTime);	//初始化lastTime，使第一次可以直接开始PID计算
}
//设置PID计算式各项系数以及控制模式是超调还是正常模式
//mpid:PID对象
//Kp:比例环节系数
//Ki:积分环节系数
//Kd:微分环节系数
//POn:控制模式 	P_ON_M(0)	控制超调(不会出现超调，或者超调小，上升慢)
//							P_ON_E(1)	正常模式(有超调，上升更快)
void PID_SetTunings(PID_Class *mpid,PID_type Kp, PID_type Ki, PID_type Kd, bool POn)
{
		PID_type SampleTimeInSec = 0;
   if (Kp<0 || Ki<0 || Kd<0) return;

   //mpid->pOn = POn;
   mpid->pOnE = POn;

   mpid->dispKp = Kp; mpid->dispKi = Ki; mpid->dispKd = Kd;

   SampleTimeInSec = ((PID_type)mpid->SampleTime)/1000;
   mpid->kp = Kp;
   mpid->ki = Ki * SampleTimeInSec;
   mpid->kd = Kd / SampleTimeInSec;

  if(mpid->controllerDirection ==REVERSE)
   {
      mpid->kp = (0 - mpid->kp);
      mpid->ki = (0 - mpid->kp);
      mpid->kd = (0 - mpid->kp);
   }
}
//设置PID计算周期
//mpid:PID对象
//NewSampleTime:计算周期为多少个时间片（PIDmillis()值1对应多长时间，一般取1ms）
void PID_SetSampleTime(PID_Class *mpid,Timetick NewSampleTime)
{
	PID_type SampleTimeInSec = 0;
   if (NewSampleTime > 0)
   {
      PID_type ratio  = (PID_type)NewSampleTime
                      / (PID_type)mpid->SampleTime;
      mpid->ki *= ratio;
      mpid->kd /= ratio;
      mpid->SampleTime = (Timetick)NewSampleTime;
		 
	 

   SampleTimeInSec = ((PID_type)mpid->SampleTime)/1000;
   mpid->kp = mpid->dispKp;
   mpid->ki = mpid->dispKi * SampleTimeInSec;
   mpid->kd = mpid->dispKd / SampleTimeInSec;//配置PID系数
		 
	 mpid->lastTime = PIDmillis()-(mpid->SampleTime);
   }
}
//设置PID输出限幅大小
//mpid:PID对象
//Min:输出最小值限幅
//Max:输出最大值限幅
void PID_SetOutputLimits(PID_Class *mpid,PID_type Min, PID_type Max)
{
   if(Min >= Max) return;
   mpid->outMin = Min;
   mpid->outMax = Max;

   if(mpid->inAuto)
   {
	   if(*(mpid->myOutput) > (mpid->outMax)) *(mpid->myOutput) = (mpid->outMax);
	   else if(*(mpid->myOutput) < (mpid->outMin)) *(mpid->myOutput) = (mpid->outMin);

	   if((mpid->outputSum) > mpid->outMax) (mpid->outputSum)= mpid->outMax;
	   else if((mpid->outputSum) < mpid->outMin) mpid->outputSum= mpid->outMin;
   }
}
//设置PID模式（自动或手动）
//mpid:PID对象
//Mode: AUTOMATIC(1)到时间自动运算
//			MANUAL	(0)	手动
void PID_SetMode(PID_Class *mpid,bool Mode)
{
    bool newAuto = (Mode == AUTOMATIC);
    if(newAuto && !(mpid->inAuto))
    {  
        PID_Initialize(mpid);
    }
    mpid->inAuto = newAuto;
}
//PID对象初始化
void PID_Initialize(PID_Class *mpid)
{
   mpid->outputSum = *(mpid->myOutput);
   mpid->lastInput = *(mpid->myInput);
   if(mpid->outputSum > mpid->outMax) mpid->outputSum = mpid->outMax;
   else if(mpid->outputSum < mpid->outMin) mpid->outputSum = mpid->outMin;
}
//设置PID控制方向
//Direction:	DIRECT  0  : PID输出增加 控制量增加
//						REVERSE  1 : PID输出增加，控制量减少  参考空调制冷，输出功率增加，空调制冷增强，越冷
void PID_SetControllerDirection(PID_Class *mpid,bool Direction)
{
   if(mpid->inAuto && Direction !=mpid->controllerDirection)
   {
	    mpid->kp = (0 - mpid->kp);
      mpid->ki = (0 - mpid->ki);
      mpid->kd = (0 - mpid->kd);
   }
   mpid->controllerDirection = Direction;
}

//PID计算，在while中不断调用
bool PID_Compute(PID_Class *mpid)
{
	 PID_type output = 0;
	Timetick now;					//存储这次运行函数的时间点
	Timetick timeChange;	//存储上次执行完PID和这次进入函数的时间差
   if(!(mpid->inAuto))	//手动计算模式下该函数失效，直接返回false
	 {
		 return false;
	 }
	 now = PIDmillis();		//读取时间变量
   timeChange = (now - mpid->lastTime);	//得到时间差
   if(timeChange>=(mpid->SampleTime))	//如果时间差大于等于计算间隔，则开始PID计算
   {
      /*Compute all the working error variables*/
      PID_type input = *(mpid->myInput);	//采集到的输入
      PID_type error = *(mpid->mySetpoint) - input;	//采集值与期望值的偏差
      PID_type dInput = (input - mpid->lastInput);	//这次采集值与上次采集值的差，即△error
      mpid->outputSum+= (mpid->ki * error);	//积分环节 由于积分环节与历史有关，故outputSum受历史值影响

      /*Add Proportional on Measurement, if P_ON_M is specified*/
      if(!(mpid->pOnE)) (mpid->outputSum)-= (mpid->kp) * dInput;	//P_ON_M模式（控制超调）下 积分环节加入超调控制

      if((mpid->outputSum) > (mpid->outMax)) (mpid->outputSum)= (mpid->outMax);	//这是带有限幅功能的PID（防止调节过大），超过最大限值时，取最大限值
      else if((mpid->outputSum) < (mpid->outMin)) (mpid->outputSum)= (mpid->outMin);	//低于最小限值时，取最小限值

      /*Add Proportional on Error, if P_ON_E is specified*/
      if(mpid->pOnE) output = mpid->kp * error;										//P_ON_E模式(正常模式)下，output直接赋值比例环节部分
      else output = 0;																						//P_ON_M模式(控制超调)下，由于在积分环节中加入了比例环节控制，这里output直接赋值0

      /*Compute Rest of PID Output*/ //计算PID式的剩余项（kd，微分环节） 
      output += mpid->outputSum - mpid->kd * dInput;	//P_ON_E模式下，output在执行这条语句之前包括了kp（比例环节）部分，outputSum包括了积分部分（与历史值有关）
																											//P_ON_M模式下，output为0，outputSum是受比例环节影响的积分部分，也就是包括了两者
	    if(output > mpid->outMax) output = mpid->outMax;	//最大值限幅
      else if(output < mpid->outMin) output = mpid->outMin;	//最小值限幅
	    *(mpid->myOutput) = output;	//存储output计算结果

      /*Remember some variables for next time*/
      mpid->lastInput = input;	//保存此次采集值。以便下次求△error
      mpid->lastTime = now;	//保存此次计算完毕的时间点，以便下次判断是否到了计算周期
	    return true;		//计算完毕，返回true
   }
   else return false;	//未到计算周期，直接返回false
}