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
    int fd;
    fd=serial_open(GPS_PORT,GPS_Baud_Rate);
	if(fd<0) {
		return  -1;
	}
    	else {
		return fd;
	}
}
/*

	state_gps(int filedescriptor)
	State of GPS

*/
int state_gps(int filedescriptor){
    return 0;
}

/*

	char* read_data_gprmc(int filedescriptor)
	output full GPRMC of 1 trame

*/

char* read_data_gprmc(int filedescriptor){
}

/*

	char* read_data_gprmc_parse(int filedescriptor, int gprmc_index)
	extract part of GPRMC of 1 trame

*/

char* read_data_gprmc_parse(int filedescriptor, int gprmc_index){
}

/*
	char* read_data_gpgga(int filedescriptor)
	output full GPGGA of 1 trame

*/

char* read_data_gpgga(int filedescriptor){
}

/*

	char* read_data_gpgga_parse(int filedescriptor, int gpgga_index)
	extract part of GPGGA of 1 trame

*/

char* read_data_gpgga_parse(int filedescriptor, int gpgga_index){
}

/*

	close_port(int filedescriptor)
	Close GPS serial link

*/

int close_port(int filedescriptor)
{
	if(filedescriptor>0) {
		if(!serial_close(filedescriptor)) {
			return 0;	
		}
		else {
			return -1;
		}
	}
	else {
		return -1;
	}
}
	
