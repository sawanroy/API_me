
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

#ifndef libeth_switch_h__
#define libeth_switch_h__

#include <stdbool.h>
#include <stdio.h>
#include <nmcommon.h>

/**
 * @file
 * @brief APIs refernce for ETH_SWITCH
*/

/**
 * Enable/Disable logs
*/
#define ENABLE_LOGS 0

#if ENABLE_LOGS
    #define dbg_log(a) printf a
#else
    #define dbg_log(a) (void)0
#endif


/*!
 * Enum SW_MODE
*/
typedef enum
{
    MANUAL,     /*!<Manual configuration*/
    SHARED,     /*!<Shared configuration*/
    DYNAMIC,    /*!<Dynamic/Auto configuration*/
}SW_MODE;


/*!
 * Enum SW_PORT
*/
typedef enum
{
    ETH0=0,     /*!< iface eth0 */
    PORT1=1,    /*!< iface port1 */
    PORT2=2,    /*!< iface port2 */
    PORT3=3,    /*!< iface port3 */
    PORT4=4,    /*!< iface port4 */
    PORT5=5,    /*!< iface port5 */
    PORT6=6,    /*!< iface port6 */
    PORT7=7,    /*!< iface port7 */
}SW_PORT;


/*!
 * Enum ERROR_CODE
*/
typedef enum
{
    RESET_FAILURE=0,        /**< Port reset failure*/
    SUCCESS=1,              /**< SUCCESS*/
    RELOAD_FAILURE=-1,      /**< Reload connections failure*/
    ADDITION_FAILURE=-2,    /**< Addition of connection failure*/
    ACTIVATION_FAILURE=-3,  /**< Activate connection failure*/
    MODIFY_FAILURE=-4,      /**< Modify connection failure*/
    REAPPLY_FAILURE=-5,     /**< Reapply configuration to device failure*/
}ERROR_CODE;


/**
 * Structure for port configuration
*/
typedef struct{
    SW_MODE port_mode;          /**< SW_MODE type variable to set mode*/
    const char *port_ip;        /**< Constant character type variable for IP address*/
    const char *port_prefix;    /**< Integer type variable for subnet prefix(0-24)*/
    const char *port_gateway;   /**< Constant character type variable for gateway address*/
    const char *port_dns;       /**< Constant character type variable for DNS address*/
}port_config;



/*! \addtogroup ETH_SWITCH
  Additional documentation for group 'ETH_SWITCH'
      @{
*/


/*!
 * Initialize ethernet switch
 * @return error code from ERROR_CODE enum
*/
int switch_init();


/*!
 * Set config for switch port
 * @param[in] port number from SW_PORT enum
 * @param[in] structure containing configuration data
 * @return error code from ERROR_CODE enum
*/
int switch_set_config(SW_PORT port_num, port_config config);


/*!
 * Get config of switch port
 * @param[in] port number fromSW_PORT enum
 * @param[out] structure pointer containing configuration data
 * @return status of function(True/False)
*/
bool switch_get_config(SW_PORT port_num, port_config *config);


/*!
 * Reset switch port
 * @param[in] port number from SW_PORT enum
 * @return status of function(True/False)
*/
bool switch_port_reset(SW_PORT port_num);

/*! @} */

#endif