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

static int rs485bud;

/*

  open_port(int portnumber,int baudrate,bool parity,int dataBits, int stopBits)
  This function Open rs485 serial link

*/
int rs485_open_port(int portnumber,int baudrate, enum RS_PARITY parity, int dataBits, int stopBits, enum RS_FLOWCONTROL flowcontrol )
{
    int filedescriptor;
    rs485bud = baudrate;
    filedescriptor = usb_open_rs(portnumber, baudrate, parity, dataBits, stopBits, flowcontrol);
    if(filedescriptor == -10)
    {
        return -1;
    }
    else
    {
        if(filedescriptor < 0)
        {
            return -1;
        }
        else
        {
            return filedescriptor;
        }
    }
}



/*
   read_data(int filedescriptor, unsigned char *buf, int size,int Timeout)
   This function Read rs485 serial link
*/
int rs485_read_data(int filedescriptor, unsigned char *buf, int size,int Timeout)
{
    if(filedescriptor > 0)
    {
        int usbrd;
        usbrd = usb_read_t(filedescriptor, buf, size, Timeout);
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
   This function Write rs485 serial link
*/
bool rs485_write_data(int filedescriptor, unsigned char *RS485_buf, int size)
{
    if(filedescriptor > 0)
    {
        int ret;
        float WAIT;
        ret = usb_write(filedescriptor, RS485_buf, size);
        if(ret < 0)
        {
            return false;
        }
        else
        {
            WAIT = (1 / (float)rs485bud) * 8 * size * 1000000+5000;
              usleep(WAIT);
            return true;
        }
    }
    else
    {
        return false;
    }
}



/*
  close_port(int filedescriptor)
  This function Close rs485 serial link
*/
bool rs485_close_port(int filedescriptor)
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
