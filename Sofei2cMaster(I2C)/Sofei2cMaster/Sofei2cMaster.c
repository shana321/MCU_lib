#include "Sofei2cMaster.h"
#define I2C_INIT()         slf->obj_cb(slf,SI2C_MSG_INIT)
#define I2C_SET_SDA()      slf->obj_cb(slf,SI2C_MSG_SET_SDA)	//SDA高电平
#define I2C_RESET_SDA()    slf->obj_cb(slf,SI2C_MSG_RESET_SDA) //SDA低电平
#define I2C_SDA_INPUT()    slf->obj_cb(slf,SI2C_MSG_INPUT_SDA)//把SDA设置成INPUT准备读
#define I2C_SDA_OUTPUT()   slf->obj_cb(slf,SI2C_MSG_OUTPUT_SDA)//把SDA设置成输出


#define I2C_READ_SDA()     slf->obj_cb(slf,SI2C_MSG_READ_SDA)//读SDA

#define I2C_SCL_OUTPUT()  	slf->obj_cb(slf,SI2C_MSG_OUTPUT_SCL)//SCL配置为输出
#define I2C_SET_SCL()  		slf->obj_cb(slf,SI2C_MSG_SET_SCL)	//SCL设置为高电平
#define I2C_RESET_SCL() 	slf->obj_cb(slf,SI2C_MSG_RESET_SCL) //SCL设置为低电平
#define I2C_DELAY()         slf->obj_cb(slf,SI2C_MSG_DELAY)//用于控制软I2C总线速度
#define I2C_DELAY_R()       slf->obj_cb(slf,SI2C_MSG_R_DELAY)//用于控制软I2C总线读取速度
#define I2C_DELAY_W()       slf->obj_cb(slf,SI2C_MSG_W_DELAY)//用于控制软I2C总线读取速度


void New_Sofei2cMaster(Sofei2cMaster *slf,Sofei2c_cb you_call_back)
{
	slf->obj_cb = you_call_back;
	I2C_INIT();//初始化I2C总线的IO口
	
	Sofei2cMaster_init(slf);
}
void Sofei2cMaster_init(Sofei2cMaster *slf)
{
	I2C_SCL_OUTPUT();
	I2C_SDA_OUTPUT();
	
	I2C_SET_SDA();
	I2C_SET_SCL();//释放总线
}
void Sofei2cMaster_start(Sofei2cMaster *slf)
{
	Sofei2cMaster_init(slf);
	
	I2C_DELAY();
	I2C_RESET_SDA();//START:when CLK is high,DATA change form high to low 
	
	I2C_DELAY();
	I2C_RESET_SCL();//钳住I2C总线，准备发送或接收数据 
	
	//SDA = 0;SCL = 0;
}
void Sofei2cMaster_stop(Sofei2cMaster *slf)
{
	I2C_SDA_OUTPUT();
	//I2C_SCL_OUTPUT();
	
	I2C_RESET_SCL();
	I2C_RESET_SDA();//STOP:when CLK is high DATA change form low to high
	
	I2C_DELAY();
	
	I2C_SET_SCL();
	I2C_SET_SDA();
	
	I2C_DELAY();
	
	//SDA = 1;SCL = 1;
}

