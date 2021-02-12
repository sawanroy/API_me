
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

#include <vector.h>
#ifndef HEX_NETWORK_H
#define HEX_NETWORK_H

typedef struct{
    int strength;
    char *ssid;
} WiFi_scanResult;

struct wifiinfo{
    char *ssid; 
    char *password;
};

#ifdef __cplusplus
extern "C"
{
#endif
/*
Turn on WiFi. (and also check the first item of preferred list to connect)
*/
bool wifi_activation(bool state);
/*
Turn off WiFi.
*/
//int WiFi_powerOff();
/*
Check whether WiFi is turned on or off.
*/
bool wifi_get_power_status();
/*
Set WiFi power mode when the system start.
*/
bool wifi_set_defaultpower_mode(bool powermode);
/*
Get WiFi power mode when the system starts.
*/
bool wifi_get_defaultpower_mode();
/*
Connect to a WiFi network
*/
int wifi_connect_network(struct wifiinfo credentials);
/*
disconnect from currently activate WiFi network
*/
bool wifi_disconnect_network();
/*
Add a network to SSID preferred list (the device check to connect to preferred list networks after reconnect time)
*/
bool wifi_add_to_ssid_preferred_list(struct wifiinfo credentials);
/*
Get SSID preferred list
*/
int wifi_get_ssid_preferred_list(vector* SSIDs);
/*
Remove an item from preferred list. (and also shift up lower priorities)
*/
bool wifi_remove_from_ssid_preferred_list(char* SSID);
/*
Clean the SSID preferred list
*/
bool wifi_clean_ssid_preferred_list();
/*
Get WiFi signal strength
*/
int WiFi_getSignalStrength(char* SSID, int* strength);
// int WiFi_getSignalStrength(WiFi_scanResult *); //in "C" we don't have overloading, or templating
/*
NOTE: this function allocates memories for the structure, it is your responsibility to free those memory
*/
int WiFi_getSignalsStrength(vector *v);

/*
Reconnect to the currently connected network if it was disconnected.
*/
bool wifi_reconnect();
/*
Allows an application to keep the WiFi radio awake.
*/
int WiFi_setSSIDLock(char* SSID, bool value);
/*
Set static IP address WiFi
*/
bool wifi_set_static_ip_Address(char* ipaddress, char* subnetmask, char* gateway, char* dns);
/*
Use DHCP to get IP for WiFi
*/
int WiFi_useDHCP();

#ifdef __cplusplus
}
#endif

#endif
