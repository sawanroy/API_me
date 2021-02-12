/*
***************************************************************************
*
* Author:Sawan Roy
*
* Copyright (C) 2021 TRUNEXA PVT LTD
*
* Email: sawan.roy@trunexa.com
*
***************************************************************************
*/

#ifndef open_usb_h__
#define open_usb_h__

#define SIM_PORT 4
#define SIM_Baud_Rate 9600
#include <stdbool.h>

int set_tty_attribs(int fd, int baudrate);						/* sets the baudrate */
int USB_open(int portname,int buadrate);						/* open the usb port */
int USB_open_rs(int portnumber,int baudrate,bool parity,int dataBits, int stopBits);	/* open the usb port for rs485 and rs232*/
int USB_read(int fd, unsigned char *buf, int size);					/* Read data from opened USB port */
int USB_read_t(int fd, unsigned char *buf, int size,int T);				/* Read data from opened USB port with timeout*/
int USB_write(int fd, unsigned char *write_buffer, int size);				/* write data to opened USB port */
int USB_close(int fd);									/* close the opened USB port */					



#endif
