/*
***************************************************************************
*
* Author: Sawan Roy
*
* Copyright (C) 2021 Trunexa INC
*
* Email: sawan.roy@trunexa.com
*
***************************************************************************
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <can.h>



int fileDesc;              
struct sockaddr_can addr;
struct can_frame frame;  



/*
    internal functions 
*/
bool socket_connect()
{
    struct ifreq ifr;
    
    if((fileDesc = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) 
    {
        perror("Socket");
        return false;
    }

    strcpy(ifr.ifr_name, "can0");
    ioctl(fileDesc, SIOCGIFINDEX, &ifr);
    memset(&addr, 0, sizeof(addr));
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;
    
    if(bind(fileDesc, (struct sockaddr *)&addr, sizeof(addr)) < 0) 
    {
        perror("Bind");
        return false;
    }

    return true;
}



/*
    Enable and Disable the CAN
*/
bool can_enable(bool state)
{
    FILE *fp;
    char cmd[20];
    
    if(state)
    {
        sprintf(cmd, "ifconfig can0 up");
        fp = popen(cmd, "r");
        if(fp == NULL) 
        {
            return false;
        }
        return true;
        pclose(fp);
    }
    else
    {
        sprintf(cmd, "ifconfig can0 down");
        fp = popen(cmd, "r");
        if(fp == NULL) 
        {
            return false;
        }
        return true;
        pclose(fp);
    }
}



/*
    Read data from CAN
*/
struct can_frame can_read()
{
    socket_connect();
    int nbytes;   
    nbytes = read(fileDesc, &frame, sizeof(struct can_frame));
    if(nbytes < 0)
    {
        close(fileDesc);
        perror("read");
    }
    if(close(fileDesc) < 0)
    {
        perror("Close");
    }
    return frame;
}



/*
    Write data to CAN
*/
bool can_write(unsigned int id, int size, char *message) 
{ 
    socket_connect();
    frame.can_id = id;
    frame.can_dlc = size;
    sprintf((char *)frame.data, message);
    
    if(write(fileDesc, &frame, sizeof(struct can_frame))) 
    {
        perror("Write");
        return false;
    }
    
    if(close(fileDesc) < 0) 
    {
        perror("Close");
        return false;
    }
    
    return true;
}



/*
    Configure the CAN
*/
bool can_configuration(int bitrate)
{
    FILE *fp;
    char cmd[50];
    int speed;

    switch(bitrate)
    {
        case 125000:
            speed = 125000;
            break;
        case 250000:
            speed = 250000;
            break;
        case 500000:
            speed = 500000;
            break;
        case 1000000:
            speed = 1000000;
            break;
        default:
            printf("invalid baudrate\n");
            return false;
    }

    sprintf(cmd,"ip link set can0 type can bitrate %d",speed);
    fp = popen(cmd, "r");
    if(fp == NULL) 
    {
        return false;
    }

    return true;
    pclose(fp);
}