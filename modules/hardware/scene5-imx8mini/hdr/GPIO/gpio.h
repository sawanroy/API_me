

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

#ifndef gpio_h__
#define gpio_h__

bool read_value_from_input_pin(int pinnumber);                      /* Read value from Input pin */
int write_value_to_output_gpio(int pinnumber, bool state);          /* Write value to output pin */     

#endif
