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

struct apn		
{
	char apn_name[20];			//	Acess point name
	char Proxy[20];				//	proxty url
	int  Port;				//	port number
	char username[20];			//	username
	char password[20];			//	password
	char mmsc[20];				//	mmsc url
	char mmsProxy[20];			//	proxy ip
	int  mmsPort[20];			//	port number
	int  mcc;				//	Mobile Country Code
	int  mnc;				//	Mobile Network Code
	char authentication[20];		//	type char security protocol : PAP or CHAP
	char apnType[20];			//	type of data which must use this APN configuration
	char apnProtocol[20];			//	protocol to be used to access internet (ipv4 ipv6)
	char apnRoamingProtocol[20];		//	protocol to be used to access internet via cellular data(ipv4 ipv6)
};

struct list_apn
{
	char name[10];
	
};

bool unlock_sim(int pincode);				/* activates the sim card by using sim pin */
bool dialup_connect(unsigned char phone_no[]);		/* tries to connect to telephone network */
bool dialup_disconnect();				/* disconnect from telephone network */
bool set_ap(struct apn setapn);				/* Creates or update an AP (identified by its name) */
bool remove_ap(char* ap_name);				/* remove an AP from the list (identified by its name) */
bool connect_to_internet();				/* tries to connect to internet */	
unsigned int get_signal_strength();			/* gets the strengh of the signal (in dbm) */
char* get_imsi();					/* gets the IMSI */
char* get_imei();					/* gets the IMEI */

#endif

