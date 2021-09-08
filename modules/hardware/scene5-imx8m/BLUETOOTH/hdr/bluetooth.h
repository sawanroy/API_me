/*!
    @file
    @ API for bluetooth 
 */
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


#ifndef libbluetooth_h__
#define libbluetooth_h__
#include <stdbool.h>
#include "../../WIFI/hdr/vector.h"



/**
 *Structure for bluetooth scan results
 *
*/
typedef struct{
    /*@{*/
    char *devname;             /**< Char type variable for BT device name*/
    char *devaddress;          /**< Char type variable for MAC address of BT device*/
    /*@}*/

} bt_scanresult;



/**
 * Structure For authentication pin 
*/
typedef struct {
    /*@{*/ 
    
    char *target;               /**< target device */
    int pin;                    /**< passkey */
    /*@}*/
    
} bt_authpin;



/**
 * Structure for bluetooth configuration 
*/
typedef struct {
    /* @{ */
    bool radioOn;               /**< Bool type variable for radio status */
    bool enabled;               /**< Bool type variable to enable BT connection*/
    char name[100];             /**< Char type variable for BT device name */   
    bool discoverable;          /**< Bool type variable to enable device discoverable for Bluetooth pairing */
    int discoverabletimeout;    /**< Int type variable to set the time in seconds after which other devices cannot discover this device over Bluetooth */
    int maxpaired;              /**< Int type variable to set the limit for the maximum number of Bluetooth devices that can be paired */
    /* @} */
} bt_config;



/*! \addtogroup BLUETOOTH
    Additional documentation for group 'BLUETOOTH'
   @{    
*/

/*!
  Activate bluetooth 
  @return true and false 
 */
bool bluetooth_on();   


/*! 
 Disable bluetooth 
 @return true and false 
 */                                                                          
bool bluetooth_off();


/*!
 * Pair the device with another device
  @param[in] deviceName unsinged char type variable (bleutooth device name) 
  @param[in] size integer type variable 
  @return true and false 
 */                                                                                   
bool bluetooth_pair_device(unsigned char *devicename);


 /*!
  * Unpair from bluetooth device
  * @param[in]  devicename unsinged char type variable (bleutooth device name) 
    @param[in] size integer type variable
    @return true and false 
  */                                    
bool bluetooth_unpair_device(unsigned char *devicename);  
  
  
/*!
 * Configure bluetooth 
 * @param[in] conf structure bluetoothconfig type variable
 * @see bluetoothconfig  
 */                             
bool bluetooth_set_config(bt_config conf); 


/*!
 *Get bluetooth configuration
 @return true and false 
 */                                                
bt_config bluetooth_get_config();


/*!
 * Connect to bluetooth device
   @param[in] name character type variable 
 */                                                         
bool bluetooth_connect_to_device(char *name);  


/*!
 *Scan for bluetooth devices 
  @param[in] v typedef type strcture variable 
  @return true and false 
 */                                                         
vector bluetooth_scan();       


/*! @} */
#endif