/*
@ 模块名称:软件I2C
@ 模块功能:软件模拟I2C
@ 作者:Suaig
@ 时间: 2018-4-6

*/
#ifndef _SOFEI2CMASTER_H_
#define _SOFEI2CMASTER_H_//主要用于防止重复包含


#ifdef __cplusplus
	extern "C"{
#endif

#include <reg52.h>		
/*
	头文件的包含	
	#include <reg52.h>
*/

#define I2C_MEM_SIZE_8BIT     0      //
#define I2C_MEM_SIZE_16BIT    1		

//消息机制所标志的消息类型
#define SI2C_MSG_INIT         39    //初始化IO口
#define SI2C_MSG_SET_SDA   		40		//SDA = 1
#define SI2C_MSG_RESET_SDA 		41		//SDA = 0
#define SI2C_MSG_READ_SDA  		42		//读SDA
#define SI2C_MSG_OUTPUT_SDA   43		//SDA设置为输出
#define SI2C_MSG_INPUT_SDA 		44		//SDA设置为输入

#define SI2C_MSG_OUTPUT_SCL 	45		//SCL设置为输出
#define SI2C_MSG_SET_SCL   		46		//SCL =1
#define SI2C_MSG_RESET_SCL		47		//SCL = 0

#define SI2C_MSG_DELAY     		48		//控制启，停，响应的速度
#define SI2C_MSG_R_DELAY      49		//控制读的总线速度
#define SI2C_MSG_W_DELAY      50		//控制写的总线速度

//PS:一般情况写而言，这三个延时可以使用同一个延时函数 4us以上为好，
//但是实践中，发现例如，DHT12的写时序可以可以更快，读则更慢，为了兼容所有I2C器件，
//故作此修改 一般器件推荐值 自上而下分别为 4us 2us 4us.... DHT12推荐值 15us 4us 50us
/*
		
	宏，常量，以及一些枚举类型，，，	
*/
typedef unsigned char uint8_t;	
typedef unsigned int uint16_t;
typedef struct _sofei2c Sofei2cMaster;
		
		
typedef uint8_t (*Sofei2c_cb)(Sofei2cMaster *i2c,uint8_t msg);
 
struct _sofei2c //定义一个软件模拟i2c主机
{
	Sofei2c_cb obj_cb;
};


//基本功能
/************************************
@函数功能：NEW一个I2C对象
@输入：1.软件I2C的对象指针 2.软件I2C的回调函数指针
@输出：void
@作者:Suaig
@时间:
*************************************/
void New_Sofei2cMaster(Sofei2cMaster *slf,Sofei2c_cb you_call_back);

/************************************
@函数功能：使用回调函数中的函数去初始化软件I2C总线
@输入：1.软件I2C的对象指针
@输出：void
@作者:Suaig
@时间:
*************************************/
void Sofei2cMaster_init(Sofei2cMaster *slf);
/************************************
@函数功能：发送I2C起始信号
@输入：1.软件I2C的对象指针
@输出：void
@作者:Suaig
@时间:
*************************************/
void Sofei2cMaster_start(Sofei2cMaster *slf);
/************************************
@函数功能：等待从机响应
@输入：1.软件I2C的对象指针
@输出：void
@作者:Suaig
@时间:
*************************************/
uint8_t Sofei2cMaster_wait_ack(Sofei2cMaster *slf);
/************************************
@函数功能：主机发送ACK信号(表示我收到字节，请准备下一个字节)
@输入：1.软件I2C的对象指针
@输出：void
@作者:Suaig
@时间:
*************************************/
void Sofei2cMaster_ack(Sofei2cMaster *slf);
/************************************
@函数功能：主机发送NACK信号(表示我收到字节，不需要准备下一个字节，即将结束)
@输入：1.软件I2C的对象指针
@输出：void
@作者:Suaig
@时间:
*************************************/
void Sofei2cMaster_nack(Sofei2cMaster *slf);
/************************************
@函数功能：发送一个字节
@输入：1.软件I2C的对象指针 2.需要发送的数据 8位
@输出：void
@作者:Suaig
@时间:
*************************************/
void Sofei2cMaster_send_byte(Sofei2cMaster *slf,uint8_t txd);
/************************************
@函数功能：发送停止位
@输入：1.软件I2C的对象指针
@输出：void
@作者:Suaig
@时间:
*************************************/
void Sofei2cMaster_stop(Sofei2cMaster *slf);
/************************************
@函数功能：读字节
@输入：1.软件I2C的对象指针 2.发送的响应类型，1 ACK 0 NACK
@输出：读取到的数据
@作者:Suaig
@时间:
*************************************/
uint8_t Sofei2cMaster_read_byte(Sofei2cMaster *slf,uint8_t ack);

/************************************
@函数功能：查看某器件地址是否响应(通常用来扫描器件地址)
@输入：1.软件I2C的对象指针 2.器件地址
@输出：如果响应返回 1，超时未响应返回 0
@作者:Suaig
@时间:
*************************************/
uint8_t Sofei2cMaster_I2C_SCAN(Sofei2cMaster *slf,uint8_t dev_addr);

//高级抽象函数
/************************************
@函数功能：I2C写器件寄存器(可连续写)
@输入：看函数
@输出：写成功返回0，失败返回1
@作者:Suaig
@时间:
*************************************/
uint8_t Sofei2cMaster_wirte_mem(Sofei2cMaster *slf,//i2c对象指针
														 uint8_t dev_addr,	//器件地址
														 uint16_t addr,			//起始地址
														 uint8_t addr_size,	//地址类型，8位地址，16位地址 I2C_MEN_SIZE_8BIT  I2C_MEN_SIZE_16BIT
														 uint8_t *pData,		//写入的数据串指针
														 uint16_t size_t		//写入的字节个数
														);
/************************************
@函数功能：I2C读器件寄存器
@输入：看函数
@输出：读成功返回0，失败返回1
@作者:Suaig
@时间:
*************************************/
uint8_t Sofei2cMaster_read_mem(Sofei2cMaster *slf,	//i2c对象指针
														 uint8_t dev_addr,	//器件地址
														 uint16_t addr,			//起始
														 uint8_t addr_size,	//地址类型，8位地址，16位地址 I2C_MEN_SIZE_8BIT  I2C_MEN_SIZE_16BIT
														 uint8_t *pData,		//接受缓冲区指针
														 uint16_t size_t		//接受多少个字节
														);



/*使用举例
//回调函数举例
uint8_t at89c52_i2c_cb(Sofei2cMaster *i2c,uint8_t msg)
{
	switch(msg)
	{
		case SI2C_MSG_INIT:  		break;
		case SI2C_MSG_SET_SDA:   P1 |= 0x01;break;		
		case SI2C_MSG_RESET_SDA: P1 &= ~(0x01);break;
		case SI2C_MSG_READ_SDA:	 return P1&0x01;break;
		case SI2C_MSG_OUTPUT_SDA:  break; 
		case SI2C_MSG_INPUT_SDA:	break;	

		case SI2C_MSG_OUTPUT_SCL: 	break;
		case SI2C_MSG_SET_SCL:		P1 |= 0x02; break;
		case SI2C_MSG_RESET_SCL:	P1 &= ~(0x02);break;
		case SI2C_MSG_DELAY:    //控制起始,停止,响应的速度
		case SI2C_MSG_R_DELAY:	//控制写的速度
		case SI2C_MSG_W_DELAY:	//控制读的速度
		break;//这里都不使用额外延时，因为51，调用回调的时间已经够长了。
		//对于有些外设，读的时候需要适当慢一点，要不然外设反应不过来
	}
	return 0;
}
//定义了回调函数，现在
New_Sofei2cMaster(&eeprom,at89c52_i2c_cb);
可以使用new去新建一个对象


Sofei2cMaster_wirte_mem(&PCF8574,0x40,0x00,I2C_MEM_SIZE_8BIT,&buff,1);
@使用高级抽象，直接读取。
也可以直接使用I2C子函数，自己读取，标准I2C的外设都可以使用高级抽象的I2C去读取














*/
#ifdef __cplusplus
}
#endif

#endif