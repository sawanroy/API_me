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

int fd, retval, irqcount = 0;



/* internal function*/
void open_port()
{
    fd = open(default_rtc, O_RDONLY);
    if (fd == -1)
    {
        perror(default_rtc);
        exit(errno);
    }
}
/* internal function*/
void close_port()
{
    if (close(fd))
    {
        perror(default_rtc);
        exit(errno);
    }
}



void set_time(struct rtc_time time)
{
    open_port();
    retval = ioctl(fd, RTC_SET_TIME, &time);
    if (retval == -1)
    {
        perror("RTC_SET_TIME ioctl");
        exit(errno);
    }
    close_port();
}



struct rtc_time get_time()
{
    struct rtc_time rtc_tm;
    open_port();
    printf("linessss");
    retval = ioctl(fd, RTC_RD_TIME, &rtc_tm);
    printf("line1");
    if (retval == -1)
    {
        perror("RTC_RD_TIME ioctl");
        exit(errno);
    }
    fprintf(stderr, "\n\nCurrent RTC date/time is %d-%d-%d, %02d:%02d:%02d.\n",
    rtc_tm.tm_mday, rtc_tm.tm_mon + 1, rtc_tm.tm_year + 1900,
    rtc_tm.tm_hour, rtc_tm.tm_min, rtc_tm.tm_sec);
    close_port();
    return rtc_tm;
}



void set_Alarm(struct rtc_time time)
{
    open_port();
    retval = ioctl(fd, RTC_ALM_SET, &time);
    if (retval == -1)
    {
        if (errno == ENOTTY) 
        {
            fprintf(stderr,"\n...Alarm IRQs not supported.\n");
        }
        close_port();
        perror("RTC_ALM_SET ioctl");
        exit(errno);
    }
}