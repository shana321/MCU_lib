#ifndef _HardwareSerial_h_
#define _HardwareSerial_h_
/*
模块功能




作者:Suaig
时间:2018/02/21
*/
#ifdef __cplusplus
				extern "C"{
#endif
					
//C99就包含
//#include <stdint.h>
					
					
#include "main.h"
					
					
#include "STC15F2K60S2.h"

#define AVRAM 1024  //定义总内存大小，关系到后面缓冲区大小

#define HAL_PRINTF            0u    //串口printf开关（和C标准库printf独立）

#ifndef SERIAL_TX_BUFFER_SIZE
	#if (AVRAM < 1023)
	#define SERIAL_TX_BUFFER_SIZE 16
	#else
	#define SERIAL_TX_BUFFER_SIZE 64
	#endif
#endif

#ifndef SERIAL_RX_BUFFER_SIZE
	#if (AVRAM < 1023)
	#define SERIAL_RX_BUFFER_SIZE 16
	#else
	#define SERIAL_RX_BUFFER_SIZE 64
	#endif
#endif


#if (SERIAL_TX_BUFFER_SIZE>256)
typedef uint16_t tx_buffer_index_t;
#else
typedef uint8_t tx_buffer_index_t;
#endif
#if  (SERIAL_RX_BUFFER_SIZE>256)
typedef uint16_t rx_buffer_index_t;
#else
typedef uint8_t rx_buffer_index_t;
#endif

typedef struct HardwareSerial_struct HardwareSerial_t;
typedef uint8_t (*hard_msg_cb)(HardwareSerial_t *ser,uint8_t msg);
	

#define HSER_MSG_INIT          40
#define HSER_MSG_END           41
#define HSER_MSG_SEND_RI     	 42		//返回接受完成标志位
#define HSER_MSG_WRITE_TI      43   //返回发送完成标志位
#define HSER_MSG_EA            44		//返回总中断标志位
#define HSER_MSG_ES            45		//返回串口中断标志位

struct HardwareSerial_struct
{
    volatile bool _written;//用于标识串口状态，true 串口正在发送数据，false 串口空闲

    volatile rx_buffer_index_t _rx_buffer_head;
    volatile rx_buffer_index_t _rx_buffer_tail;
    volatile tx_buffer_index_t _tx_buffer_head;
    volatile tx_buffer_index_t _tx_buffer_tail;

    unsigned char _rx_buffer[SERIAL_RX_BUFFER_SIZE];
    unsigned char _tx_buffer[SERIAL_TX_BUFFER_SIZE];
	
		hard_msg_cb init_end_cb;
		hard_msg_cb hard_send_cb;
		
};

//串口对象初始化
//参数1:串口对象的结构体指针,参数2:串口回调函数(函数指针) 参数3:发送回调(函数指针)
//
void HardwareSerial_Init(HardwareSerial_t *ser,hard_msg_cb basic_cb,hard_msg_cb send);//对象初始化
//调用回调函数里的初始化函数进行串口初始化
void HardwareSerial_begin(HardwareSerial_t *ser);//串口初始化
//DeInit   初始化的反向操作
void HardwareSerial_end(HardwareSerial_t *ser);

//有多少可读
size_t HardwareSerial_available(HardwareSerial_t *ser);
//有多少可写
size_t HardwareSerial_availableForWrite(HardwareSerial_t *ser);

//从接受缓冲读一个，但是不移动队列
int16_t HardwareSerial_peek(HardwareSerial_t *ser);
//从接受缓冲读一个，并队列后移
int16_t HardwareSerial_read(HardwareSerial_t *ser);
uint8_t HardwareSerial_write(HardwareSerial_t *ser,uint8_t p_tr);


void HardwareSerial_tx_udr_empty_irq(HardwareSerial_t *ser);
void HardwareSerial_rx_complete_irq(HardwareSerial_t *ser,uint8_t c);



#if HAL_PRINTF > 0
	#define HSER_IT       0
	#define HSER_SOFE     1
void HardwareSerial_printf(HardwareSerial_t *ser,uint8_t ,char* fmt, ...);
#endif


#ifdef __cplusplus
}
#endif

#endif