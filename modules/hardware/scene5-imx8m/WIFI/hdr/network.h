
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
 * Structure for list of wifi  
*/
typedef struct{
    int strength; /**< Interger type variable for string strength for wifi*/
    char *ssid;   /**< Character type variable for ssid name */
} WiFi_scanResult;

/**
 * Structure for Wifi detail
*/
struct wifiinfo{
    char *ssid;        /**< Character type variable for ssid name */
    char *password;    /**< Character type variable for Password */
};

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
int wifi_connect_network(struct wifiinfo credentials);
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
bool wifi_add_to_ssid_preferred_list(struct wifiinfo credentials);
/*!
 * Get SSID preferred list
 * @param[in] SSIDs structure type variable.
*/
int wifi_get_ssid_preferred_list(vector* SSIDs);
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
int WiFi_getSignalStrength(char* SSID, int* strength);
/*!
 * @warning This function allocates memories for the structure, it is your responsibility to free those memory
   scan the wifi networks and gives the list of network presrent.
 * @param[out] v typedef varaiable for list of ssid.
*/
int wifi_scan(vector* v);

/*!
 * Reconnect to the currently connected network if it was disconnected.
*/
bool wifi_reconnect();
/*!
 * Lock to current SSID
 * @param[in] SSID variable character type variable
 * @param[in] value variable bool type variable
*/
int WiFi_setSSIDLock(char* SSID, bool value);
/*!
 * Set static WiFi IP address
 * @param[in] ipaddress  character type variable
 * @param[in] subnetmask character type variable
 * @param[in] getway  character type variable
 * @param[in] dns character type variable
*/
bool wifi_set_static_ip_Address(char* ipaddress, char* subnetmask, char* gateway, char* dns);
/*!
 * Use DHCP to get IP for WiFi
*/
int WiFi_useDHCP();

/*! 
	Switch the wifi mode into client or access point.
	@param mode bool type variable
	@return true or false
*/
bool wifi_mode(bool mode);

/*! @} */
#ifdef __cplusplus
}
#endif

#endif