uint8_t Sofei2cMaster_wait_ack(Sofei2cMaster *slf)
{
	uint8_t errtime = 0;
	I2C_SET_SDA();
	I2C_DELAY_W();
	I2C_SET_SCL();
	I2C_DELAY_W();
	
	I2C_SDA_INPUT();
	while(I2C_READ_SDA())
	{
		errtime++;
		if(errtime > 250)
		{
			Sofei2cMaster_stop(slf);
			return 1;
		}
		//I2C_DELAY();
	}
	I2C_RESET_SCL();
}
void Sofei2cMaster_ack(Sofei2cMaster *slf)
{
	I2C_RESET_SCL();
	I2C_SDA_OUTPUT();
	I2C_RESET_SDA();
	
	
	I2C_DELAY_W();
	I2C_SET_SCL();
	I2C_DELAY_W();
	I2C_RESET_SCL();
}
void Sofei2cMaster_nack(Sofei2cMaster *slf)
{
	I2C_RESET_SCL();
	I2C_SDA_OUTPUT();
	I2C_SET_SDA();//SDA = 1;
	
	I2C_DELAY_W();
	I2C_SET_SCL();
	I2C_DELAY_W();
	I2C_RESET_SCL();
}
void Sofei2cMaster_send_byte(Sofei2cMaster *slf,uint8_t txd)
{
	uint8_t t = 0;
	I2C_SDA_OUTPUT();
	I2C_RESET_SCL();
	for(t=0;t<8;t++)
	{
		if(txd & 0x80)
		{
			I2C_SET_SDA();
		}
		else
		{
			I2C_RESET_SDA();
		}
		txd<<=1;
		
		I2C_DELAY_W();
		I2C_SET_SCL();
		I2C_DELAY_W();
		I2C_RESET_SCL();
		I2C_DELAY_W();
	}
}
uint8_t Sofei2cMaster_read_byte(Sofei2cMaster *slf,uint8_t ack)
{
	uint8_t i,receive=0;
	I2C_SDA_INPUT();//SDA设置为输入
  for(i=0;i<8;i++ )
	{
        I2C_RESET_SCL();
        I2C_DELAY_R();
				I2C_SET_SCL();
        receive<<=1;
        if(I2C_READ_SDA())receive++;   
				I2C_DELAY_R();
  }					 
  if (!ack)
	{
        Sofei2cMaster_nack(slf);//发送nACK
	}
  else
	{
       Sofei2cMaster_ack(slf); //发送ACK   
	}
  return receive;
}
uint8_t Sofei2cMaster_I2C_SCAN(Sofei2cMaster *slf,uint8_t dev_addr)
{
	Sofei2cMaster_start(slf);   
	
	Sofei2cMaster_send_byte(slf,dev_addr);
	
	if(Sofei2cMaster_wait_ack(slf)) return 1;
	
	return 0;
}
uint8_t Sofei2cMaster_wirte_mem(Sofei2cMaster *slf,
														 uint8_t dev_addr,
														 uint16_t addr,
														 uint8_t addr_size,
														 uint8_t *pData,
														 uint16_t size_t
														)
{
	Sofei2cMaster_start(slf);  
	if(addr_size == I2C_MEM_SIZE_16BIT)
	{
		Sofei2cMaster_send_byte(slf,dev_addr);	    //发送写命令
		if(Sofei2cMaster_wait_ack(slf)) return 1;
		Sofei2cMaster_send_byte(slf,addr>>8);//发送高地址	  
	}
	else
	{
		Sofei2cMaster_send_byte(slf,dev_addr+((addr/256)<<1));   //发送器件地址0XA0,写数据
	}		
	if(Sofei2cMaster_wait_ack(slf)) return 1;  
    Sofei2cMaster_send_byte(slf,addr&0x00FF);   //发送低地址
	if(Sofei2cMaster_wait_ack(slf)) return 1;
	
	while(size_t--)
	{
	Sofei2cMaster_send_byte(slf,*pData);     //发送字节							   
	if(Sofei2cMaster_wait_ack(slf)) return 1;
	pData++;
	}
  Sofei2cMaster_stop(slf);//产生一个停止条件 
	
	return 0;
}
uint8_t Sofei2cMaster_read_mem(Sofei2cMaster *slf,	//i2c对象指针
														 uint8_t dev_addr,	//器件地址
														 uint16_t addr,			//起始
														 uint8_t addr_size,	//地址类型，8位地址，16位地址 I2C_MEN_SIZE_8BIT  I2C_MEN_SIZE_16BIT
														 uint8_t *pData,		//接受缓冲区指针
														 uint16_t size_t		//接受多少个字节
														)
{
    Sofei2cMaster_start(slf);   
	if(addr_size == I2C_MEM_SIZE_16BIT)
	{
		Sofei2cMaster_send_byte(slf,dev_addr);	   //发送写命令
		if(Sofei2cMaster_wait_ack(slf)) return 1;
		Sofei2cMaster_send_byte(slf,addr>>8);//发送高地址	    
	}else Sofei2cMaster_send_byte(slf,dev_addr+((addr/256)<<1));   //发送器件地址0XA0,写数据 	   
	if(Sofei2cMaster_wait_ack(slf)) return 1;
  Sofei2cMaster_send_byte(slf,addr&0x00FF);   //发送低地址
	if(Sofei2cMaster_wait_ack(slf)) return 1;
	Sofei2cMaster_start(slf);   	 	   
	Sofei2cMaster_send_byte(slf,dev_addr|0x01);           //进入接收模式			   
	if(Sofei2cMaster_wait_ack(slf)) return 1;
	while(size_t--)
	{
    if(size_t == 0)
		{
			(*pData)=Sofei2cMaster_read_byte(slf,0);//NACK准备结束
		}
		else
		{
			(*pData) = Sofei2cMaster_read_byte(slf,1);//ACK我还要继续接受
		}
		pData++;
	}		
  Sofei2cMaster_stop(slf);//产生一个停止条件
	return 0;
}