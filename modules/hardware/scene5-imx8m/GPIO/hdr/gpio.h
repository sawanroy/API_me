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
 * @return true -> gpio value 1, false -> gpio value is 0, -1 -> error  
 */
int read_value_from_input_pin(int pinnumber); 


 /*!
  * Write value to output pin 
  * @param[in] pinnumber interger type variable(gpio pinnumber to write)
  * @param[in] state boolean type variable
  * @return status of function (ture or false) 
 */                  
bool write_value_to_output_gpio(int pinnumber, bool state);


/*! 
 * get the high state position
 * @param[in] pinnumber interger type variable (gpio pinnumber to read )
 * @return true -> inverter state 1, false -> inverter state 0, -1 -> error 
 */
int get_inverter_state(int pinnumber);


/*! 
 * Inverse the high state position
  * @param[in] pinnumber interger type variable(gpio pinnumber to write)
  * @param[in] state boolean type variable
  * @return status of function (ture or false)
 */
bool set_inverter_state(int pinnumber, bool state);    


/*! @} */

#endif
