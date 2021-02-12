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


int rs485_open_port(int portnumber,int baudrate,bool parity,int dataBits, int stopBits);			/* Open rs485 serial link  */
int rs485_read_data(int filedescriptor, unsigned char *buf, int size,int Timeout);				    /* Read rs485 serial link  */
int rs485_write_data(int filedescriptor, unsigned char *RS485_buf, int size);					    /* Write rs485 serial link */
int rs485_close_port(int filedescriptor);									                        /* Close rs485 serial link */


#endif

