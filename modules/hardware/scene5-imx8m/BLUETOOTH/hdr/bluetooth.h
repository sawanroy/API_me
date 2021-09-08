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

    int devnumb;
    char *devname;
    char *devaddress;
    /*@}*/

} bt_scanResult;



/**
 * Structure For authentication pin 
*/
typedef struct {
    /*@{*/ 
    
    char* target;               /**< target device */
    int pin;                    /**< pincode */
    /*@}*/
    
} bt_authpin;



/**
 * Structure for bluetooth configuration 
*/
typedef struct {
    /* @{ */
    bool radioOn;               /**< Enable Bluetooth radio device */
    bool enabled;               /**< Enable Bluetooth connections  */
    char name[100];                 /**< Device name */   
    bool discoverable;          /**< Enable device discoverable for Bluetooth pairing */
    int discoverabletimeout;    /**< Set the time in seconds after which other devices can discover this device over Bluetooth */
    int maxpaired;              /**< Set the limit for the maximum number of Bluetooth devices that can be paired with */
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