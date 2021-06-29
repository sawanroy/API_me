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

/**
 * Structure for APN configuration
*/

struct apn
{
    char *apn_name;                          /**<	Acess point name */
    char Proxy[20];                             /**<	proxy url */
    int  Port;                                  /**<	port number */
    char username[20];                          /**<	username */
    char password[20];                          /**<	password */
    char mmsc[20];                              /**<	mmsc url */
    char mmsProxy[20];                          /**<	proxy ip */
    int  mmsPort[20];                           /**<	port number */
    int  mcc;                                   /**<	Mobile Country Code */
    int  mnc;                                   /**<	Mobile Network Code */
    char authentication[20];                    /**<	type char security protocol : PAP or CHAP */
    char apnType[20];                           /**<	type of data which must use this APN configuration */
    char apnProtocol[20];                       /**<	protocol to be used to access internet (ipv4 ipv6) */
    char apnRoamingProtocol[20];                /**<	protocol to be used to access internet via cellular data(ipv4 ipv6) */
};

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
bool sim_set_ap(struct apn setapn);


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
  @return single strength in dbm
*/
unsigned int sim_get_signal_strength();


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
bool sim_send_sms(char* phone_no, char* message);


/*!
  @}
*/
#endif


