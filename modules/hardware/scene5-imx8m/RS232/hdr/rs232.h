/*!
 * @file rs232.h
 * @brief APIs refernce for rs232
 * 
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

#ifndef librs232_h__
#define librs232_h__

/*! \addtogroup RS232
	Additional documentation for group 'RS232'
      @{
*/

/*! 
 * Open the  rs232 serial link 
 * @param[in] portnumber serial port number 
 * @param[in] baudrate   baudrate for device 
 * @param[in] parity     set the parity true or false
 * @param[in] databit    databit 
 * @param[in] stopbits
 * 
 * @return filedescriptor for open port
 * 
*/
int rs232_open_port(int portnumber,int baudrate,bool parity,int dataBits, int stopbits);	    
/*!
 * Read from rs232 serial link  
 * @param[in] filedescriptor  FD of opened port 
 * @param[out] buf   buffer needeed to store data 
 * @param[in] size   size for buffer
 * @param[in] timeout timeout time to read data
 * 
 * @return buffer size or -1 in case of error 
 * 
*/
int rs232_read_data(int filedescriptor, unsigned char *buf, int size,int Timeout);		      
/*!
 * Write to rs232 serial link
 * @param[in] filedescriptor  FD of opened port 
 * @param[in] write_buf  data to write on serial port
 * @param[in] size         write_buf buffer size
 * 
 * @return true or false
*/  
bool rs232_write_data(int filedescriptor, unsigned char *write_buf, int size);	
/*!
 * Close rs232 serial link 
 *  @param[in] filedescriptor  FD of opened port
 *  @return true or false
*/        	    
bool rs232_close_port(int filedescriptor);							                            

/*! @}*/

#endif
