//////////////////////////////////////////////////////////////////////////////////	 
//
//  文 件 名   : Sofei2cMaster.h
//  版 本 号   : v1.0
//  作    者   : Suaig
//  生成日期   : 2018-04-06
//  最近修改   : xq(2021-11-07)
//  功能描述   : IO口模拟I2C
//              说明:   对象化封装，使用时提供I/O口及延时操作的回调函数用于重写方法
//                      因为I/O不同等原因，每个I2C对象都有独特的属性和方法，需要重写(最底处有例子/模板)
//              结构：user->I2C->GPIO
//Copyright(C) Suaig 2018/4/6
//All rights reserved
//////////////////////////////////////////////////////////////////////////////////
#ifndef _SOFEI2CMASTER_H_
#define _SOFEI2CMASTER_H_//主要用于防止重复包含


#ifdef __cplusplus
	extern "C"{
#endif

#include <stdint.h>		
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

#define SI2C_MSG_BUF_DELAY		48		//停止信号与开始信号间的总线释放时间（SSD1306、TMP75为1.3us）
#define SI2C_MSG_SUSTA_DELAY	49		//重复开始信号的建立时间（SCL=1的最短时间，SSD1306、TMP75为0.6us）
#define SI2C_MSG_HDSTA_DELAY	50		//开始信号的保持时间（SDA拉低的最短时间，在这之后产生第一个时钟信号，SSD1306、TMP75为0.6us）
#define SI2C_MSG_SUSTO_DELAY	51		//停止信号的建立时间（SDA拉高前SCL=1，SDA=0的保持时间，SSD1306、TMP75为0.6us）
#define SI2C_MSG_HDDAT_DELAY	52		//数据保持时间(SCL=0后数据需要保持不变的时间，TMP75为4ns,很短,IO频率超过250M才需要?)
#define SI2C_MSG_SUDAT_DELAY	53		//数据建立时间
#define	SI2C_MSG_SCLL_DELAY		54		//SCL低电平时间
#define SI2C_MSG_SCLH_DELAY		55		//SCL高电平时间

/*
		
	宏，常量，以及一些枚举类型，，，	
*/
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
		case SI2C_MSG_BUF_DELAY:    //停止信号与开始信号间的总线释放时间（TMP75为1.3us）
		case SI2C_MSG_SUSTA_DELAY:	//重复开始信号的建立时间（SDA=SCL=1的最短时间，TMP75为0.6us）
		case SI2C_MSG_HDSTA_DELAY:	//开始信号的保持时间（SDA拉低的最短时间，在这之后产生第一个时钟信号，TMP75为0.6us）
		case SI2C_MSG_SUSTO_DELAY:	//停止信号的建立时间（SDA拉高前SCL=1，SDA=0的保持时间，TMP75为0.6us）
		case SI2C_MSG_HDDAT_DELAY:	//数据保持时间(SCL=0后数据需要保持不变的时间，4ns,很短,IO频率超过250M才需要?)
		case SI2C_MSG_SUDAT_DELAY:	//数据建立时间(SCL=1前SDA需要保持状态的时间，100ns，在读写时序中被SCL延时覆盖，在ack中需使用，若频率低于10M则不需要)
		case SI2C_MSG_SCLL_DELAY:	//SCL低电平时间(读写时序中的主要延时，TMP75为1.3us min)
		case SI2C_MSG_SCLH_DELAY:	//SCL高电平时间(读写时序中的主要延时, TMP75为0.6us min)
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
