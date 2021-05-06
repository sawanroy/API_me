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


#include <common.h>

char interfaceName[255] = "";

NMClient* getClient(){
	NMClient *client = NULL;
	GError *error = NULL;

	if (!(client = nm_client_new (NULL, &error))) {
		g_message ("Error: Could not connect to NetworkManager: %s.", error->message);
		g_error_free (error);
		return NULL;
	}

	if (!nm_client_get_nm_running (client)) {
		g_message ("Error: Can't obtain connections: NetworkManager is not running.");
		return NULL;
	}
	return client;
}
int getIfname()
{
	int skfd;
	if((skfd = iw_sockets_open())<0)
	{
		return -1;
	}
	iw_enum_devices(skfd, &_list_iface_info, NULL,0);
	if(strlen(interfaceName)==0){
		printf("cannot find wireless interface, forgot root?\n");
		return -1;
	}
	return 1; 
}
//-----------------------------------------------------------------------------------------------------
int get_info(int			skfd,
	 char *			ifname,
	 struct wireless_info *	info)
{
  struct iwreq	wrq;

  memset((char *) info, 0, sizeof(struct wireless_info));

  /* Get basic information */
  if(iw_get_basic_config(skfd, ifname, &(info->b)) < 0)
    {
      /* If no wireless name : no wireless extensions */
      /* But let's check if the interface exists at all */
      struct ifreq ifr;

      strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
      if(ioctl(skfd, SIOCGIFFLAGS, &ifr) < 0)
	return(-ENODEV);
      else
	return(-ENOTSUP);
    }

  /* Get ranges */
  if(iw_get_range_info(skfd, ifname, &(info->range)) >= 0)
    info->has_range = 1;

  /* Get AP address */
  if(iw_get_ext(skfd, ifname, SIOCGIWAP, &wrq) >= 0)
    {
      info->has_ap_addr = 1;
      memcpy(&(info->ap_addr), &(wrq.u.ap_addr), sizeof (sockaddr));
    }

  /* Get bit rate */
  if(iw_get_ext(skfd, ifname, SIOCGIWRATE, &wrq) >= 0)
    {
      info->has_bitrate = 1;
      memcpy(&(info->bitrate), &(wrq.u.bitrate), sizeof(iwparam));
    }

  /* Get Power Management settings */
  wrq.u.power.flags = 0;
  if(iw_get_ext(skfd, ifname, SIOCGIWPOWER, &wrq) >= 0)
    {
      info->has_power = 1;
      memcpy(&(info->power), &(wrq.u.power), sizeof(iwparam));
    }

  /* Get stats */
  if(iw_get_stats(skfd, ifname, &(info->stats),
		  &info->range, info->has_range) >= 0)
    {
      info->has_stats = 1;
    }

#ifndef WE_ESSENTIAL
  /* Get NickName */
  wrq.u.essid.pointer = (caddr_t) info->nickname;
  wrq.u.essid.length = IW_ESSID_MAX_SIZE + 1;
  wrq.u.essid.flags = 0;
  if(iw_get_ext(skfd, ifname, SIOCGIWNICKN, &wrq) >= 0)
    if(wrq.u.data.length > 1)
      info->has_nickname = 1;

  if((info->has_range) && (info->range.we_version_compiled > 9))
    {
      /* Get Transmit Power */
      if(iw_get_ext(skfd, ifname, SIOCGIWTXPOW, &wrq) >= 0)
	{
	  info->has_txpower = 1;
	  memcpy(&(info->txpower), &(wrq.u.txpower), sizeof(iwparam));
	}
    }

  /* Get sensitivity */
  if(iw_get_ext(skfd, ifname, SIOCGIWSENS, &wrq) >= 0)
    {
      info->has_sens = 1;
      memcpy(&(info->sens), &(wrq.u.sens), sizeof(iwparam));
    }

  if((info->has_range) && (info->range.we_version_compiled > 10))
    {
      /* Get retry limit/lifetime */
      if(iw_get_ext(skfd, ifname, SIOCGIWRETRY, &wrq) >= 0)
	{
	  info->has_retry = 1;
	  memcpy(&(info->retry), &(wrq.u.retry), sizeof(iwparam));
	}
    }

