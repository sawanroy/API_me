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


/*
  set_tty_attribs(int fd, int baudrate)
  This function sets the baudrate
*/
int set_tty_attribs(int fd, int baudrate)
{
    int speed;
    switch(baudrate)
    {
        case 50:
            speed = B50;
            break;
        case 75:
            speed = B75;
            break;
        case 110:
            speed = B110;
            break;
        case 134:
            speed = B134;
            break;
        case 150:
            speed = B150;
            break;
        case 200:
            speed = B200;
            break;
        case 300:
            speed = B300;
            break;
        case 600:
            speed = B600;
            break;
        case 1200:
            speed = B1200;
            break;
        case 1800:
            speed = B1800;
            break;
        case 2400:
            speed = B2400;
            break;
        case 4800:
            speed = B4800;
            break;
        case 9600:
            speed = B9600;
            break;
        case 19200:
            speed = B19200;
            break;
        case 38400:
            speed = B38400;
            break;
        case 57600:
            speed = B57600;
            break;
        case 115200:
            speed = B115200;
            break;
        case 230400:
            speed = B230400;
            break;
        case 460800:
            speed = B460800;
            break;
        case 500000:
            speed = B500000;
            break;
        case 576000:
            speed = B576000;
            break;
        case 921600:
            speed = B921600;
            break;
        case 1000000:
            speed = B1000000;
            break;
        case 1152000:
            speed = B1152000;
            break;
        case 1500000:
            speed = B1500000;
            break;
        case 2000000:
            speed = B2000000;
            break;
        case 2500000:
            speed = B2500000;
            break;
        case 3000000:
            speed = B3000000;
            break;
        case 3500000:
            speed = B3500000;
            break;
        case 4000000:
            speed = B4000000;
            break;
        default:
            printf("invalid baudrate\n");
            return -1;
    }

    struct termios tty;
    if(tcgetattr(fd, &tty) < 0)
    {
        printf("Error from tcgetattr: %s\n", strerror(errno));
        return -1;
    }

    cfsetospeed(&tty, (speed_t)speed);
    cfsetispeed(&tty, (speed_t)speed);

    tty.c_cflag |= (CLOCAL | CREAD);                                                          /* ignore modem controls */
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;                                                                       /* 8-bit characters */
    tty.c_cflag &= ~PARENB;                                                                   /* no parity bit */
    tty.c_cflag &= ~CSTOPB;                                                                   /* only need 1 stop bit */
    tty.c_cflag &= ~CRTSCTS;                                                                  /* no hardware flowcontrol */

    /* setup for non-canonical mode */
    tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
    tty.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
    tty.c_oflag &= ~OPOST;

    /* fetch bytes as they become available */
    tty.c_cc[VMIN] = 1;
    tty.c_cc[VTIME] = 1;

    if(tcsetattr(fd, TCSANOW, &tty) != 0)
    {
        printf("Error from tcsetattr: %s\n", strerror(errno));
        return -1;
    }

    return 0;
}



/*
usb_open(int portname,int buadrate)
This function open the usb port
*/
int usb_open(int portname, int buadrate)
{
    char buf[15];
    snprintf(buf, sizeof(buf), "/dev/ttyUSB%d", portname);
    int fd;
    fd = open(buf, O_RDWR | O_NOCTTY | O_SYNC);
    if(fd < 0)
    {
        printf("Error opening %s: \n", buf);
        return -1;
    }

    if(flock(fd, LOCK_EX | LOCK_NB) != 0)
    {
        printf("Another process has locked the tty USB port\n");
        return -1;
    }
    set_tty_attribs(fd, buadrate);

    return fd;
}



/*
  usb_open_rs(portnumber,baudrate,parity,dataBits,stopBits)
  use open
*/
int usb_open_rs(int portnumber, int baudrate, bool parity, int dataBits, int stopBits)
{
    char buf[15];
    snprintf(buf, sizeof(buf), "/dev/ttyUSB%d", portnumber);
    int fd;
    fd = open(buf, O_RDWR | O_NOCTTY | O_SYNC);
    if(fd < 0)
    {
        printf("Error opening %s: \n", buf);
        return -1;
    }

    if(flock(fd, LOCK_EX | LOCK_NB) != 0)
    {
        printf("Another process has locked the tty USB port\n");
        return -1;
    }
    set_tty_attribs(fd, baudrate);

    return fd;
}



/*
usb_read(int fd, unsigned char *buf, int size)
This function Read data from opened USB port
*/
int usb_read(int fd, unsigned char *buf, int size)
{
    int n;
    n = read(fd, buf, size-1);
    if(n < 0)
    {
        if(errno == EAGAIN)  
        {
            return -1;
        }
    }

    return(n);
}



/*
  usb_read_t(int fd, unsigned char *buf, int size,int T)
  usb read function with timeout.
*/
int usb_read_t(int fd, unsigned char *buf, int size, int T)
{
    fd_set set;
    struct timeval timeout = {0, T};
    int ready = 0;
    int TotalBytes = 0;
    int r = 0;
    int expected_Byte = size;
    int filedisc = fd;
    int sizetoread = 0;
    static int ntoread = 0;

    FD_ZERO(&set);  /* clear the set */
    FD_SET(filedisc, &set); /* add our file descriptor to the set */


    double sec = ((double)T/(double)1000000);
    ready = select(fd + 1 , &set, NULL, NULL, &timeout);

    if(ready == -1)
    {
        /* Some error has occured in input */
        printf("NOT READY");
        return -1;
    }

    if(ready == 0)
    {
        /* a timeout occured */
        printf("TIMEOUT !ST");
        return -2;
    }
    else if(ready)
    {
        /* there was data to read */
        TotalBytes = read(fd, buf, expected_Byte);

        if(TotalBytes < expected_Byte)
        {
            sizetoread = expected_Byte - TotalBytes;
            ntoread = sizetoread;

            clock_t  total_t;
            total_t = clock() + sec * CLOCKS_PER_SEC;
            while(clock() < total_t)
            {
                ntoread = ntoread - r;
                long int time = ((double)total_t - (double)clock())/ (double)CLOCKS_PER_SEC ;
                struct timeval t = {0, time};
                if(select(fd + 1 , &set, NULL, NULL, &t) > 0)
                {
                    r = read(fd, &buf[TotalBytes], ntoread);
                    TotalBytes += r;
                }
                else
                {
                    break;
                }
            }
        }
    }
    if(TotalBytes == 0)
    {
        printf("TIMEOUT 2&");
        return -2;
    }

    FD_CLR(filedisc, &set);

    return TotalBytes;
}



/*
  usb_write(int fd, unsigned char *write_buffer, int size)
  This function write data to opened USB port
*/
int usb_write(int fd, unsigned char *write_buffer, int size)
{
    int n;
    n = write(fd, write_buffer, size-1);
    if(n != (size-1))
    {
        if(errno == EAGAIN)
        {
            return -1;
        }
    }

    return(n);
}



/*
   usb_close(int fd)
   This function close the opened USB port
*/
int usb_close(int fd)
{
    close(fd);
    flock(fd, LOCK_UN);
    printf("fd is closed %d \n",fd);

    return 0;
}



int match(char *a, char *b)
{
    int position = 0;
    char *x, *y;
    x = a;
    y = b;

    while(*a)
    {
        while(*x == *y)
        {
            x++;
            y++;
            if((*x == '\0') || (*y == '\0'))
            {
                break;
            }
        }
        if(*y == '\0')
        {
            break;
        }
        a++;
        position++;
        x = a;
        y = b;
    }
    if(*a)
    {
        return 0;
    }
    else
    {
        return -1;
    }
}

