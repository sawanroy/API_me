/**
 * @file gps.h
 * @brief APIs for gps access
*/

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

#ifndef libgps_h__
#define libgps_h__

#include <stdio.h>
#include <stdbool.h>

#define GPS_PORT 1                                          /**<  "/dev/ttymxc1" */
#define GPS_Baud_Rate 115200                                /**< set baudrate */


/**
 * Enable/Disable logs
*/
#define ENABLE_LOGS 0

#if ENABLE_LOGS
    #define dbg_log(a) printf a
#else
    #define dbg_log(a) (void)0
#endif


/*! \addtogroup GPS
  Additional documentation for group 'GPS'
      @{
*/


/*!
  Open GPS serial link
  @return File Descriptor of open port
*/
int open_port();


/*!
  State of GPS
  @param[in] filedescriptor  FD of opened port
  @return frame of PSTMANTENNASTATUS data
*/
char* state_gps(int filedescriptor);


/*!
  Output full GPRMC of 1 frame
  @param[in] filedescriptor FD of opened port
  @return frame of GPRMC data
*/
char* read_data_gprmc(int filedescriptor);


/*!
  Extract part of GPRMC of 1 frame
  @param[in] filedescriptor  FD of opened port
  @param[in] index
  @return required gprmc data
*/
char* read_data_gprmc_parse(int filedescriptor, int index);


/*!
  Output full GPGGA of 1 frame
  @param[in] filedescriptor  FD of opened port
  @return frame of GPGGA data
*/
char* read_data_gpgga(int filedescriptor);


/*!
  Extract part of GPGGA of 1 frame
  @param[in] filedescriptor FD of opened port
  @param[in] index
  @return required GPGGA data
*/
char* read_data_gpgga_parse(int filedescriptor, int index);


 /*!
  Close GPS serial link
  @param[in] filedescriptor FD of opened port
  @return status of function
 */
bool close_port(int filedescriptor);

/*! @} */

#endif