  /* Get RTS threshold */
  if(iw_get_ext(skfd, ifname, SIOCGIWRTS, &wrq) >= 0)
    {
      info->has_rts = 1;
      memcpy(&(info->rts), &(wrq.u.rts), sizeof(iwparam));
    }

  /* Get fragmentation threshold */
  if(iw_get_ext(skfd, ifname, SIOCGIWFRAG, &wrq) >= 0)
    {
      info->has_frag = 1;
      memcpy(&(info->frag), &(wrq.u.frag), sizeof(iwparam));
    }
#endif	/* WE_ESSENTIAL */

  return(0);
}
//-----------------------------------------------------------------------------------------------------
int _list_iface_info(int skfd, char* ifname, char* args[], int count){
	wireless_info	info;
	int			rc;

	/* Avoid "Unused parameter" warning */
	args = args; count = count;

	rc = get_info(skfd, ifname, &info);
	switch(rc)
	{
		case 0:	/* Success */
		/* Display it ! */
			strcpy(interfaceName, ifname);
		break;

		default:
			//ignore
			break;
	}
	return(rc);
}
//-----------------------------------------------------------------------------------------------------
NMAccessPoint *findApOnDevice (NMDevice *device, GByteArray *bssid, const char *ssid)
{
	const GPtrArray *aps;
	NMAccessPoint *ap = NULL;
	int i;
	g_return_val_if_fail (NM_IS_DEVICE_WIFI (device), NULL);
	g_return_val_if_fail ((bssid && !ssid) || (!bssid && ssid), NULL);
	// nm_device_wifi_request_scan(NM_DEVICE_WIFI(device),NULL,NULL);
    char ret[1024];
    if(!runCommand("wpa_cli scan",ret,1024)){
        printf("scan failed\n");
        return (NMAccessPoint *)-1;
    }
	aps = nm_device_wifi_get_access_points (NM_DEVICE_WIFI (device));
	printf("Found %d APs\n", aps->len);
	for (i = 0; i < aps->len; i++) {
		NMAccessPoint *candidate_ap = g_ptr_array_index (aps, i);
		if (ssid) {
			/* Parameter is SSID */
			GBytes *candidate_ssid;
			candidate_ssid = nm_access_point_get_ssid (candidate_ap);
			if (candidate_ssid) {
				char *ssid_tmp = nm_utils_ssid_to_utf8 (g_bytes_get_data (candidate_ssid, NULL),
				                                        g_bytes_get_size (candidate_ssid));
				/* Compare SSIDs */
				if (strcmp (ssid, ssid_tmp) == 0) {
					ap = candidate_ap;
					g_free (ssid_tmp);
					break;
				}
				g_free (ssid_tmp);
			}
		} else if (bssid) {
			/* Parameter is BSSID */
			const char *candidate_bssid = nm_access_point_get_bssid (candidate_ap);
			char *bssid_up = nm_utils_hwaddr_ntoa (bssid->data, bssid->len);
			/* Compare BSSIDs */
			if (strcmp (bssid_up, candidate_bssid) == 0) {
				ap = candidate_ap;
				g_free (bssid_up);
				break;
			}
			g_free (bssid_up);
		}
	}
	return ap;
}
//-----------------------------------------------------------------------------------------------------
bool runCommand( const char* cmd, char* output,int size){
		memset(output,'\0', size);
	int n;
 	char path[1035];
	FILE* fp = popen(cmd,"r");
	if (fp == NULL) {
    		exit(-1);
  		}

     		/* Read the output a line at a time - output it. */
  		while (fgets(path, sizeof(path)-1, fp) != NULL) {
			  printf("%s",path);
			  strcpy(output,path);
		  }
		  
	return true;
}
//-----------------------------------------------------------------------------------------------------
//this calculation vary depending on hardware
int dbmToQuality(int dBm){
	int quality;
	if(dBm<= -100)
		quality = 0;
	else if(dBm>= -50)
		quality = 100;
	else
		quality = 2*(dBm + 100);

	return quality;
}
