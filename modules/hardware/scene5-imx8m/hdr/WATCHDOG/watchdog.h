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

int get_timeout();                                          /* gets the watchdog timeout */
int get_timer();                                            /* gets the current watchdog time count */
int Watchdog_setTime(int interval);
int Watchdog_trigger();

#endif
