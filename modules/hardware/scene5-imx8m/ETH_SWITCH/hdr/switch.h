/**
 * @file
 * @brief APIs refernce for RS232
*/
/*
***************************************************************************
*
* Author: Nikhil Anjane
*
* Copyright (C) 2021 TRUNEXA INC
*
* Email: nikhil.anjane@trunexa.com
*
***************************************************************************
*/


#include <stdbool.h>
#include <stdio.h>
#include <nmcommon.h>


/**
 * Enable/Disable logs
*/
#define ENABLE_LOGS 1

#if ENABLE_LOGS
    #define dbg_log(a) printf a
#else
    #define dbg_log(a) (void)0
#endif


typedef enum 
{ 
    MANUAL, 
    SHARED, 
    DYNAMIC,
}SW_MODE;

typedef enum
{ 
    ETH0=0,
    PORT1=1, 
    PORT2=2,
    PORT3=3, 
    PORT4=4, 
    PORT5=5, 
    PORT6=6, 
    PORT7=7,
}SW_PORT;

typedef struct{
    SW_MODE port_mode;
    const char *port_ip;
    int  port_prefix;
    const char *port_gateway;
    const char *port_dns;
}port_config;

typedef enum
{
    RESET_FAILURE=0,
    SUCCESS=1,
    RELOAD_FAILURE=-1,
    ADDITION_FAILURE=-2,
    ACTIVATION_FAILURE=-3,
    MODIFY_FAILURE=-4,
    REAPPLY_FAILURE=-5,
}ERROR_CODE;

int switch_init();
int switch_set_config( SW_PORT port_num, port_config config);
bool switch_get_config( SW_PORT port_num, port_config *config);
bool switch_port_reset(SW_PORT port_num);