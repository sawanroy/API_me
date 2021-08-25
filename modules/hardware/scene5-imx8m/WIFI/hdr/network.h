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


#ifndef HEX_NETWORK_H
#define HEX_NETWORK_H
#include <vector.h>
#include <common.h>
#include <vector.h>
#include <sys/wait.h>

/*!
  @file 
  @brief API refrence for wifi access 
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
 * Enum WIFI_CODES 
*/
enum WIFI_CODES
{
    FAILURE,                /**< FAILURE STATUS*/
    WIFI_CONNECTED,         /**< WIFI_CONNECTED STATUS*/
    WIFI_INACTIVE,          /**< WIFI_INACTIVE STATUS*/
    WIFI_CLIENT_DISABLED,   /**< WIFI_CLIENT_DISABLED STATUS*/
    WIFI_SSID_UNAVAILABLE,  /**< WIFI_SSID_UNAVAILABLE STATUS*/
    WIFI_LOCK_IS_ON,        /**< WIFI_LOCK_IS_ON STATUS*/
    WIFI_ALREADY_CONNECTED, /**< WIFI_ALREADY_CONNECTED STATUS*/
};



/**
 * Structure for list of wifi  
*/
typedef struct
{
    int strength;   /**< Interger type variable for string strength for wifi*/
    char *ssid;     /**< Character type variable for ssid name */
    char *security; /**< Character type variable for security type */
    int channel;    /**< Character type variable for channel */
} wifi_scanresult;



/**
 * Structure for Wifi detail
*/
typedef struct 
{
    char *ssid;        /**< Character type variable for ssid name */
    char *password;    /**< Character type variable for Password */
} wifi_info;


#ifdef __cplusplus
extern "C"
{
#endif

/*! \addtogroup WIFI
Additional documentation for group 'WIFI'
    @{
*/

/*!
 * Turn on WiFi.
 * @param[in] state  true or false to change the state of wifi 
 * @return status of function.
*/
bool wifi_activation(bool state);


/*!
 * Check whether WiFi is turned on or off.
 * @return status of power.
*/
bool wifi_get_power_status();


/*!
 * Set WiFi power mode when the system start.
 * @param[in] powermode 
 * @return status of function.
*/
bool wifi_set_defaultpower_mode(bool powermode);


/*!
 * Get WiFi power mode when the system starts.
 * @return status of default power mode
*/
bool wifi_get_defaultpower_mode();


/*!
 * Connect to a WiFi network
 * @param[in] wifi_info structure type variable 
 * @see wifi_info 
 * @return Success/Error codes will be returened
 * @see enum WIFI_CODES
*/
int wifi_connect_network(wifi_info credentials);


/*!
 * Disconnect from currently activate WiFi network
 * @return status of function.
*/
bool wifi_disconnect_network();


/*!
 * Add the SSID to preferred list
 * @param[in] wifi_info structure type variable 
 * @see wifi_info
 * @return status of function.
*/
bool wifi_add_to_ssid_preferred_list(wifi_info credentials);


/*!
 * Get SSID preferred list
 * @warning This function allocates memories for the structure, memory needs to be freed on application side
 * @param[out] vector typedef structure for list of wifi_info.
 * @return status of function.
*/
bool wifi_get_ssid_preferred_list(vector *con_list);


/*!
 * Remove an item from preferred list
 * @param[in] SSID character type variable
 * @return status of function.
*/
int wifi_remove_from_ssid_preferred_list(char *ssid);


/*!
 * Clean the SSID preferred list
 * @return status of function.
*/
bool wifi_clean_ssid_preferred_list();


/*!
 * Get WiFi signal strength
 * @param[in] SSID charcter type variable
 * @param[in] strength integer type variable
 * @return strenght of signal  
*/
int wifi_get_signal_strength(char *ssid);


/*!
 * Get wifi scan results
 * @warning This function allocates memories for the structure, memory needs to be freed on application side
 * @param[out] vector typedef structure for list of wifi_scanresults.
 * @return status of function.
*/
bool wifi_scan(vector* v);


/*!
 * Reconnect to the currently connected network if it was disconnected.
 * @return status of function.
*/
bool wifi_reconnect();


/*!
 * Lock to current SSID
 * @param[in] SSID variable character type variable
 * @param[in] enable variable bool type variable
 * @return status of function.
*/
bool wifi_set_ssid_lock(char *ssid, bool enable );


/*!
 * Set static WiFi IP address
 * @param[in] ipaddress  character type variable
 * @param[in] prefixnetmask character type variable (value between 1-32) 
 * @param[in] getway  character type variable
 * @param[in] dns character type variable
 * @return status of function.
*/
bool wifi_set_static(char *ipaddress, char *prefixnetmask, char *gateway, char *dns);


/*!
 * Use DHCP to enable or disable dhcp for ip address
 * @param[in] enable bool type variable
 * @return status of function. 
*/
bool wifi_use_dhcp(bool enable);


/*! 
 * Switch the wifi mode into client or access point.
 * @param[in] mode bool type variable
 * @param[in] wifi_info struct containing AP name and password.
 * @see wifi_info
 * @return status of function.
*/
bool wifi_mode(bool mode, wifi_info apn);


/*!
 * Delete the ssid connection completely
 * @param ssid ssid to be deleted
 * @return status of function (true or false)
*/
bool wifi_delete_ssid(char *ssid);


/*! @} */
#ifdef __cplusplus
}
#endif

#endif
