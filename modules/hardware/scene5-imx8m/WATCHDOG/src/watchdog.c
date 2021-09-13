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




/*
    gets the watchdog timeout
    get_timeout()
*/
int wd_get_timeout(int fd)
{
    int timeoutint=0;
    int localfd = open("/dev/watchdog0", O_RDWR);
    if(localfd < 0)
    {
        if(fd <= 0)
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
 
    if(write(localfd, "V", 1) < 0)				
    {
        close(localfd);
        return -1;
    }

    if(!ioctl(localfd, WDIOC_GETTIMEOUT, &timeoutint) == 0)	
    {
        close(localfd);
        return -1;
    }
        
    if(timeoutint > 0)
    {
        close(localfd);						
        return timeoutint;						 
    }
    else
    {
        close(localfd);
        return -1;
    }
}



/*
  get the time left of watchdog timer

*/
//////////deprecated function (because of timer function not supported in driver) ///////////////////////
int wd_get_timer(int fd)
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
    int wd_resettime(int fd)
*/
bool wd_resettime(int fd)
{
    if(fd <= 0)
    {
        return false;
    }

    if(write(fd, "\0", 1) < 0)					
    {
        return false;
    }

    ioctl(fd, WDIOC_KEEPALIVE, NULL);								
    return true;							
}



/*
    enables the watchdog
    int wd_enable()
*/
 int wd_enable()
{
    int fd = open("/dev/watchdog0", O_RDWR);
    if(fd < 0)
    {
        return -1;
    }

    return fd;
}



/*
    disables the watchdog
    int wd_disable(int fd)
*/
bool wd_disable(int fd)
{
    if(write(fd, "V", 1) < 0)				
    {
        return false;
    }

    close(fd);
    return true;
}

