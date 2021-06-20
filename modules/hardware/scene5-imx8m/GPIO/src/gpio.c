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
*	bool read_value_from_input_pin(int pinnumber)
*	Read value from Input pin
*/

int read_value_from_input_pin(int pinnumber)
{
	int *value;
	int check;
	if(gpio_get_value( pinnumber,value))
	{
			return value;
	}
	else
	{
		check = Check_if_exported(pinnumber);
		if(check == 0)
		{

			return -1;
		}
		else
		{
			if(gpio_export(pinnumber)==0)
			{
				if(gpio_get_value( pinnumber,value))
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
*	int write_value_to_output_gpio(int pinnumber, bool state)
*	Write value to output pin
*/

int write_value_to_output_gpio(int pinnumber, bool state)
{
	if(Check_if_exported(pinnumber))
	{
		gpio_export(pinnumber);
	}
	if(!gpio_set_dir(pinnumber,1))
	{
		if(state==true)
		{	  	
			if(!gpio_set_value(pinnumber,1))
			{
				return 0;
			}
			else 
			{
				return -1;
			}                     
		}
		else
		{
			if(!gpio_set_value(pinnumber,0))
			{
				return 0;
			}
			else
			{	
				return -1; 		                             
			}
		}
	}
	else
	{	
		return -1;
	}
	return 0;		
}

/*
*	bool get_inverter_state(int pinnumber)
*	get the high state position
*/

bool get_inverter_state(int pinnumber)
{
	int state ;
	state = read_value_from_input_pin(pinnumber);
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
*	void set_inverter_state(int pinnumber, bool state)
*	Inverse the high state position
*/

void set_inverter_state(int pinnumber, bool state)
{
	if(state == true)
	{
		write_value_to_output_gpio(pinnumber,false);
	}
	else
	{
		write_value_to_output_gpio(pinnumber,true);
	}
	
}
