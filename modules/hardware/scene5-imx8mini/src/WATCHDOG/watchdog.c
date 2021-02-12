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
 
/*

	gets the watchdog timeout
	get_timeout()

*/
int get_timeout() {
	int fd,timeout_int=0;

	fd = open("/dev/watchdog",O_RDWR);		
	if(fd<0)					
	{
		return fd;
	}

	if(write(fd, "V", 1)<0)				
	{
		close(fd);
		return -1;
	}
   	if (ioctl(fd, WDIOC_GETTIMEOUT, &timeout_int) == 0)	
	{
      		fprintf(stdout, "Current watchdog interval is %d\n", timeout_int);
   	}
	else
	{
      		fprintf(stderr, "Error: Cannot read watchdog interval\n");
		close(fd);
      		return -1;
   	}
close(fd);	
if(timeout_int > 0)
{							
return timeout_int;						 
}
else
{
return -1;
}

}


int get_timer(){

	//gets the current watchdog time count 
	return 0;

};


/*

	Watchdog_setTime(int interval)
	reset the time to 0

*/

int Watchdog_setTime(int interval)
{
int fd;

	fd = open("/dev/watchdog",O_RDWR);			 
	if(fd<0)						
	{
		return fd;
	}
	if(write(fd, "V", 1)<0)					
	{
		close(fd);
		return -1;
	}
	if (interval > 0)					
	{
		
      		if (ioctl(fd, WDIOC_SETTIMEOUT, &interval) != 0) 	
		{
         		fprintf(stderr,"Error: Set watchdog interval failed\n");
			close(fd);
         		return -1;
      		}
		ioctl(fd, WDIOC_KEEPALIVE, NULL);			
	}
close(fd);						
return 0;							
}

/*

	enables or disables the watchdog
*/

int Watchdog_trigger()
{
int fd;
	fd = open("/dev/watchdog",O_RDWR);			
	if(fd<0)						
	{
		return -1;
	}
	close(fd);						
return 0;								
}
