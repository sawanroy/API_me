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


/*! \addtogroup CAN
    Additional documentation for group 'CAN'
      @{
*/


#include <linux/can.h>
#include <linux/can/raw.h>


/*!
 *  opens the can node 
 *  @return true and false
 *  @param[in] state true for enable  and false for disable 
*/
bool can_enable(bool state);


/*!
 *  reads the data from can node 
 *  @return returns ID , Message , data size in form of Structure  
 */
struct can_frame can_read();


/*!
 *  writes the data to can node
 *  @param[in] id CAN ID 
 *  @param[in] size CAN message size
 *  @param[in] message CAN message 
 *  @return true and false 
 */
bool can_write(unsigned int id, int size, char *message);


/*!
 *  Configuring can at perticular bitrate
 *  @param[in] bitrate bit per second tranfer speed
 *  @return true and false
 */
bool can_configuration(int bitrate);


/*! @} */
#endif