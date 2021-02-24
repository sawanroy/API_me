/**
 * @file 
 * @brief API refrence for Watchdog
*/

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

#ifndef libwatchdog_h__
#define libwatchdog_h__

/*! \addtogroup WATCHDOG
	Additional documentation for group 'WATCHDOG'
      @{
*/
/*! 
 * Get the watchdog timeout 
   @return timeout of watchdog 
*/
int get_timeout();                                          
/*!
  Get the current watchdog time count 
  @return timeout of watchdog 
  
*/
int get_timer();
/*!
   Enable and disable the watchdog 
   @param[in] state bool type variable 
   @return true and false

*/
bool wd_enable(bool state);


/*! @}*/                                          
int Watchdog_setTime(int interval);
int Watchdog_trigger();
#endif
