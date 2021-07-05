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
 *  Get the watchdog timeout 
    @param[in] fd file discriptor of open device node
    @return timeout of watchdog

*/
int get_timeout(int fd);                                          


/*!
    Get the current watchdog time count
    @param[in] fd file discriptor of open device node
    @return timeout of watchdog 
  
*/
int get_timer(int fd);


/*!
   Enable the watchdog 
   @return file descriptor of open device node

*/
int wd_enable();


/*!
   disable the watchdog
   @param[in] fd file descriptor of open device node
   @return integer value 0 and -1

*/
int wd_disable(int fd);


/*!
   Reset watchdog timer
   @param[in] fd file discriptor of open device node 
   @return integer value 0 and -1 

*/
int wd_resettime(int fd);
/*! @}*/                                          

#endif
