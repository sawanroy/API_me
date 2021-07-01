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
#include <time.h>

int retval, irqcount = 0;

int set_time(struct rtc_time time)
{
    int fd;
    fd = open("/dev/rtc0", O_RDWR);
    if (fd < 0)
    {
        return -1;
    }
    retval = ioctl(fd, RTC_SET_TIME, &time);
    if (retval == -1)
    {
        perror("RTC_SET_TIME ioctl");
        return -1;
    }
    close(fd);
}



int64_t get_time()
{
    time_t t_of_day;
    struct rtc_time rtc_tm;
    struct tm t;
    char buf[80];
    int fd;
    fd = open("/dev/rtc0", O_RDWR);
    if (fd < 0)
    {
        return -1;
    }
    retval = ioctl(fd, RTC_RD_TIME, &rtc_tm);
    if (retval == -1)
    {
        perror("RTC_RD_TIME ioctl");
        return -1;
    }
    t.tm_year = rtc_tm.tm_year; 
    t.tm_mon = rtc_tm.tm_mon;
    t.tm_mday = rtc_tm.tm_mday;
    t.tm_hour = rtc_tm.tm_hour;
    t.tm_min = rtc_tm.tm_min;
    t.tm_sec = rtc_tm.tm_sec;
    t.tm_isdst = -1;
    t_of_day = mktime(&t);
    close(fd);
    return t_of_day;
}



int set_Alarm(struct rtc_time rtc_tm)
{
    int fd;
    fd = open("/dev/rtc0", O_RDWR);
    if (fd < 0)
    {
        return -1;
    }
    retval = ioctl(fd, RTC_ALM_SET, &rtc_tm);
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