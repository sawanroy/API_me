/**
 * @file
 * @brief APIs refernce for RS485
*/
/*
***************************************************************************
*
* Author: Sawan roy 
*
* Copyright (C) 2021 TRUNEXA INC
*
* Email: sawan.roy@trunexa.com
*
***************************************************************************
*/



#ifndef librs485_h__
#define librs485_h__

/*! \addtogroup RS485
	Additional documentation for group 'RS485'
      @{
*/
/*!
 *  Open rs485 serial link  
 * @param[in] portnumber serial port number 
 * @param[in] baudrate   baudrate for device
 * @param[in] parity     set the parity true or false
 * @param[in] databit    databit 
 * @param[in] stopbits
 * 
 * @return filedescriptor for open port
*/
int rs485_open_port(int portnumber,int baudrate,bool parity,int dataBits, int stopBits);
/*! 
 * Read from rs485 serial link  
 * @param[in] filedescriptor  FD of opened port
 * @param[out] buf   buffer needeed to store data 
 * @param[in] size   size for buffer
 * @param[in] timeout timeout time to read data
 * 
 * @return buffer size or -1 in case of error 
*/			
int rs485_read_data(int filedescriptor, unsigned char *buf, int size,int Timeout);	
/*! 
 * Write to rs485 serial link 
 * @param[in] filedescriptor  FD of opened port
 * @param[in] write_buf  data to write on serial port
 * @param[in] size         write_buf buffer size
 * 
 * @return true or false
*/			    
int rs485_write_data(int filedescriptor, unsigned char *RS485_buf, int size);
/*! 
 * Close rs485 serial link
 * @param[in] filedescriptor  FD of opened port
 * @return true or false
*/					    
int rs485_close_port(int filedescriptor);									                        

/*! @} */
#endif

