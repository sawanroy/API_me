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
#include <fcntl.h>
#include <unistd.h>



/*
*	bool read_value_from_input_pin(int pin_number)
*	Read value from Input pin
*/
int read_value_from_input_pin(int pin_number)
{
    unsigned int value = 0;
	if(check_if_exported(pin_number))
    {
        gpio_export(pin_number);
    }
    if(!gpio_set_dir(pin_number, 0))
    {
        if(gpio_get_value(pin_number, &value) == 0)
        {
            return value;
        }
        else
        {
		    return -1;       
        }
    }
    else
    {
        return false;
    }
}



/*
*	int write_value_to_output_gpio(int pin_number, bool state)
*	Write value to output pin
*/
bool write_value_to_output_gpio(int pin_number, bool state)
{
    if(check_if_exported(pin_number))
    {
        gpio_export(pin_number);
    }
    if(!gpio_set_dir(pin_number, 1))
    {
        if(state == true)
        {
            if(!gpio_set_value(pin_number, 1))
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
            if(!gpio_set_value(pin_number, 0))
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
}



/*
*	bool get_inverter_state(int pin_number)
*	get the high state position
*/
int get_inverter_state(int pin_number)
{
    int fd;
    char buf[MAX_BUF];
    char ch;

    if(check_if_exported(pin_number))
    {
        gpio_export(pin_number);
    }

    snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR "/gpio%d/active_low", pin_number);
    fd = open(buf, O_RDONLY);
    if(fd < 0)
    {
        perror("gpio/active_low");
        return fd;
    }
    read(fd, &ch, 1);
    if(ch != '0')
    {
        close(fd);
        return 1;
    }
    else
    {
        close(fd);
        return 0;
    }
}



/*
*	void set_inverter_state(int pin_number, bool state)
*	Inverse the high state position
*/
bool set_inverter_state(int pin_number, bool state)
{
    int fd;
    char buf[MAX_BUF];

    snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR "/gpio%d/active_low", pin_number);
    fd = open(buf, O_WRONLY);
    if(fd < 0)
    {
        perror("gpio/set-value");
        return false;
    }
    if(state)
    {
        write(fd, "1", 2);
        close(fd);
        return true;
    }
    else
    {
        write(fd, "0", 2);
        close(fd);
        return true;
    }
    close(fd);
}
