#include "DHT22.h"

void DHT22_Init(dht22_t *dht,dht22_msg_cb dht_cb)
{
	dht->gpio_and_delay_cb = dht_cb;
	dht->gpio_and_delay_cb(dht,DHT22_MSG_SDA_HIGH);
	dht->gpio_and_delay_cb(dht,DHT22_MSG_2S_DELAY);//保证上电2S越过DHT22不稳定状态
}
uint8_t DHT22_Read(dht22_t *dht)
{
	uint8_t i,j,sum;
	dht->DHT22_RXD_Buff[0] = dht->DHT22_RXD_Buff[1] = dht->DHT22_RXD_Buff[2] = dht->DHT22_RXD_Buff[3] = dht->DHT22_RXD_Buff[4] = 0;
	dht->gpio_and_delay_cb(dht,DHT22_MSG_SDA_HIGH);//拉高总线
	dht->gpio_and_delay_cb(dht,DHT22_MSG_1MS_DELAY);//延时1ms
	dht->gpio_and_delay_cb(dht,DHT22_MSG_SDA_LOW);//拉低
	
	dht->gpio_and_delay_cb(dht,DHT22_MSG_18MS_DELAY);//延时18ms

	
	dht->gpio_and_delay_cb(dht,DHT22_MSG_SDA_HIGH);//然后拉高
	dht->gpio_and_delay_cb(dht,DHT22_MSG_30US_DELAY);//等待30us
		//做完一个起始信号，等待从机拉低
	if(dht->gpio_and_delay_cb(dht,DHT22_MSG_READ_SDA) == 0)//读到的是低电平，从机响应了，那么等待其变高后准备接收数据
	{
		while(!dht->gpio_and_delay_cb(dht,DHT22_MSG_READ_SDA));//等待低电平结束
	}
	else
	{
		return 1;//直接返回，避免下面等待高电平陷入死循环
	}
	while(dht->gpio_and_delay_cb(dht,DHT22_MSG_READ_SDA));//等待80us的高电平结束，然后开始接收
	
	for(j=0;j<5;j++)
	{
		for(i=0;i<8;i++)
		{
			while(!dht->gpio_and_delay_cb(dht,DHT22_MSG_READ_SDA));//等待50us左右的低电平结束
			dht->DHT22_RXD_Buff[j] <<=1;
			dht->gpio_and_delay_cb(dht,DHT22_MSG_30US_DELAY);//延时30us
			if(dht->gpio_and_delay_cb(dht,DHT22_MSG_READ_SDA))
			{
				dht->DHT22_RXD_Buff[j] |= 0x01;
				while(dht->gpio_and_delay_cb(dht,DHT22_MSG_READ_SDA));//等待高电平结束
			}
		}
	}
	
	sum = dht->DHT22_RXD_Buff[0] + dht->DHT22_RXD_Buff[1] + dht->DHT22_RXD_Buff[2] + dht->DHT22_RXD_Buff[3];
	
	if(sum != dht->DHT22_RXD_Buff[4] || sum == 0) return 1;//检查校验和
	
	return 0;
}
uint8_t DHT22_Fget(dht22_t *dht,float *Humi,float *Temp)
{
	
	uint16_t t = 0x0000;
	uint8_t sum = 0;
	sum = dht->DHT22_RXD_Buff[0] + dht->DHT22_RXD_Buff[1] + dht->DHT22_RXD_Buff[2] + dht->DHT22_RXD_Buff[3];
	
	if(sum != dht->DHT22_RXD_Buff[4] || sum == 0) return 1;//检查校验和
	t |= dht->DHT22_RXD_Buff[0];
	t <<= 8;
	t |= dht->DHT22_RXD_Buff[1];
	
	*Humi = (float)t/10;
	
	t = 0x0000;
	
	t |= dht->DHT22_RXD_Buff[2];
	t <<= 8;
	t |= dht->DHT22_RXD_Buff[3];
	
		if(t & 0x8000)
	{
		t&=~0x8000;
		*Temp = -(float)t/10;
	}
	else
	{
		*Temp = (float)t/10;
	}
	return 0;
}
uint8_t DHT22_Iget(dht22_t *dht,int16_t *Humi,int16_t *Temp)
{
	
	int16_t t = 0x0000;
	uint8_t sum = 0;
	sum = dht->DHT22_RXD_Buff[0] + dht->DHT22_RXD_Buff[1] + dht->DHT22_RXD_Buff[2] + dht->DHT22_RXD_Buff[3];
	
	if(sum != dht->DHT22_RXD_Buff[4] || sum == 0) return 1;//检查校验和
	
	t |= dht->DHT22_RXD_Buff[0];
	t <<= 8;
	t |= dht->DHT22_RXD_Buff[1];
	
	*Humi = (int16_t)t/10;
	
	t = 0x0000;
	
	t |= dht->DHT22_RXD_Buff[2];
	t <<= 8;
	t |= dht->DHT22_RXD_Buff[3];
	if(t & 0x8000)
	{
		t&=~0x8000;
		*Temp = -(int16_t)t/10;
	}
	else
	{
		*Temp = (int16_t)t/10;
	}
	return 0;
}