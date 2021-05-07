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

bool enable_can(){
    struct ifreq ifr;          /**< Bluetooth MAC address */
    if ((fileDesc = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
        perror("Socket");
        return false;
    }
    strcpy(ifr.ifr_name, "DEV_NAME" );
    ioctl(fileDesc, SIOCGIFINDEX, &ifr);
    memset(&addr, 0, sizeof(addr));
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;
    if (bind(fileDesc, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("Bind");
        return false;
    }
    return true;
}

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

bool write_data(unsigned int id, int size, char * message) {
    frame.can_id = id;
    frame.can_dlc = size;
    sprintf((char *)frame.data, message);
    while(1){
        if (write(fileDesc, &frame, sizeof(struct can_frame)) != sizeof(struct can_frame)) {
        perror("Write");
        return false;
        }
    }
    if (close(fileDesc) < 0) {
        perror("Close");
        return false;
    }
    return true;
}