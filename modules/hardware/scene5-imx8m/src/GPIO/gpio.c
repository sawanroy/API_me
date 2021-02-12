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

bool read_value_from_input_pin(int pinnumber){
	int *value;
	if(gpio_get_value( pinnumber,value)) {
		return true;
	}
	else{
		return false;	
	}
}

/*
*	int write_value_to_output_gpio(int pinnumber, bool state)
*	Write value to output pin
*/

int write_value_to_output_gpio(int pinnumber, bool state) {
	if(Check_if_exported(pinnumber)) {
		gpio_export(pinnumber);
	}
	if(!gpio_set_dir(pinnumber,1)) {
		if(state==true) {	  	
			if(!gpio_set_state(pinnumber,1)) {
				return 0;
			}
			else {
				return -1;
			}                     
		}
		else {
			if(!gpio_set_state(pinnumber,0)) {
				return 0;
			}
			else {	
				return -1; 		                             
			}
		}
	}
	else {	
		return -1;
	}
	return 0;		
}

