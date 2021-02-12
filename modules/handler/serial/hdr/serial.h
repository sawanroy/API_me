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

#ifndef libserial_h__
#define libserial_h__


int serial_open(int portname,int baudrate);  // ttyAMA port open; Return fd  
int serial_read(int fd, unsigned char *buf, int size,int Timeout); // ttyAMA port read 
int serial_write(int fd, unsigned char *write_buffer, int size); //ttyAMA port write 
int serial_close(int fd);  // ttyAMA port close

#endif
