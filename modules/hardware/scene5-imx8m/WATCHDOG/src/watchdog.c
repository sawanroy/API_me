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



/* Internal functions */
int open_watchdog()
{
    int fd;
    fd = open("/dev/watchdog", O_RDWR);		
	return fd;
}



/*

	gets the watchdog timeout
	get_timeout()

*/
int get_timeout()
{
    int fd,timeoutint=0;
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
	int fd;
    int timeleft;
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
	
	if(ioctl(fd, WDIOC_GETTIMELEFT, &timeleft))
    {
		printf("The timeout was is %d seconds\n", timeleft);
	}
	else
    {
		printf("Error while getting time left");
        close(fd);
        return -1;
	}
        
    close(fd);
	return timeleft;

}



/*

	Watchdog_setTime(int interval)
	

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

bool wd_enable(bool state){
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
	
	if(state){
		   int flags = WDIOS_ENABLECARD;
			int ret = ioctl(fd, WDIOC_SETOPTIONS, &flags);
			if (!ret)
				printf("Watchdog card disabled.\n");
			else {
				printf("WDIOS_DISABLECARD error");
}
		return true;
	}
	else{
		int flags = WDIOS_DISABLECARD;
		int	ret = ioctl(fd, WDIOC_SETOPTIONS, &flags);
			if (!ret)
				printf("Watchdog card enabled.\n");
			else {
				printf("WDIOS_ENABLECARD error ");
			
}
		return true;
	}
}
