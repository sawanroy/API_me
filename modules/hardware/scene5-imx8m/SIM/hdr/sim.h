/**
 * @file
 * @brief API refrence for SIM or GPRS
*/

/*
***************************************************************************
*
* Author:Sawan Roy
*
* Copyright (C) 2021 TRUNEXA INC
*
* Email: sawan.roy@trunexa.com
*
***************************************************************************
*/


#ifndef sim_h__
#define sim_h__

#include <stdbool.h>
#include <vector.h>


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
 * Structure for APN configuration
*/

typedef struct
{
    char *apn_name;                          /**<	Acess point name */
    char *username;                          /**<	username */
    char *password;                          /**<	password */
    char *authentication;                    /**<	type char security protocol : PAP or CHAP */
    char *apnProtocol;                       /**<	protocol to be used to access internet (ipv4 ipv6) */
} sim_apn;

 /**
  * Structure for List if APN
 */
struct list_apn
{
    char name[10];
};


/*! \addtogroup SIM
    Additional documention for group 'SIM'
   @{
*/


/*!
  Activate the sim card by using sim pin
  @param[in] pincode integer type variable
  @return true when success and false when failure.
*/
bool sim_unlock(int pincode);


bool sim_deactivate();
/*!
  Connect to telephone network to dial
  @param[in] Phone_no unsigned char type variable
  @return true when success and false when failure.
*/
bool sim_dialup_connect(unsigned char phone_no[]);


/*!
  Disconnect from telephone network
  @return true when success and false when failure.
*/
bool sim_dialup_disconnect();


/*!
  Create or update an AP (identified by its name)
  @param[in] apn struct type argument
  @return true when success and false when failure.
*/
bool sim_set_ap(sim_apn setapn);


/*!
  list already configured AP
  @param[out] vector typedef structure for list of sim_apn.
  @return true when success and false when failure.
*/
bool sim_list_ap(vector *aplist);


/*!
  Remove an AP from the list (identified by its name)
  @param ap_name charater type variable
  @return true when success and false when failure.
*/
bool sim_remove_ap(char* ap_name);


/*!
  Connect to internet
  @return true when success and false when failure.
*/
bool sim_connect_to_internet();


/*!
  Get ip address
  @return ip address.
*/
char* sim_get_ipaddress();


/*!
  Get the strengh of the signal (in dbm)
  @return signal strength in dbm
*/
int sim_get_signal_strength();


/*!
  Get the IMSI
  @return IMSI number
*/
char* sim_get_imsi();


/*!
  Get the IMEI
  @return IMEI number
*/
char* sim_get_imei();


/*!
  Send the SMS
  @param[in] message character type variable.
  @param[in] phone_no character type variable.
  @return true and false

*/
bool sim_send_sms(char* phoneno, char* message);


/*!
  @}
*/
#endif


