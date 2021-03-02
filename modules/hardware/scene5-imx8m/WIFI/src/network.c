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

#include <network.h>


static void __attribute__ ((constructor)) \
lib_init(void);

static void lib_init(void) {
  /* check root permission */
  if(geteuid() != 0)
  {
    return;
    /* Tell user to run  app as root, then exit */ 
  } 
    char ret[1024];
    if(!runCommand("wpa_cli scan",ret,1024)){
        printf("scan failed\n");
        return 10600;
    }
    /* leave some time for the scan to finish */
    usleep(1000000);
    return;
}
/*
    wifi_activation(bool state)
*/
bool wifi_activation(bool state){
    NMClient* client = getClient();
    if(!client)
    return false;

    if(getIfname()<1){
        return false;
    }
    char cmd[255];
    char ret[1024];
    if(state){
        sprintf(cmd, "ifconfig %s up", interfaceName);
        if(!runCommand(cmd,ret,1024)){
        return false;
        }
    }
    else{
         sprintf(cmd, "ifconfig %s down", interfaceName);
        if(!runCommand(cmd,ret,1024)){
        return false;
        }
    }
    return true;
}

/*

    wifi_get_power_status()

*/

bool wifi_get_power_status(){
    NMClient* client = getClient();
    if(!client)
    return false;

    bool enabled = nm_client_wireless_get_enabled(client);
    return enabled;
}
/*

    wifi_set_defaultpower_mode(bool powermode)

*/
bool wifi_set_defaultpower_mode(bool powermode){
    NMClient* client = getClient();
    if(!client)
    return false;

    nm_client_wireless_set_enabled (client, powermode);
    return true;
}
/*

    wifi_get_defaultpower_mode(){

*/
bool wifi_get_defaultpower_mode(){
    NMClient* client = getClient();
    if(!client)
    return true;

    bool enabled = nm_client_wireless_get_enabled(client);
    return enabled;
}
/*

    wifi_connect_network(struct wifiinfo credentials)

*/
int wifi_connect_network(struct wifiinfo credentials){
    wifi_disconnect_network();
    NMClient* client = getClient();
    if(!client)
    return 10600;

    if(getIfname()<1){
        return 10600;
    }
    char ret[1024]="";

    if(!runCommand("wpa_cli scan",ret,1024)){
        printf("scan failed\n");
        return 10600;
    }
    char cmd[255];
    sprintf(cmd, "wpa_cli scan_results | awk -F \" \" ' {print $5 }' - | grep -E '^%s$'", credentials.ssid);
    if(!runCommand(cmd,ret,1024)){
        printf("scan result failed\n");
        return 10600;
    }
    if(strlen(ret)==0){
        printf("network not in range");
        return 10605;
    }
    if(!runCommand("wpa_cli add_network | tail -1",ret,1024)){
        printf("adding network failed\n");
        return 10600;
    }
    int networkId = atoi(ret);
    sprintf(cmd, "wpa_cli set_network %d ssid '\"%s\"'",networkId,credentials.ssid);
    // printf(cmd);
    if(!runCommand(cmd,ret,1024)){
        printf("set ssid failed\n");
        return 10600;
    }
    sprintf(cmd, "wpa_cli set_network %d psk '\"%s\"'",networkId,credentials.password);
    // printf(cmd);
    if(!runCommand(cmd,ret,1024)){
        printf("set password failed\n");
        return 10600;
    }
    sprintf(cmd, "sh -c 'wpa_cli enable_network %d && wpa_cli select_network %d' | grep 'FAIL'", networkId,networkId);
    //printf(cmd);
    if(!runCommand(cmd,ret,1024)){
        printf("enabling network failed, check password\n");
        return 10602;
    }
    printf(ret);
    if(strlen(ret)){
        printf("enabling network failed, check password\n");
        return 10602;
    }
    return 0;
}
/*

    wifi_disconnect_network()

*/
bool wifi_disconnect_network(){
    NMClient* client = getClient();
    if(!client)
    return false;

    if(getIfname()<0){
        return false;
    }
    NMDevice *device = nm_client_get_device_by_iface (client,interfaceName);
    NMDeviceState state = nm_device_get_state(device);
    if(state != NM_DEVICE_STATE_ACTIVATED)
    {
        return false;
    }
    if(!nm_device_disconnect(device, NULL, NULL)){
        return false;
    }
    return true;
}
/*

    wifi_add_to_ssid_preferred_list(struct wifiinfo credentials)

*/
bool wifi_add_to_ssid_preferred_list(struct wifiinfo credentials)
{
    bool wep_passphrase = true; //key:false, phrase:true

    NMClient* client = getClient();
    if(!client)
    return false;
    NMConnection *connection=nm_simple_connection_new();
    NMSettingWireless* s_wifi = nm_setting_wireless_new();
    if(!connection )
    return false;
    //check for access point
    if(getIfname()<0){
        return false;
    }
    NMDevice *device = nm_client_get_device_by_iface (client,interfaceName);

    NMAccessPoint* ap = findApOnDevice(device,NULL, credentials.ssid);
    if(ap==NULL){
        printf("Cannot find the AP nearby");
        return false;
    }
    // const char* bssid = nm_access_point_get_bssid(ap);
    // printf("bssid:%s\n",bssid);
    // return;
    /*Connection Setting*/
    NMSettingConnection *s_con = nm_setting_connection_new();
    if(!s_con)
    {
        printf("cannot create new connection\n");
        return false;
    }
    nm_connection_add_setting(connection, NM_SETTING(s_con));
    char* uuid = nm_utils_uuid_generate();
    g_object_set (s_con,
        NM_SETTING_CONNECTION_ID, credentials.ssid,
        NM_SETTING_CONNECTION_UUID, uuid,
        NM_SETTING_CONNECTION_AUTOCONNECT, TRUE,
        NM_SETTING_CONNECTION_TYPE, NM_SETTING_WIRELESS_SETTING_NAME,
        NULL
    );
    g_free (uuid);
    /* Wifi setting*/
    NM80211ApFlags ap_flags             = nm_access_point_get_flags(ap);
    NM80211ApSecurityFlags ap_wpa_flags = nm_access_point_get_wpa_flags(ap);
    NM80211ApSecurityFlags ap_rsn_flags = nm_access_point_get_rsn_flags(ap);
    /* Set password for WEP or WPA-PSK. */
    if (ap_flags & NM_802_11_AP_FLAGS_PRIVACY) {
        /* Ask for missing password when one is expected and '--ask' is used */
        NMSettingWirelessSecurity *s_wsec = (NMSettingWirelessSecurity *) nm_setting_wireless_security_new ();
        nm_connection_add_setting (connection, NM_SETTING (s_wsec));
        if (ap_wpa_flags == NM_802_11_AP_SEC_NONE && ap_rsn_flags == NM_802_11_AP_SEC_NONE) {
            /* WEP */
            nm_setting_wireless_security_set_wep_key (s_wsec, 0, credentials.password);
            g_object_set (G_OBJECT (s_wsec),
            NM_SETTING_WIRELESS_SECURITY_WEP_KEY_TYPE,
            wep_passphrase ? NM_WEP_KEY_TYPE_PASSPHRASE: NM_WEP_KEY_TYPE_KEY,
            NULL);
        } else if (   !(ap_wpa_flags & NM_802_11_AP_SEC_KEY_MGMT_802_1X)
        && !(ap_rsn_flags & NM_802_11_AP_SEC_KEY_MGMT_802_1X)) {
            /* WPA PSK */
            g_object_set (s_wsec, NM_SETTING_WIRELESS_SECURITY_PSK, credentials.password, NULL);
        }
    }
    //we don't have callback, so assuming it will be successful with this async operation
    //network manager only provides async operation for adding connection
    nm_client_add_and_activate_connection_async(client,connection, device, nm_object_get_path(NM_OBJECT(ap)),NULL, NULL,NULL);
    return true;
}
/*

    wifi_getsignal_strength(char* SSID)


*/
int wifi_getsignal_strength(char* SSID){
    NMClient* client = getClient();
    int strength;
    if(!client)
    return 10600;

    if(getIfname()<1){
        return 10600;
    }
    char ret[1024]="";

    if(!runCommand("wpa_cli scan",ret,1024)){
        printf("scan failed\n");
        return 10600;
    }
    char cmd[255];
    sprintf(cmd, "wpa_cli scan_results | awk -F \" \" ' {print $3 $5 }' - | grep -E '[-0-9]+%s$'", SSID);
    if(!runCommand(cmd,ret,1024)){
        printf("scan result failed\n");
        return 10600;
    }
    if(strlen(ret)==0){
        printf("network not in range");
        return 10600;
    }
    int dBm;
    sscanf(ret,"%3d%s", &dBm,ret);
    strength = dbmToQuality(dBm);
    return strength;
}
/*

    wifi_scan(vector* v)

*/
int wifi_scan(vector* v)
{
    NMClient* client = getClient();
    if(!client)
    return 10600;

    if(getIfname()<1){
        return 10600;
    }
    char ret[2048]="";

    if(!runCommand("wpa_cli scan",ret,1024)){
        printf("scan failed\n");
        return 10600;
    }
    char cmd[255];
//    sprintf(cmd, "wpa_cli scan_results | awk -F \" \" 'NR>2 {print $3,$5 }' - ");
    sprintf(cmd, "wpa_cli scan_results | awk 'NR>2 {print $3,$5 }' - ");
    if(!runCommand(cmd,ret,2048)){
        printf("scan result failed\n");
        return 10600;
    }
    sleep(0.3);
    if(strlen(ret)==0){
        printf("network not in range");
        return 10600;
    }
    int count =0;
    int dbm;
    char ssid[128]="";
    char* p = strtok(ret,"\n");
    while(p!=NULL){
        sscanf(p,"%3d %s\n", &dbm,ssid);
        WiFi_scanResult *res = malloc(sizeof(WiFi_scanResult));
        res->strength = dbmToQuality(dbm);
        res->ssid = strdup(ssid);
        vector_add(v, res);
        count++;
        p = strtok(NULL,"\n");
    }

    return 0;
}
/*
    wifi_get_ssid_preferred_list(vector* SSIDs)

*/
int wifi_get_ssid_preferred_list(vector* SSIDs)
{
    NMClient* client = getClient();
    if(!client)
    return 10600;
	const GPtrArray *connections = nm_client_get_connections (client);
    unsigned int i;
    for(i =0; i < connections->len;i++){
		NMConnection *connection = connections->pdata[i];
		NMSettingConnection *s_con;

		s_con = nm_connection_get_setting_connection (connection);
		if (s_con) {
			const char* id = nm_setting_connection_get_id (s_con);
            vector_add(SSIDs, id);
		}
        else{
            return 10600;
        }
    }
    return 0;
}
/*
    wifi_remove_from_ssid_preferred_list(char* SSID)


*/
bool wifi_remove_from_ssid_preferred_list(char* SSID){

    NMClient* client = getClient();
    if(!client)
    return false;

    NMRemoteConnection* connection = nm_client_get_connection_by_id(client, SSID);
    if(!connection)
    {
        printf("connection does not exist\n");
        return false;
    }

    char ret[1024];
    char cmd[125];
    sprintf(cmd,"nmcli connection delete id \"%s\"", SSID);
    if(!runCommand(cmd,ret,1024)){
        return false;
    }
    return true;
}
/*
    wifi_clean_ssid_preferred_list


*/
bool wifi_clean_ssid_preferred_list()
{
    vector v;
	vector_init(&v);
	wifi_get_ssid_preferred_list(&v);
    int i;
	for(i=0;i<vector_count(&v);i++){
		if(wifi_remove_from_ssid_preferred_list(vector_get(&v,i)))
        {
            return false;
        }
	}
	vector_free(&v);
    return true;
}
/*

wifi_reconnect()

*/
bool wifi_reconnect(){
    if(getIfname()<0){
        return false;
    }
    char ret[1024]="";
    if(!runCommand("wpa_cli reconnect | grep -o 'OK'", ret, 1024)){
        return false;
    }
    if(strlen(ret)==0){
        return false;
    }
    return true;
}
/*

wifi_set_static_ip_Address(char* ipaddress, char* subnetmask, char* gateway, char* dns)

*/
bool wifi_set_static_ip_Address(char* ipaddress, char* subnetmask, char* gateway, char* dns){
    NMClient* client = getClient();
    if(!client)
    return false;

    if(getIfname()<1){
        return false;
    }
    char ret[1024]="";
    char cmd[255];
    if(strlen(ipaddress)>0 && strlen(subnetmask)>0){
        sprintf(cmd, "ifconfig %s %s netmask %s up", interfaceName, ipaddress, subnetmask);
        if(!runCommand(cmd,ret,1024)){
            printf("setting IP failed\n");
            return false;
        }
    }
    if(strlen(gateway)>0){
        sprintf(cmd, "route add default gw %s", gateway);
        if(!runCommand(cmd,ret,1024)){
            printf("setting gateway failed");
            return false;
        }
    }
    if(strlen(dns)>0){
        sprintf(cmd, "echo \"nameserver %s\" >> /etc/resolv.conf", dns);
        if(!runCommand(cmd,ret,1024)){
            printf("dns setting failed");
            return false;
        }
    }
    return true;
}
/*

    WiFi_useDHCP()

*/
int WiFi_useDHCP(){
    if(getIfname()<0){
        return 10600;
    }
    char ret[1024]="";
    char cmd[255];
    sprintf(cmd,"dhclient %s",interfaceName);
    if(!runCommand(cmd, ret, 1024)){
        return 10600;
    }
    return 0;
}
/*
 *   Select mode Client/Access Point
     wifi_mode(bool mode)
*/
bool wifi_mode(bool mode){
    char cmd[255];
    char ret[1024]="";
    char ret2[1024]="";
    sprintf(cmd, "nmcli dev | grep wifi | awk '{print $1}'");
    if(!runCommand(cmd,ret,1024)) {
        printf("fail to change the mode to Access point\n");
        return false;
    }  
    if(mode){         
        char cmd2[25];
        sprintf(cmd, "nmcli dev wifi hotspot ifname %s ssid Trunexa password '123459090'",ret);
        if(!runCommand(cmd,ret2,1024)) {
            printf("fail to change the mode to Access point\n");
            return false;
        }           
        return true;
    }
    else{
        char cmd[255];
        char ret3[1024]="";
        sprintf(cmd, "nmcli device disconnect %s",ret);
        if(!runCommand(cmd,ret3,1024)) {
            printf("fail to change the mode to client\n");
            return false;
        }   
        return true;        
    }
}

bool wifi_set_ssid_lock(char *ssid,bool enable ){
    if(enable){
        return true;
    }
    else{
        return false;
    }
}
