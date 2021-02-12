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


int rs232_open_port(int portnumber,int baudrate,bool parity,int dataBits, int stopBits);	    /* This function Open rs232 serial link  */
int rs232_read_data(int filedescriptor, unsigned char *buf, int size,int Timeout);		        /* This function Read rs232 serial link  */
bool rs232_write_data(int filedescriptor, unsigned char *write_buf, int size);	        	    /* This function Write rs232 serial link */
bool rs232_close_port(int filedescriptor);							                            /* This function Close rs232 serial link */

#endif
