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

#ifndef libgpio_h__
#define libgpio_h__

#define SYSFS_GPIO_DIR "/sys/class/gpio"
#define MAX_BUF 64


int gpio_export(unsigned int gpio);

int gpio_unexport(unsigned int gpio); //gpio unexport

int check_if_exported(unsigned int gpio);

int gpio_set_dir(unsigned int gpio, unsigned int out_flag);  //gpio set dir

int gpio_set_value(unsigned int gpio, unsigned int value);  //gpio set value

int gpio_get_value(unsigned int gpio, unsigned int value);  //gpio get value

int gpio_set_edge(unsigned int gpio, char *edge);  //gpio set edge

int gpio_fd_open(unsigned int gpio);  //gpio fd open

int gpio_fd_close(int fd);  ////gpio fd close


#endif
