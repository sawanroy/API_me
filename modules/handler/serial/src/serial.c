/*
***************************************************************************
*
* Author: Sawan Roy
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
            return -10;
    }

    struct termios tty;

    if (tcgetattr(fd, &tty) < 0)
    {
        printf("Error from tcgetattr: %s\n", strerror(errno));
        return -1;
    }

    cfsetospeed(&tty, (speed_t)speed);
    cfsetispeed(&tty, (speed_t)speed);

    tty.c_cflag |= (CLOCAL | CREAD);    /* ignore modem controls */
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;         /* 8-bit characters */
    tty.c_cflag &= ~PARENB;     /* no parity bit */
    tty.c_cflag &= ~CSTOPB;     /* only need 1 stop bit */
    tty.c_cflag &= ~CRTSCTS;    /* no hardware flowcontrol */

    /* setup for non-canonical mode */
    tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
    tty.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
    tty.c_oflag &= ~OPOST;

    /* fetch bytes as they become available */
    tty.c_cc[VMIN] = 1;
    tty.c_cc[VTIME] = 1;

    if (tcsetattr(fd, TCSANOW, &tty) != 0)
    {
        printf("Error from tcsetattr: %s\n", strerror(errno));
        return -1;
    }

    return 0;
}



/*************************** ttyAMA port ***********************/

int serial_open(int portname,int baudrate)
{
    char buf[15];
    snprintf(buf, sizeof(buf), "/dev/ttymxc%d", portname);
    int fd;

    fd = open(buf, O_RDWR | O_NOCTTY | O_SYNC);
    if (fd < 0)
    {
        printf("Error opening %s: \n", buf);
        return -1;
    }

    if (flock(fd, LOCK_EX | LOCK_NB) != 0)
    {
        printf("Another process has locked the ttyport\n");
        return -1;
    }

    set_tty_attribs(fd,baudrate);

    return fd;
}



int serial_read(int fd, unsigned char *buf, int size, int T)
{

    fd_set set;
    struct timeval timeout = {0, T};
    int ready=0;
    int TotalBytes=0;
    int r = 0;
    int expected_Byte=size;
    int filedisc = fd;
    int sizetoread = 0;
    int ntoread=0;

    FD_ZERO(&set);  /* clear the set */
    FD_SET(filedisc, &set); /* add our file descriptor to the set */
    
    ready = select(fd + 1 , &set, NULL, NULL, &timeout);
    if(ready == -1)
    {
    /* Some error has occured in input */
        return -1;
    }

    if(ready == 0)
    {
    /* a timeout occured */
        return -2;
    }
   
    else if(ready)
    {
        /* there was data to read */
        TotalBytes = read(fd, buf, expected_Byte);
        if (TotalBytes < expected_Byte)
        {
            sizetoread = expected_Byte - TotalBytes;
            ntoread = sizetoread;
            clock_t start = clock();
            double sec = 2;
            clock_t total = start + (sec * CLOCKS_PER_SEC);
            while (clock() < total)
            {   
                r = read(fd, &buf[TotalBytes], ntoread);
                TotalBytes += r;
                ntoread = ntoread - r;
                if(ntoread <= 0)
                {
                    break;
                }
            }
        }
    }
    if(TotalBytes == 0)
    {
        return -2;
    }
    FD_CLR(filedisc, &set);

    return TotalBytes;
}




int serial_write(int fd, unsigned char *write_buffer, int size)
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

    return n;
}



int serial_close(int fd)
{
    close(fd);
    flock(fd, LOCK_UN);
    printf("fd is closed %d \n",fd);

    return 0;
}