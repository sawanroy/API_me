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

#define SIM_PORT 1
#define SIM_PORT_AT 7
#define SIM_Baud_Rate 115200
#include <stdbool.h>

int set_tty_attribs(int fd, int baudrate);						/* sets the baudrate */
int usb_open(int portname, int buadrate);						/* open the usb port */
int usb_open_rs(int portnumber, int baudrate, bool parity, int dataBits, int stopBits);	/* open the usb port for rs485 and rs232*/
int usb_read(int fd, unsigned char *buf, int size);					/* Read data from opened USB port */
int usb_read_t(int fd, unsigned char *buf, int size,int T);				/* Read data from opened USB port with timeout*/
int usb_write(int fd, unsigned char *write_buffer, int size);				/* write data to opened USB port */
int usb_close(int fd);									/* close the opened USB port */
int match(char *a, char *b);


#endif
