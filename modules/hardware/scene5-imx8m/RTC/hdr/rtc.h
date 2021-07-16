
#ifndef rtc_h__
#define rtc_h__

#include <linux/rtc.h>
#include <stdbool.h>

/*!
	@param[in] rtc_time structure type variable
	@return true on success false on failure 

*/
bool set_time(struct rtc_time rtc_tm);


/*!
	Get the time from rtc
	@return int64_t type variable return the time
*/
int64_t get_time();


/*! 
	To set the Alarm for RTC
	@param[in] rtc_time structure type variable
	@return true on success false on failure
*/
bool set_alarm(struct rtc_time rtc_tm);

#endif