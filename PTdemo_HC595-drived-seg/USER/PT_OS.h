/******************************************************************
*		功能：协作式线程库
*		作者：Sauig
*		时间：2017-11-15
*	
*
*		PS:基于PT线程加上定时器调度时基所写，和我上次所写的OS_T伪线程思想一样
*		但是具体实现代码略有差别
*******************************************************************/
#ifndef _PT_OS_H_
#define _PT_OS_H_

#include "stdint.h"
#include "stm32f4xx.h"

#define SWITCH_POINT  											 1u     //ANSI C仅仅可以使用switch状态机实现， GNU C 可以使用goto语句跳转     0是GNU 1是ANSI C
#define FDDLAY															 0u

typedef uint16_t Timetick; //最大时间片数，可大可小，这里取32位。最长可延时50天。如果是uint那么最长延时 65秒


#define __PT_DISABLE_IRQ()  __disable_irq()
#define __PT_ENABLE_IRQ()   __enable_irq()

#undef NULL
#if defined(__cplusplus)
#define NULL 0
#else
#define NULL ((void *)0)
#endif



#if SWITCH_POINT > 0
typedef uint8_t lc_t;

#define LC_INIT(s) s = 0;

#define LC_RESUME(s) switch(s) { case 0:

#define LC_SET(s) s = __LINE__; case __LINE__:

#define LC_END(s) }

#else

typedef void * lc_t;

#define LC_INIT(s) s = NULL

#define LC_RESUME(s)				\
  do {						\
    if(s != NULL) {				\
      goto *s;					\
    }						\
  } while(0)

#define LC_CONCAT2(s1, s2) s1##s2
#define LC_CONCAT(s1, s2) LC_CONCAT2(s1, s2)

#define LC_SET(s)				\
  do {						\
    LC_CONCAT(LC_LABEL, __LINE__):   	        \
    (s) = &&LC_CONCAT(LC_LABEL, __LINE__);	\
  } while(0)

#define LC_END(s)	 
	
#endif
typedef struct pt_ {
  lc_t lc;
	Timetick t;
}pt;

#define PT_WAITING 0
#define PT_YIELDED 1
#define PT_EXITED  2
#define PT_ENDED   3

/* 初始化一个协程，也即初始化状态变量 */
#define PT_INIT(pt)   LC_INIT((pt)->lc)

/* 声明一个函数，返回值为 char 即退出码，表示函数体内使用了 proto thread (非必要)*/
#define PT_THREAD(name_args) char name_args
	
/* 协程入口点， PT_YIELD_FLAG=0表示出让，=1表示不出让，放在 switch 语句前面，下次调用的时候可以跳转到上次出让点继续执行 */
#define PT_BEGIN(pt) { char PT_YIELD_FLAG = 1; LC_RESUME((pt)->lc)
	

/* 协程退出点，至此一个协程算是终止了，清空所有上下文和标志 */
#define PT_END(pt) LC_END((pt)->lc); PT_YIELD_FLAG = 0; \
                  PT_INIT(pt); return PT_ENDED; }
//任务等待直到某事件发生
#define PT_WAIT_UNTIL(pt, condition)	        \
  do {						\
    LC_SET((pt)->lc);				\
    if(!(condition)) {				\
      return PT_WAITING;			\
    }						\
  } while(0)

//任务延时,传递任务控制块和延时
#define PT_TIME_DELAY(pt, time)	        \
  do {						\
		(pt)->t = millis();								\
    LC_SET((pt)->lc);				\
    if((millis()-(pt->t))<time) {				\
      return PT_WAITING;			\
    }						\
  } while(0)
//任务等待某事件发生，并有等待超时
#define PT_TIME_WAIT_TIMEOUT(pt,condition,time)		\
	do {												\
		(pt)->t = millis();								\
		LC_SET((pt)->lc);		\
		if( (!condition) && ((millis()-(pt->t))<(time))) {				\
      return PT_WAITING;			\
    }	         	\
	}while(0)
	

/*和PT_WAIT_UNTIL条件相反*/
#define PT_WAIT_WHILE(pt, cond)  PT_WAIT_UNTIL((pt), !(cond))

