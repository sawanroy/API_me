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
#include <stdbool.h>

int fd;



/*

	gets the watchdog timeout
	get_timeout()

*/
int get_timeout(int fd)
{
    int timeoutint=0;
    if(fd < 0)
    {
        return -1;
    }

   	if(!ioctl(fd, WDIOC_GETTIMEOUT, &timeoutint) == 0)	
	{
      	return -1;
   	}
    	
    if(timeoutint > 0)
    {							
        return timeoutint;						 
    }
    else
    {
        return -1;
    }
}



/*
  get the time left of watchdog timer
   get_timer()

*/

//////////deprecated function (because of timer function not supported in driver) ///////////////////////
int get_timer(int fd)
{
    long long int timeleft;
    if(fd < 0)
    {
        return -1;
    }
	
	if(!ioctl(fd, WDIOC_GETTIMELEFT, &timeleft))
    {
        return -1;
	}
	return timeleft;
}



/*
	Watchdog_setTime(int interval)
*/
int wd_resettime(int fd)
{
    if(fd <= 0)
    {
        return -1;
    }

	if(write(fd, "\0", 1) < 0)					
	{
		return -1;
	}

	ioctl(fd, WDIOC_KEEPALIVE, NULL);								
    return 0;							
}

/*
	enables or disables the watchdog
*/
 int wd_enable(bool state)
{
    if(state)
    {
        fd = open("/dev/watchdog0", O_RDWR);
        if(fd < 0)
        {
            return -1;
        }
        return fd;
	}
	else
    {
        if(write(fd, "V", 1) < 0)				
	    {
		    close(fd);
		    return -1;
	    }
        close(fd);
        return 0;
	}
}

