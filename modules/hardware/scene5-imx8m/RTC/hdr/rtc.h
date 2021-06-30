
#ifndef rtc_h__
#define rtc_h__

#include <linux/rtc.h>

//const char default_rtc[] = "/dev/rtc0";

/*!
   Set the time to rtc
   @return 

*/
int set_time(struct rtc_time time);
/*!
struct rtc_time {
	int tm_sec;
	int tm_min;
	int tm_hour;
	int tm_mday;
	int tm_mon;
	int tm_year;
	int tm_wday;
	int tm_yday;
	int tm_isdst;
};
*/
struct rtc_time get_time();
/*! 
 void set_Alarm()
 to set the Alarm for RTC
*/
int set_Alarm(struct rtc_time time);
#endif