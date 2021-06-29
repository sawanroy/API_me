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
/* Internal functions */
int open_watchdog()
{
    
    fd = open("/dev/watchdog0", O_RDWR);		
	return fd;
}



/*

	gets the watchdog timeout
	get_timeout()

*/
int get_timeout()
{
    int timeoutint=0;
	fd = open_watchdog();
    if(fd < 0)
    {
        return -1;
    }

	if(write(fd, "V", 1) < 0)				
	{
		close(fd);
		return -1;
	}

   	if (ioctl(fd, WDIOC_GETTIMEOUT, &timeoutint) == 0)	
	{
      	fprintf(stdout, "Current watchdog interval is %d\n", timeoutint);
   	}
	else
	{
      	fprintf(stderr, "Error: Cannot read watchdog interval\n");
		close(fd);
      	return -1;
   	}
    close(fd);	
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
int get_timer()
{
    long long int timeleft;
	fd = open_watchdog();
    if(fd < 0)
    {
        return -1;
    }

	if(write(fd, "V", 1) < 0)				
	{
		close(fd);
		return -1;
	}
	
	if(!ioctl(fd, WDIOC_GETTIMELEFT, &timeleft))
    {
		printf("The timeout was is %lld seconds\n", timeleft);
	}
	else
    {
		printf("WDIOC_GETTIMELEFT error '%s'\n", strerror(errno));
        close(fd);
        return -1;
	}
        
    close(fd);
	return timeleft;
}



/*

	Watchdog_setTime(int interval)
	

*/
int Watchdog_resetTime()
{
	fd = open_watchdog();
    if(fd < 0)
    {
        return -1;
    }

	if(write(fd, "\0", 1) < 0)					
	{
		close(fd);
		return -1;
	}
	    ioctl(fd, WDIOC_KEEPALIVE, NULL);			
    close(fd);						
    return 0;							
}

/*

	enables or disables the watchdog
*/
bool wd_enable(bool state)
{
    fd = open_watchdog();
    if(fd < 0)
    {
        return -1;
    }

    if(write(fd, "V", 1) < 0)				
    {
    	close(fd);
    	return -1;
    }
	
    if(state)
    {
    	int flags = WDIOS_ENABLECARD;
    	int ret = ioctl(fd, WDIOC_SETOPTIONS, &flags);
    	if(!ret)
        {
            printf("Watchdog card disabled.\n");
        }
        else 
        {
            printf("WDIOS_DISABLECARD error");
            close(fd);
            return false;
        }
        close(fd);
        return true;
	}
	else
    {
		int flags = WDIOS_DISABLECARD;
		int	ret = ioctl(fd, WDIOC_SETOPTIONS, &flags);
		if(!ret)
        {
            printf("Watchdog card enabled.\n");
        }
        else
        {
            printf("WDIOS_ENABLECARD error ");
            close(fd);
            return false;
        }
        close(fd);
        return true;
	}
}
