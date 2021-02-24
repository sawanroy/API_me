
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

typedef struct vector_ {
    char* data;
    int size;
    int count;
} vector1;

/**
 *Structure for bluetooth scan results
 *
*/

typedef struct{
    /*@{*/

    int dev_numb;
    char *dev_name;

    /*@}*/

} Bt_scanResult;

/**
 * Structure For authentication pin 
*/
struct authpin{
    /*@{*/
    char* target;               /**< target device */
    int pin;                    /**< pincode */
    /*@}*/
};
/**
 * Structure for Bluetooth devices information
*/

struct bluetooth_devices{
    /*@{*/
    char* mac;                  /**< Bluetooth MAC address */
    char* name;                 /**< Bluetooth Name */
    char* other;                /**< Other infos */
    /*@}*/
};

/**
 * Structure for bluetooth configuration 
*/
struct bluetoothconfig{
    /* @{ */
    bool radioOn;               /**< Enable Bluetooth radio device */
    bool enabled;               /**< Enable Bluetooth connections  */
    char* name;                 /**< Device name */   
    bool discoverable;          /**< Enable device discoverable for Bluetooth pairing */
    int discoverableTimeout;    /**< Set the time in seconds after which other devices can discover this device over Bluetooth */
    int maxPaired;              /**< Set the limit for the maximum number of Bluetooth devices that can be paired with */
    /* @} */
};

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
bool bluetooth_pair_to_device(unsigned char *deviceName, int size );
 /*!
  * Unpair from bluetooth device
  * @param[in]  devicename unsinged char type variable (bleutooth device name) 
    @param[in] size integer type variable
    @return true and false 
  */                                    
bool bluetooth_unpair_to_device(unsigned char *deviceName, int size );    
/*!
 * Configure bluetooth 
 * @param[in] conf structure bluetoothconfig type variable
 * @see bluetoothconfig  
 */                             
bool bluetooth_set_config(struct bluetoothconfig conf); 
/*!
 *Get bluetooth configuration
 @return true and false 
 */                                                
struct bluetoothconfig bluetooth_get_config();
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
int bluetooth_scan( vector1* v );       

/*! 
   @}
*/

#endif