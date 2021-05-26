
/*
***************************************************************************
*
* Author: Sawan Roy
*
* Copyright (C) 2021 Trunexa INC
*
* Email: sawan.roy@trunexa.com
*
***************************************************************************
*/

#include <stdbool.h>
#ifndef libbluetooth_h__
#define libbluetooth_h__

typedef struct{
    int dev_numb;
    char *dev_name;
} Bt_scanResult;

struct authpin{
    char* target;               /* target device */
    int pin;                    /* pincode */
};

struct bluetooth_devices{
    char* mac;                  /* Bluetooth MAC address */
    char* name;                 /* Bluetooth Name */
    char* other;                /* Other infos */

};

struct bluetoothconfig{
    bool radioOn;               /* Enable Bluetooth radio device */
    bool enabled;               /* Enable Bluetooth connections  */
    char* name;                 /* Device name */   
    bool discoverable;          /* Enable device discoverable for Bluetooth pairing */
    int discoverableTimeout;    /* Set the time in seconds after which other devices can discover this device over Bluetooth */
    int maxPaired;              /* Set the limit for the maximum number of Bluetooth devices that can be paired with */

};

bool bluetooth_on();                                                                                    /* Activate Bluetooth */                                                                            
bool bluetooth_off();                                                                                   /* Disable Bluetooth */ 
bool bluetooth_pair_to_device(unsigned char *deviceName, int size );                                    /* Pair device with another by bluetooth */
bool bluetooth_unpair_to_device(unsigned char *deviceName, int size );                                  /* Unpair device with another bt bluetooth  */
bool bluetooth_set_config(struct bluetoothconfig conf);                                                 /* Configure Bluetooth */
struct bluetoothconfig bluetooth_get_config();                                                          /* Get Bluetotth configuration */
bool bluetooth_connect_to_device(char *name);                                                           /* Connect the device to another by bluetooth */
int bluetooth_scan( vector* v );                                                                       /* Scan for bluetooth devices */

#endif
