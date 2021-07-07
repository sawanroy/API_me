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

/*!
 *  opens the can node 
 *  @return true and false
 *  @param[in] state true for enable  and false for disable 
*/
bool enable_can(bool state);


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
bool write_data(unsigned int id, int size, char * message);


#endif