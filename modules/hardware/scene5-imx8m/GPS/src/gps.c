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
#include <serial.h>
#include <gps.h>



/*
    open_port()
    Open GPS serial link
*/
int open_port()
{
    int fd = serial_open(GPS_PORT,GPS_Baud_Rate);
    if(fd < 0)
    {
        dbg_log(("open port error\n"));
        return  -1;
    }
    else
    {
        return fd;
    }
}



/*
    state_gps(int filedescriptor)
    State of GPS
*/
char* state_gps(int filedescriptor)
{
    char *buftmp = malloc(100);
    char buf[5000];
    int size = sizeof(buf);
    int timeout = 800000; //usec
    int findresult = 0;

    if(filedescriptor > 0)
    {
        int usbrd;
        usbrd=serial_read(filedescriptor, (unsigned char *)buf, size, timeout);
        dbg_log(("usbrd : %d \n",usbrd));
        if(usbrd < 1)
        {
           dbg_log(("serial read error\n"));
           sprintf(buftmp,"ERROR");
           return buftmp; 
        }
    }

    char *temp = strtok(buf, "\n");
    while(temp != NULL)
    {
        if((strstr(temp, "$PSTMANTENNASTATUS")) != NULL)
        {
            strcpy(buftmp,temp);
            findresult++;
        }
        temp = strtok(NULL, "\n");
    }
    if(findresult == 0)
    {
        dbg_log(("\n didnt get the data \n"));
    }

    return buftmp;
}



/*
    char* read_data_gprmc(int filedescriptor)
    output full GPRMC of 1 trame
*/
char* read_data_gprmc(int filedescriptor)
{
    char *buftmp = malloc(100);
    char buf[5000];
    int size = sizeof(buf);
    int timeout = 800000; //usec
    int findresult = 0;

    if(filedescriptor > 0)
    {
        int usbrd;
        usbrd=serial_read(filedescriptor, (unsigned char *)buf, size, timeout);
        dbg_log(("usbrd : %d \n",usbrd));
        if(usbrd < 1)
        {
           dbg_log(("serial read error\n"));
           sprintf(buftmp,"ERROR");
           return buftmp; 
        }
    }

    char *temp = strtok(buf, "\n");
    while(temp != NULL)
    {
        if((strstr(temp, "$GPRMC")) != NULL)
        {
            strcpy(buftmp,temp);
            findresult++;
        }
        temp = strtok(NULL, "\n");
    }
    if(findresult == 0)
    {
        dbg_log(("\n didnt get the data \n"));
    }

    return buftmp;
}



/*
    char* read_data_gprmc_parse(int filedescriptor, int gprmc_index)
    extract part of GPRMC of 1 trame
*/
char* read_data_gprmc_parse(int filedescriptor, int index)
{
    char *buftmp = malloc(100);
    int i = 0;
    char *str1 = "";
    char gpsdata[14][14];

    if(!(index > 0 && index < 14))
    {
        sprintf(buftmp,"ERROR");
        return buftmp;
    }

    char *data = read_data_gprmc(filedescriptor);
    if((strcmp(data, "") == 0) || (strcmp(data, "ERROR") == 0))
    {
        free(data);
        sprintf(buftmp,"ERROR");
        return buftmp;
    }

    char *original = strdup((const char *)data);
    while((str1 = strsep(&original, ",")) != NULL)
    {
        strcpy(gpsdata[i], str1);
        i++;
    }
    
    strcpy(buftmp, gpsdata[index]);    
    free(original);
    free(data);

    return buftmp;
}



/*
    char* read_data_gpgga(int filedescriptor)
    output full GPGGA of 1 trame
*/
char* read_data_gpgga(int filedescriptor)
{
    char *buftmp = malloc(100);
    char buf[5000];
    int size = sizeof(buf);
    int timeout = 800000; //usec
    int findresult = 0;

    if(filedescriptor > 0)
    {
        int usbrd;
        usbrd=serial_read(filedescriptor, (unsigned char *)buf, size, timeout);
        dbg_log(("usbrd : %d \n",usbrd));
        if(usbrd < 1)
        {
           dbg_log(("serial read error\n"));
           sprintf(buftmp,"ERROR");
           return buftmp; 
        }
    }

    char *temp = strtok(buf, "\n");
    while(temp != NULL)
    {
        if((strstr(temp, "$GPGGA")) != NULL)
        {
            strcpy(buftmp,temp);
            findresult++;
        }
        temp = strtok(NULL, "\n");
    }
    if(findresult == 0)
    {
        dbg_log(("\n didnt get the data \n"));
    }
    
    return buftmp;
}



/*
    char* read_data_gpgga_parse(int filedescriptor, int gpgga_index)
    extract part of GPGGA of 1 trame
*/
char* read_data_gpgga_parse(int filedescriptor, int index)
{
    char *buftmp = malloc(100);
    int i = 0;
    char *str1 = "";
    char gpsdata[14][14];

    if(!(index > 0 && index < 14))
    {
        sprintf(buftmp,"ERROR");
        return buftmp;
    }

    char *data = read_data_gpgga(filedescriptor);
    if((strcmp(data, "") == 0) || (strcmp(data, "ERROR") == 0))
    {
        free(data);
        sprintf(buftmp,"ERROR");
        return buftmp;
    }

    char *original = strdup((const char *)data);
    while((str1 = strsep(&original, ",")) != NULL)
    {
        strcpy(gpsdata[i], str1);
        i++;
    }
    
    strcpy(buftmp, gpsdata[index]);    
    free(original);
    free(data);

    return buftmp;
}



/*
    close_port(int filedescriptor)
    Close GPS serial link
*/
int close_port(int filedescriptor)
{
    if(filedescriptor > 0)
    {
        if(!serial_close(filedescriptor))
        {
            return 0;
        }
        else
        {
            return -1;
        }
    }
    else
    {
        return -1;
    }
}

