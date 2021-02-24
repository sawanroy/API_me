/**
 * @file
 * @brief API refrence for GPIO
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

#ifndef gpio_h__
#define gpio_h__

/*! \addtogroup GPIO
	Additional documentation for group 'GPIO'
      @{
*/

/*!
 * Read value from Input pin
 * @param[in] pinnumber interger type variable (gpio pinnumber to read ) 
 * @return true when gpio value is 1 or false when gpio value is 0  
 */
bool read_value_from_input_pin(int pinnumber);    
 /*!
  * Write value to output pin 
  * @param[in] pinnumber interger type variable(gpio pinnumber to write) 
  * @return ture or false 
 */                  
int write_value_to_output_gpio(int pinnumber, bool state);     

/*! @} */

#endif