/* 这用于非对称协程，调用者是主协程，pt 是和子协程 thread （可以是多个）关联的上下文句柄，主协程阻塞自己调度子协程，直到所有子协程终止 */

#define PT_WAIT_THREAD(pt, thread) PT_WAIT_WHILE((pt), PT_SCHEDULE(thread))


/* 用于协程嵌套调度，child 是子协程的上下文句柄 */
#define PT_SPAWN(pt, child, thread)		\
  do {						\
    PT_INIT((child));				\
    PT_WAIT_THREAD((pt), (thread));		\
  } while(0)
	
	
#define PT_RESTART(pt)				\
  do {						\
    PT_INIT(pt);				\
    return PT_WAITING;			\
  } while(0)
	
	
//任务退出传递任务控制块
#define PT_EXIT(pt)				\
  do {						\
    PT_INIT(pt);				\
    return PT_EXITED;			\
  } while(0)
	

/* 协程调度，调用协程 f 并检查它的退出码，直到协程终止返回 0，否则返回 1。 */
	
#define PT_SCHEDULE(f) ((f) < PT_EXITED)
	
	
//任务释放CPU 传递任务控制块	
#define PT_YIELD(pt)				\
  do {						\
    PT_YIELD_FLAG = 0;				\
    LC_SET((pt)->lc);				\
    if(PT_YIELD_FLAG == 0) {			\
      return PT_YIELDED;			\
    }						\
  } while(0)



//如果条件满足，那么释放CPU
#define PT_YIELD_UNTIL(pt, cond)		\
  do {						\
    PT_YIELD_FLAG = 0;				\
    LC_SET((pt)->lc);				\
    if((PT_YIELD_FLAG == 0) || !(cond)) {	\
      return PT_YIELDED;			\
    }						\
  } while(0)
	
/**********************
信号量相关	
********************/
typedef struct pt_sem_ {
  uint16_t count;
}pt_sem;

//信号量初始化，几次发送信号触发
#define PT_SEM_INIT(s, c) (s)->count = c

//带超时的等待信号量
#define PT_SEM_WAIT_TIMEOUT(pt, s,time)	\
  do {\
		PT_TIME_WAIT_TIMEOUT(pt,(s)->count == 0,time);\
  } while(0)
	
//无限长等待信号
#define PT_SEM_WAIT(pt,s)   \
	do{          \
		PT_WAIT_UNTIL(pt,(s)->count == 0);\
	}while(0)
	
//发送信号量	
#define PT_SEND_SEM(pt,s) \
	do{        \
			--(s)->count;       \
		}while(0)
	
		
/**********************
邮箱相关	
********************/
typedef struct pt_pos_ {
  void *poscard;
}pt_pos;


#define PT_POS_INIT(s) (s)->poscard = NULL;


//无限长等待邮箱
#define PT_POS_WAIT(pt,s)   \
	do{          \
		PT_WAIT_UNTIL(pt,(s)->poscard != NULL);\
	}while(0)
//获取邮箱并清除
#define PT_POS_GET(pt,s)  		\
	(s)->poscard;			\
	(s)->poscard = NULL;
//发送邮箱
#define PT_SEND_POS(pt,s,t) \
	do{        \
			(s)->poscard = t;       \
		}while(0)
	
/**********************************************函数定义*****************************************************
* 函数名称: millis
* 输入参数: NULL
* 返回参数: Timetick 时间片
* 功    能: 返回系统从启动开始到现在所走过的时间 以时间片计算
* 作    者:
* 日    期:
************************************************************************************************************/
Timetick millis(void);
void SystickCounter(void);


/**********************************************函数定义*****************************************************
* 函数名称: delay
* 输入参数: 1.Timetick 时间片数
* 返回参数: NULL
* 功    能: 延时一定的时间片，时间片初始一般是1ms,可以简单的作为延时多少ms
* 作    者:
* 日    期:
************************************************************************************************************/
#if FDDLAY > 0u
void delay(__IO Timetick t);
#endif
#endif

