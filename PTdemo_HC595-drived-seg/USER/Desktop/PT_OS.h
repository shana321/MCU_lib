/******************************************************************
*		���ܣ�Э��ʽ�߳̿�
*		���ߣ�Sauig
*		ʱ�䣺2017-11-15
*	
*
*		PS:����PT�̼߳��϶�ʱ������ʱ����д�������ϴ���д��OS_Tα�߳�˼��һ��
*		���Ǿ���ʵ�ִ������в��
*******************************************************************/
#ifndef _PT_OS_H_
#define _PT_OS_H_

#include "main.h"

#define SWITCH_POINT  											 1u     //ANSI C��������ʹ��switch״̬��ʵ�֣� GNU C ����ʹ��goto�����ת     0��GNU 1��ANSI C
#define FDDLAY															 0u

typedef uint16_t Timetick; //���ʱ��Ƭ�����ɴ��С������ȡ32λ�������ʱ50�졣�����uint��ô���ʱ 65��

#define OS_EN()		    bit cpu_sr = EA;
#define cli()  			EA = 0 //���ж�
#define OS_Exit()		EA = cpu_sr

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

/* ��ʼ��һ��Э�̣�Ҳ����ʼ��״̬���� */
#define PT_INIT(pt)   LC_INIT((pt)->lc)

/* ����һ������������ֵΪ char ���˳��룬��ʾ��������ʹ���� proto thread (�Ǳ�Ҫ)*/
#define PT_THREAD(name_args) char name_args
	
/* Э����ڵ㣬 PT_YIELD_FLAG=0��ʾ���ã�=1��ʾ�����ã����� switch ���ǰ�棬�´ε��õ�ʱ�������ת���ϴγ��õ����ִ�� */
#define PT_BEGIN(pt) { char PT_YIELD_FLAG = 1; LC_RESUME((pt)->lc)
	

/* Э���˳��㣬����һ��Э��������ֹ�ˣ�������������ĺͱ�־ */
#define PT_END(pt) LC_END((pt)->lc); PT_YIELD_FLAG = 0; \
                  PT_INIT(pt); return PT_ENDED; }
//����ȴ�ֱ��ĳ�¼�����
#define PT_WAIT_UNTIL(pt, condition)	        \
  do {						\
    LC_SET((pt)->lc);				\
    if(!(condition)) {				\
      return PT_WAITING;			\
    }						\
  } while(0)

//������ʱ,����������ƿ����ʱ
#define PT_TIME_DELAY(pt, time)	        \
  do {						\
		(pt)->t = millis();								\
    LC_SET((pt)->lc);				\
    if((millis()-(pt->t))<time) {				\
      return PT_WAITING;			\
    }						\
  } while(0)
//����ȴ�ĳ�¼����������еȴ���ʱ
#define PT_TIME_WAIT_TIMEOUT(pt,condition,time)		\
	do {												\
		(pt)->t = millis();								\
		LC_SET((pt)->lc);		\
		if( (!condition) && ((millis()-(pt->t))<(time))) {				\
      return PT_WAITING;			\
    }	         	\
	}while(0)
	

/*��PT_WAIT_UNTIL�����෴*/
#define PT_WAIT_WHILE(pt, cond)  PT_WAIT_UNTIL((pt), !(cond))

/* �����ڷǶԳ�Э�̣�����������Э�̣�pt �Ǻ���Э�� thread �������Ƕ���������������ľ������Э�������Լ�������Э�̣�ֱ��������Э����ֹ */

#define PT_WAIT_THREAD(pt, thread) PT_WAIT_WHILE((pt), PT_SCHEDULE(thread))


/* ����Э��Ƕ�׵��ȣ�child ����Э�̵������ľ�� */
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
	
	
//�����˳�����������ƿ�
#define PT_EXIT(pt)				\
  do {						\
    PT_INIT(pt);				\
    return PT_EXITED;			\
  } while(0)
	

/* Э�̵��ȣ�����Э�� f ����������˳��룬ֱ��Э����ֹ���� 0�����򷵻� 1�� */
	
#define PT_SCHEDULE(f) ((f) < PT_EXITED)
	
	
//�����ͷ�CPU ����������ƿ�	
#define PT_YIELD(pt)				\
  do {						\
    PT_YIELD_FLAG = 0;				\
    LC_SET((pt)->lc);				\
    if(PT_YIELD_FLAG == 0) {			\
      return PT_YIELDED;			\
    }						\
  } while(0)



//����������㣬��ô�ͷ�CPU
#define PT_YIELD_UNTIL(pt, cond)		\
  do {						\
    PT_YIELD_FLAG = 0;				\
    LC_SET((pt)->lc);				\
    if((PT_YIELD_FLAG == 0) || !(cond)) {	\
      return PT_YIELDED;			\
    }						\
  } while(0)
	
/**********************
�ź������	
********************/
typedef struct pt_sem_ {
  uint16_t count;
}pt_sem;

//�ź�����ʼ�������η����źŴ���
#define PT_SEM_INIT(s, c) (s)->count = c

//����ʱ�ĵȴ��ź���
#define PT_SEM_WAIT_TIMEOUT(pt, s,time)	\
  do {						\	
		PT_TIME_WAIT_TIMEOUT(pt,(s)->count == 0,time);    \			
  } while(0)
	
//���޳��ȴ��ź�
#define PT_SEM_WAIT(pt,s)   \
	do{          \
		PT_WAIT_UNTIL(pt,(s)->count == 0);\
	}while(0)
	
//�����ź���	
#define PT_SEND_SEM(pt,s) \
	do{        \
			--(s)->count;       \
		}while(0)
	
		
/**********************
�������	
********************/
typedef struct pt_pos_ {
  void *poscard;
}pt_pos;


#define PT_POS_INIT(s) (s)->poscard = NULL;


//���޳��ȴ�����
#define PT_POS_WAIT(pt,s)   \
	do{          \
		PT_WAIT_UNTIL(pt,(s)->poscard != NULL);\
	}while(0)
//��ȡ���䲢���
#define PT_POS_GET(pt,s)  		\
	(s)->poscard;			\
	(s)->poscard = NULL;
//��������
#define PT_SEND_POS(pt,s,t) \
	do{        \
			(s)->poscard = t;       \
		}while(0)
	
/**********************************************��������*****************************************************
* ��������: millis
* �������: NULL
* ���ز���: Timetick ʱ��Ƭ
* ��    ��: ����ϵͳ��������ʼ���������߹���ʱ�� ��ʱ��Ƭ����
* ��    ��:
* ��    ��:
************************************************************************************************************/
Timetick millis();
void SystickCounter ();


/**********************************************��������*****************************************************
* ��������: delay
* �������: 1.Timetick ʱ��Ƭ��
* ���ز���: NULL
* ��    ��: ��ʱһ����ʱ��Ƭ��ʱ��Ƭ��ʼһ����1ms,���Լ򵥵���Ϊ��ʱ����ms
* ��    ��:
* ��    ��:
************************************************************************************************************/
#if FDDLAY > 0u
void delay(__IO Timetick t);
#endif
#endif

