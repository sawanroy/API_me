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

#include <usb.h>
/*! \addtogroup RS485
      Additional documentation for group 'RS485'
      @{
*/

/*!
 *  Open rs485 serial link
 * @param[in] portnumber serial port number
 * @param[in] baudrate   baudrate for device
 * @param[in] parity     set the parity from the RS_PARITY enum in usb.h
 * @param[in] databits   number of databits
 * @param[in] stopbits   number of stopbits
 * @param[in] flowcontrol  set the flowcontrol from the RS_FLOWCONTROL enum in usb.h
 * @return filedescriptor for open port
*/
int rs485_open_port(int portnumber, int baudrate, enum RS_PARITY parity, int databits, int stopbits, enum RS_FLOWCONTROL flowcontrol);


/*!
 * Read from rs485 serial link
 * @param[in] filedescriptor  FD of opened port
 * @param[out] buf   buffer needed to store data
 * @param[in] size   size for buffer
 * @param[in] timeout timeout time to read data
 * @return buffer size or -1 in case of error
*/
int rs485_read_data(int filedescriptor, unsigned char *buf, int size, int timeout);


/*!
 * Write to rs485 serial link
 * @param[in] filedescriptor  FD of opened port
 * @param[in] write_buf  data to write on serial port
 * @param[in] size       write_buf buffer size
 * @return buffer size or -1 in case of error
*/
int rs485_write_data(int filedescriptor, unsigned char *buf, int size);


/*!
 * Close rs485 serial link
 * @param[in] filedescriptor  FD of opened port
 * @return true or false
*/
bool rs485_close_port(int filedescriptor);

/*! @} */
#endif

