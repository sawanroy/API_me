
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
#define ENABLE_LOGS 1

#if ENABLE_LOGS
  #define dbg_log(a) printf a
#else
  #define dbg_log(a) (void)0
#endif


/**
 * Enum wifi 
*/
enum WIFI
{
  FAILURE,
  WIFI_CONNECTED,
  WIFI_INACTIVE,
  WIFI_CLIENT_DISABLED,
  WIFI_SSID_UNAVAILABLE,
  WIFI_LOCK_IS_ON,
  WIFI_ALREADY_CONNECTED, 
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
}wifi_info;


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
*/
bool wifi_activation(bool state);


/*!
 * Check whether WiFi is turned on or off.
*/
bool wifi_get_power_status();


/*!
 * Set WiFi power mode when the system start.
 * @param[in] powermode 
*/
bool wifi_set_defaultpower_mode(bool powermode);


/*!
 * Get WiFi power mode when the system starts.
*/
bool wifi_get_defaultpower_mode();


/*!
 * Connect to a WiFi network
 * @param[in] credentials structure type variable 

 * @see wifiinfo 
 * 
 * @return Success/Error codes will be returened
*/
int wifi_connect_network(wifi_info credentials);


/*!
 * Disconnect from currently activate WiFi network
 * 
*/
bool wifi_disconnect_network();


/*!
 * Add the SSID to preferred list
 * @param[in] credentials structure type variable 
 * @see wifiinfo
 * 
*/
bool wifi_add_to_ssid_preferred_list(wifi_info credentials);


/*!
 * Get SSID preferred list
 * @param[out] SSIDs structure type variable.
*/
bool wifi_get_ssid_preferred_list(vector* con_list);


/*!
 * Remove an item from preferred list
 * @param[in] SSID character type variable
 * 
*/
bool wifi_remove_from_ssid_preferred_list(char* SSID);


/*!
 * Clean the SSID preferred list
*/
bool wifi_clean_ssid_preferred_list();


/*!
 * Get WiFi signal strength
 * @param[in] SSID charcter type variable
 * @param[in] strength integer type variable
 * 
 * @return strenght of signal  
*/
int wifi_get_signal_strength(char* SSID);


/*!
 * @warning This function allocates memories for the structure, it is your responsibility to free those memory
   scan the wifi networks and gives the list of network presrent.
 * @param[out] v typedef varaiable for list of ssid.
*/
bool wifi_scan(vector* v);


/*!
 * Reconnect to the currently connected network if it was disconnected.
*/
bool wifi_reconnect();


/*!
 * Lock to current SSID
 * @param[in] SSID variable character type variable
 * @param[in] value variable bool type variable
*/
bool wifi_set_ssid_lock(char *ssid, bool enable );


/*!
 * Set static WiFi IP address
 * @param[in] ipaddress  character type variable
 * @param[in] prefixnetmask character type variable (value between 1-32) 
 * @param[in] getway  character type variable
 * @param[in] dns character type variable
*/
bool wifi_set_static(char* ipaddress, char* prefixnetmask, char* gateway, char* dns);


/*!
 * Use DHCP to enable or disable dhcp for ip address
   @param[in] enable bool type variable
   @return return true/false 
*/
bool wifi_use_dhcp(bool enable);


/*! 
    Switch the wifi mode into client or access point.
    @param[in] mode bool type variable
  @param[in] struct wifiifo containing AP name and password.
    @return true or false
*/
bool wifi_mode(bool mode, wifi_info apn);

/*! @} */
#ifdef __cplusplus
}
#endif

#endif
