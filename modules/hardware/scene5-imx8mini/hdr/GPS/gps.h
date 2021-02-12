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

#define GPS_PORT 3  							/* "/dev/ttyAMA3" */ 
#define GPS_Baud_Rate 9600

int open_port();                                                        /*  Open GPS serial link */
int state_gps(int filedescriptor);                                      /*State of GPS*/
char* read_data_gprmc(int filedescriptor);                              /* output full GPRMC of 1 trame */
char* read_data_gprmc_parse(int filedescriptor, int gprmc_index);       /* extract part of GPRMC of 1 trame */
char* read_data_gpgga(int filedescriptor);                              /* output full GPGGA of 1 trame */
char* read_data_gpgga_parse(int filedescriptor, int gpgga_index);       /* extract part of GPGGA of 1 trame */
int close_port(int filedescriptor);                                     /* Close GPS serial link */


#endif
