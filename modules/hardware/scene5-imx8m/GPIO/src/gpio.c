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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <libgpio.h>
#include <gpio.h>



/*
*	bool read_value_from_input_pin(int pin_number)
*	Read value from Input pin
*/
int read_value_from_input_pin(int pin_number)
{
	int value;
	int check;
	if(gpio_get_value(pin_number,value))
	{
		return value;
	}
	else
	{
		check = check_if_exported(pin_number);
		if(check == 0)
		{

			return -1;
		}
		else
		{
			if(gpio_export(pin_number) == 0)
			{
				if(gpio_get_value(pin_number,value))
				{
					return value;
					
				}
			}
			else
			{
				return -1;
			}	
		}
	}
}



/*
*	int write_value_to_output_gpio(int pin_number, bool state)
*	Write value to output pin
*/
bool write_value_to_output_gpio(int pin_number, bool state)
{
	if(Check_if_exported(pin_number))
	{
		gpio_export(pin_number);
	}
	if(!gpio_set_dir(pin_number,1))
	{
		if(state == true)
		{	  	
			if(!gpio_set_value(pin_number,1))
			{
				return true;
			}
			else 
			{
				return true;
			}                     
		}
		else
		{
			if(!gpio_set_value(pin_number,0))
			{
				return true;
			}
			else
			{	
				return false; 		                             
			}
		}
	}
	else
	{	
		return false;
	}
	return 0;		
}



/*
*	bool get_inverter_state(int pin_number)
*	get the high state position
*/
bool get_inverter_state(int pin_number)
{
	int state ;
	state = read_value_from_input_pin(pin_number);
	if(state == 1)
	{
		return false;
	}
	else
	{
		return true;
	}
}



/*
*	void set_inverter_state(int pin_number, bool state)
*	Inverse the high state position
*/
void set_inverter_state(int pin_number, bool state)
{
	if(state == true)
	{
		write_value_to_output_gpio(pin_number,false);
	}
	else
	{
		write_value_to_output_gpio(pin_number,true);
	}
	
}