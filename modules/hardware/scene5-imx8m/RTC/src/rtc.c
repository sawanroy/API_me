#include <stdio.h>
#include <linux/rtc.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <stdint.h>
#include <rtc.h>

int retval, irqcount = 0;



int set_time(struct rtc_time time)
{
    int fd;
    fd = open("/dev/rtc0", O_RDWR);
    retval = ioctl(fd, RTC_SET_TIME, &time);
    if (retval == -1)
    {
        perror("RTC_SET_TIME ioctl");
        return -1;
    }
    close(fd);
}



struct rtc_time get_time()
{
    struct rtc_time rtc_tm;
    int fd;
    fd = open("/dev/rtc0", O_RDWR);
    retval = ioctl(fd, RTC_RD_TIME, &rtc_tm);
    if (retval == -1)
    {
        perror("RTC_RD_TIME ioctl");
        exit(errno);
    }
    close(fd);
    return rtc_tm;
}



int set_Alarm(struct rtc_time time)
{
    int fd;
    fd = open("/dev/rtc0", O_RDWR);
    retval = ioctl(fd, RTC_ALM_SET, &time);
    if (retval == -1)
    {
        if (errno == ENOTTY) 
        {
            fprintf(stderr,"\n...Alarm IRQs not supported.\n");
        }
        close(fd);
        perror("RTC_ALM_SET ioctl");
        return -1;
    }
}