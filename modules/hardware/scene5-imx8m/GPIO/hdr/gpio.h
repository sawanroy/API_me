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
 * @param[in] pin_number interger type variable (gpio pin_number to read ) 
 * @return true when gpio value is 1 or false when gpio value is 0  
 */
int read_value_from_input_pin(int pin_number); 


 /*!
  * Write value to output pin 
  * @param[in] pin_number interger type variable(gpio pin_number to write)
  * @param[in] state boolean type variable
  * @return ture or false 
 */                  
bool write_value_to_output_gpio(int pin_number, bool state);


/*! 
 * get the high state position
 * @param[in] pin_number interger type variable (gpio pin_number to read )
 * @return value of inverter state
 */
int get_inverter_state(int pin_number);


/*! 
 * Inverse the high state position
  * @param[in] pin_number interger type variable(gpio pin_number to write)
  * @param[in] state boolean type variable
 */
void set_inverter_state(int pin_number, bool state);    


/*! @} */

#endif
