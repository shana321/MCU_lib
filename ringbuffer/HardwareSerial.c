#include "HardwareSerial.h"

int16_t HardwareSerial_peek(HardwareSerial_t *ser)//从接受缓冲读一个，但是不移动队列
{
		if (ser->_rx_buffer_head == ser->_rx_buffer_tail) 
		{
			return -1;
		} 
		else 
		{
			return ser->_rx_buffer[ser->_rx_buffer_tail];
		}
}
int16_t HardwareSerial_read(HardwareSerial_t *ser)//从接受缓冲读一个，并队列后移
{
  if (ser->_rx_buffer_head == ser->_rx_buffer_tail)
	{
    return -1;
	}
	else
	{
    uint8_t c = ser->_rx_buffer[ser->_rx_buffer_tail];
    ser->_rx_buffer_tail = (rx_buffer_index_t)(ser->_rx_buffer_tail + 1) % SERIAL_RX_BUFFER_SIZE;
    return c;
	}
}
void HardwareSerial_tx_udr_empty_irq(HardwareSerial_t *ser)//放在发送中断中，用于从发送队列中取出新的需要发送的送往发送
{
		uint8_t c = 0;

	ser->_written = false;
	  if (ser->_tx_buffer_head == ser->_tx_buffer_tail) {

			return;//发送完成
  }
  c = ser->_tx_buffer[ser->_tx_buffer_tail];
  ser->_tx_buffer_tail = (ser->_tx_buffer_tail + 1) % SERIAL_TX_BUFFER_SIZE;

  ser->hard_send_cb(ser,c);
	ser->_written = true;

	
}
void HardwareSerial_rx_complete_irq(HardwareSerial_t *ser,uint8_t c)//放在接受中断中，用于往接受队列放入刚刚接受到的数据
{
	rx_buffer_index_t i = (uint16_t)(ser->_rx_buffer_head + 1) % SERIAL_RX_BUFFER_SIZE;


	
    if (i != ser->_rx_buffer_tail)
		{
      ser->_rx_buffer[ser->_rx_buffer_head] = c;
      ser->_rx_buffer_head = i;
    }
		
}
size_t HardwareSerial_available(HardwareSerial_t *ser)//接受队列中，有多少数据可读
{
	size_t ret = 0;
	
	ret = ((rx_buffer_index_t)(SERIAL_RX_BUFFER_SIZE + ser->_rx_buffer_head - ser->_rx_buffer_tail)) % SERIAL_RX_BUFFER_SIZE;
	
  return ret;
}
size_t HardwareSerial_availableForWrite(HardwareSerial_t *ser)//发送队列中，有多少可写
{
	return ((tx_buffer_index_t)(SERIAL_TX_BUFFER_SIZE + ser->_tx_buffer_head - ser->_tx_buffer_tail)) % SERIAL_TX_BUFFER_SIZE;
}
uint8_t HardwareSerial_write(HardwareSerial_t *ser,uint8_t p_tr)//向
{
	tx_buffer_index_t i;
	//sbool state = 0;
	uint8_t c = 0;

	//state = ser->_written;
	
	i = (ser->_tx_buffer_head + 1) % SERIAL_TX_BUFFER_SIZE;

	
	
	 while (i == ser->_tx_buffer_tail)//缓冲区满了,等待中断发送,可能有bug,当发发送寄存器没数据时,满了，就会停在这,
	{

	}
//	if(i == ser->_tx_buffer_tail)//缓冲区满了，返回0
//	{
//		//return 0;
//	}
	ser->_tx_buffer[ser->_tx_buffer_head] = p_tr;
  ser->_tx_buffer_head = i;

	
	if (ser->_written == false)
	{
			c = ser->_tx_buffer[ser->_tx_buffer_tail];
			ser->_tx_buffer_tail = (ser->_tx_buffer_tail + 1) % SERIAL_TX_BUFFER_SIZE;
		
		
			//EA = 0;
			ser->hard_send_cb(ser,c);
			ser->_written = true;
			//EA = 1;
		
			return 1;
	}
	
	return 1;
}
void HardwareSerial_Init(HardwareSerial_t *ser,hard_msg_cb basic_cb,hard_msg_cb send)
{
	
		ser->_rx_buffer_head = 0;
    ser->_rx_buffer_tail = 0;
    ser->_tx_buffer_head = 0;
    ser->_tx_buffer_tail = 0;
		ser->init_end_cb = basic_cb;
		ser->hard_send_cb = send;
		ser->_written = false;
}
void HardwareSerial_begin(HardwareSerial_t *ser)
{
	ser->init_end_cb(ser,HSER_MSG_INIT);
}
void HardwareSerial_end(HardwareSerial_t *ser)
{
	ser->init_end_cb(ser,HSER_MSG_END);
}