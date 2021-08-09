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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <termios.h>
#include <sys/file.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <getopt.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <linux/watchdog.h>
#include <usb.h>
#include <stdbool.h>

static int rs232bud;


/*
  open_port(int portnumber,int baudrate,bool parity,int dataBits, int stopBits)
  This function Open rs232 serial link
*/
int rs232_open_port(int portnumber, int baudrate, enum RS_PARITY parity, int databits, int stopbits, enum RS_FLOWCONTROL flowcontrol )
{
    int filedescriptor;
    rs232bud = baudrate;
    //FTDI allows only 7/8 databits
    if((databits < 7) || (databits > 8))
    {
        return -1;
    }
    filedescriptor = usb_open_rs(portnumber, baudrate, parity, databits, stopbits, flowcontrol);
    if(filedescriptor < 0)
    {
        return -1;
    }
    else
    {
        return filedescriptor;
    }
}



/*
   read_data(int filedescriptor, unsigned char *buf, int size,int Timeout)
   This function Read rs232 serial link
*/
int rs232_read_data(int filedescriptor, unsigned char *buf, int size, int timeout)
{
    if(filedescriptor > 0)
    {
        int usbrd;
        usbrd = usb_read_t(filedescriptor, buf, size, timeout);
        if(usbrd < 0)
        {
            return -1;
        }
        else
        {
            return usbrd;
        }
    }
    else
    {
        return -1;
    }
}



/*
   write_data(int filedescriptor, unsigned char *RS485_buf, int size)
   This function Write rs232 serial link
*/
int rs232_write_data(int filedescriptor, unsigned char *buf, int size)
{
    if(filedescriptor > 0)
    {
        int ret;
        float WAIT;
        ret = usb_write(filedescriptor, buf, size);
        if(ret < 0)
        {
            return -1;
        }
        else
        {
            WAIT = (1 / (float)rs232bud) * 8 * size * 1000000+5000;
            usleep(WAIT);
            return ret;
        }
    }
    else
    {
        return -1;
    }
}



/*
  close_port(int filedescriptor)
  This function Close rs232 serial link
*/
bool rs232_close_port(int filedescriptor)
{
    if(filedescriptor > 0)
    {
        if(!usb_close(filedescriptor))
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }
}
