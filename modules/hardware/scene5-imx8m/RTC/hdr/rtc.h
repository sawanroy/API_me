/**
 * @file 
 * @brief API refrence for RTC
*/


#ifndef rtc_h__
#define rtc_h__

/*! \addtogroup RTC
	Additional documentation for group 'RTC'
      @{
*/
#include <linux/rtc.h>
#include <stdbool.h>


/*!
	Set the RTC time
	@param[in] rtc_time structure type variable
	@return status of function (ture or false) 

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
	@return status of function (ture or false)
*/
bool set_alarm(struct rtc_time rtc_tm);

/*! @}*/  
#endif