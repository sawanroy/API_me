 /*!
    @file
    @ API for CAN
 */

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


#ifndef can_h__
#define can_h__

#include <linux/can.h>
#include <linux/can/raw.h>

#define DEV_NAME vcan0

    int fd;                  /**< File descriptor*/
	struct sockaddr_can addr;  /**< transport protocol class address information (e.g. ISOTP) stucture defined in linux/can.h */
	struct ifreq ifr;          /**< Bluetooth MAC address */
	struct can_frame frame;    /**< struct can_frame - basic CAN frame structure structure defined in linux/can.h  */
        

/**
 *opens the can node 
 *@return true and false
*/
bool enable_can();
/**
 * reads the data from can node 
 *@return returns ID , Message , data size in form of Structure  
*/
struct can_frame read_data();
/**
 *writes the data to can node 
 *@return true and false 
 *
*/
bool write_data();


#endif