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

#ifndef HEX_COMMON_H
#define HEX_COMMON_H

#include <stdio.h>
#include <stdbool.h>
#include <glib.h>
#include <NetworkManager.h>
#include <string.h>


extern NMClient* getClient();
extern char interfaceName[255];
extern NMAccessPoint *findApOnDevice (NMDevice *device, GByteArray *bssid, const char *ssid);
extern bool runCommand( const char* cmd, char* output,int size);

int _list_iface_info(int skfd, char* ifname, char* args[], int count);

#endif
