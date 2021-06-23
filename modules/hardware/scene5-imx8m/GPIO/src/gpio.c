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
*	int read_value_from_input_pin(int pinnumber)
*	Read value from Input pin
*/
int read_value_from_input_pin(int pinnumber)
{
    unsigned int value = 0;
    if(check_if_exported(pinnumber))
    {
        gpio_export(pinnumber);
    }
    if(!gpio_set_dir(pinnumber, 0))
    {
        if(gpio_get_value(pinnumber, &value) == 0)
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
        return -1;
    }
}



/*
*	bool write_value_to_output_gpio(int pinnumber, bool state)
*	Write value to output pin
*/
bool write_value_to_output_gpio(int pinnumber, bool state)
{
    if(check_if_exported(pinnumber))
    {
        gpio_export(pinnumber);
    }
    if(!gpio_set_dir(pinnumber, 1))
    {
        if(state == true)
        {
            if(!gpio_set_value(pinnumber, 1))
            {
                return true;
            }
            else
            {
                return false;
            }
        }
        else
        {
            if(!gpio_set_value(pinnumber, 0))
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
*	int get_inverter_state(int pinnumber)
*	get the high state position
*/
int get_inverter_state(int pinnumber)
{
    int fd;
    char buf[MAX_BUF];
    char ch;

    if(check_if_exported(pinnumber))
    {
        gpio_export(pinnumber);
    }

    snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR "/gpio%d/active_low", pinnumber);
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
*	bool set_inverter_state(int pinnumber, bool state)
*	Inverse the high state position
*/
bool set_inverter_state(int pinnumber, bool state)
{
    int fd;
    char buf[MAX_BUF];

    snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR "/gpio%d/active_low", pinnumber);
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
