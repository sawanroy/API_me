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

int fileDesc;              /**< File descriptor*/
struct sockaddr_can addr;  /**< transport protocol class address information (e.g. ISOTP) stucture defined in linux/can.h */
struct can_frame frame; 



/*
    Enable and Disable the CAN

*/
bool enable_can(bool state)
{
    FILE *fp;
  	char cmd[20];
    
    if(state)
    {
  	    sprintf(cmd,"ifconfig can0 up");
  	    fp = popen(cmd, "r");
  	    if (fp == NULL) 
        {
    	    return false;
  	    }
        return true;
	    pclose(fp);
    }
    else
    {
        sprintf(cmd,"ifconfig can0 down");
  	    fp = popen(cmd, "r");
  	    if (fp == NULL) 
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
// struct can_frame read_data()
// {
//     int nbytes, i;
//     FILE *fp;
//     char cmd[20];
//     printf("foo1_dump_start"); 
//     sprintf(cmd,"candump can0");
//   	fp = popen(cmd, "r");
//   	if (fp == NULL) 
//     {
//         perror("close");
//   	}
// 	printf("foo1_read");    
// 	//nbytes = fread(&frame,1000,sizeof(struct can_frame),fp);
//     printf("foo1");
//     ////remove this part after final testing ///////////
// 	while(1)
//     {
//         nbytes = fread(&frame,1000,sizeof(struct can_frame),fp);
//         printf("foo_while");
//         if(nbytes < 0)
//         {
//             perror("Read");
//             //return 1;
//         }
//         printf("0x%03X [%d] ",frame.can_id, frame.can_dlc);
//         for (i = 0; i < frame.can_dlc; i++)
//         {
//             printf("%02X ",frame.data[i]);
//         }
//     }
//     //////////////////////////////////////////////////////////
// 	pclose(fp); 
//     return frame;
// }

struct can_frame read_data() {
    int nbytes, i;   
	nbytes = read(fileDesc, &frame, sizeof(struct can_frame));
	while(1){
        if (nbytes < 0) {
            perror("Read");
            //return 1;
        }

        printf("0x%03X [%d] ",frame.can_id, frame.can_dlc);

        for (i = 0; i < frame.can_dlc; i++)
            printf("%02X ",frame.data[i]);

        printf("\r\n");
    }
	if (close(fileDesc) < 0) {
		perror("Close");
		//return 1;
	}
    return frame;
}


/*
    Write data to CAN
*/
bool write_data(unsigned int id, int size, char * message)
{
    frame.can_id = id;
    frame.can_dlc = size;
    sprintf((char *)frame.data, message);
    FILE *fp;
    char cmd[20];

    sprintf(cmd,"cansend can0 %d#%s",frame.can_id,frame.data);
  	fp = popen(cmd, "r");
  	if (fp == NULL) 
    {
        perror("close");
  	}

    pclose(fp);
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
  	if (fp == NULL) 
    {
        return false;
  	}

    return true;
	pclose(fp);
}