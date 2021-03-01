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

#define GPS_PORT 3                                        /**<  "/dev/ttyAMA3" */ 
#define GPS_Baud_Rate 9600                                /**< set baudrate */

/*! \addtogroup GPS
	Additional documentation for group 'GPS'
      @{
*/

/*! 
  Open GPS serial link 
  @return file descriptor 
*/
int open_port();
/*!
  State of GPS
  @param[in] filedescriptor  FD of opened port
*/                                                        
int state_gps(int filedescriptor);
/*!
  Output full GPRMC of 1 frame 
  @param[in] filedescriptor FD of opened port
*/                                      
char* read_data_gprmc(int filedescriptor);
/*!
  Extract part of GPRMC of 1 frame
  @param[in] filedescriptor  FD of opened port
  @param[in] gprmc_index 
  @return gprmc data
*/                              
char* read_data_gprmc_parse(int filedescriptor, int gprmc_index);
/*!
  Output full GPGGA of 1 frame 
  @param[in] filedescriptor  FD of opened port
  @return gprmc parse data
*/       
char* read_data_gpgga(int filedescriptor);
/*!
  Extract part of GPGGA of 1 frame
  @param[in] filedescriptor FD of opened port
  @param[in] gpgga_index 
  @return gpgga data
*/                              
char* read_data_gpgga_parse(int filedescriptor, int gpgga_index);      
 /*!
  Close GPS serial link 
  @param[in] filedescriptor FD of opened port
  @return gpgga parse data

 */ 
int close_port(int filedescriptor);                                    

/*! @} */

#endif
