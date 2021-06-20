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
#include <libgpio.h>


int gpio_export(unsigned int gpio)
{
	int fd, len;
	char buf[MAX_BUF];
	fd = open(SYSFS_GPIO_DIR "/export", O_WRONLY);
	if (fd < 0) 
	{
		perror("gpio/export");
		return fd;
	}
	len = snprintf(buf, sizeof(buf), "%d", gpio);
	write(fd, buf, len);
	close(fd);
	return 0;
}
 
int gpio_unexport(unsigned int gpio)
{
	int fd, len;
    	char buf[MAX_BUF];
    	fd = open(SYSFS_GPIO_DIR "/unexport", O_WRONLY);
    	if (fd < 0)
		{
        perror("gpio/export");
        return fd;
   		}
	len = snprintf(buf, sizeof(buf), "%d", gpio);
	write(fd, buf, len);
	close(fd);
	return 0;
}
 
int Check_if_exported(unsigned int gpio) 
{
	int fd;
	/*, len*/
	char buf[MAX_BUF];	
	snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR  "/gpio%d/direction", gpio);	
	fd = open(buf, O_WRONLY);
	printf("value of fd=%d\n",fd);
	if (fd < 0)
	{
		perror("gpio/direction");
		printf("unable to open gpio port not exported  %d\n",gpio);
		close(fd);
        return fd;
	}	
	else 
	{
		printf("able to open gpio port exported  %d\n",gpio);
		close(fd);
    	return 0;
    }
}

int gpio_set_dir(unsigned int gpio, unsigned int out_flag) 
{
	int fd/*, len*/;
	char buf[MAX_BUF];
	/*len =*/ 
	snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR  "/gpio%d/direction", gpio);  
	fd = open(buf, O_WRONLY);
	if (fd < 0)
	{
		perror("gpio/direction");
		return fd;
	}
	if (out_flag)
		write(fd, "out", 4);
	else
		write(fd, "in", 3);  
	close(fd);
	return 0;
}
 


int gpio_set_value(unsigned int gpio, unsigned int value)
{    
	int fd/*, len*/;    
	char buf[MAX_BUF];  
	/*len =*/ 
	snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR "/gpio%d/value", gpio);    
	fd = open(buf, O_WRONLY);    
	if (fd < 0)
	{        
		perror("gpio/set-value");        
		return fd;    
	}      
	if (value)       
		write(fd, "1", 2);    
	else        
		write(fd, "0", 2);
	close(fd);
	return 0;
}
 


int gpio_get_value(unsigned int gpio, unsigned int *value)
{
	int fd/*, len*/;    
	char buf[MAX_BUF];    
	char ch;    
	/*len =*/ 
	snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR "/gpio%d/value", gpio);    
	fd = open(buf, O_RDONLY);    
	if (fd < 0) {        
		perror("gpio/get-value");        
		return fd;    
	}    
	read(fd, &ch, 1);    
	if (ch != '0') {        
		*value = 1;    
	} 
	else {        
		*value = 0;   
	}    
	close(fd);
	return 0;
}
 


int gpio_set_edge(unsigned int gpio, char *edge)
{    
	int fd;
	/*, len*/   
	char buf[MAX_BUF];    
	/*len =*/ 
	snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR "/gpio%d/edge", gpio);    
	fd = open(buf, O_WRONLY);    
	if (fd < 0) {        
		perror("gpio/set-edge");        
		return fd;    
	}    
	write(fd, edge, strlen(edge) + 1);     
	close(fd);    
	return 0;
}

int gpio_fd_open(unsigned int gpio)
{    
	int fd;    
	char buf[MAX_BUF];     
	snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR "/gpio%d/value", gpio);    
	fd = open(buf, O_RDONLY | O_NONBLOCK );    
	if (fd < 0) {        
		perror("gpio/fd_open");
	}
	return fd;
}
 
int gpio_fd_close(int fd)  ////gpio fd close
{
    return close(fd);
}
