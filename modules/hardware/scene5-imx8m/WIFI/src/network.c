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


/*
    Internal function
*/
bool wifi_ssid_lock_check()
{
    char cmd[1024];
    char ret[1024]="";
    vector a;

    vector_init(&a);
    wifi_get_ssid_preferred_list(&a);
    for(int i=0; i< a.count; i++) 
    {
        wifi_info *con = (wifi_info *)a.data[i];
        sprintf(cmd, "nmcli con show '%s' | grep connection.autoconnect-priority: | awk '{print $2}' ", con->ssid);
        if(!runCommand(cmd,ret,1024)) 
        {
            free(con->ssid);
            free(con->password);
            free(con);
            vector_free(&a);
            return false;
        }
        if(atoi(ret)==2)
        {
            //lock is set
            free(con->ssid);
            free(con->password);
            free(con);
            vector_free(&a);
            return true;
        }
        free(con->ssid);
        free(con->password);
        free(con);
    }
    
    vector_free(&a);
    
    return false;
}



/*
    Internal function
*/
bool wifi_ssid_available(char *ssid)
{
    char cmd[1024];
    char ret[1024]="";

    if(!runCommand("wpa_cli scan",ret,1024))
    {
        return false;
    }

    sprintf(cmd, "wpa_cli scan_results | awk -F \" \" ' {print $5 }' - | grep -E '^%s$'", ssid);
    if(!runCommand(cmd,ret,1024))
    {
        return false;
    }
    if(strlen(ret)==0)
    {
        dbg_log(("SSID not available\n"));
        return false;
    }

    return true;
}



/*
    Internal function
*/
bool wifi_client_mode()
{
    char cmd[1024];
    char ret[1024]="";

    sprintf(cmd, "nmcli con show | grep HOTSPOT");
    if(!runCommand(cmd,ret,1024))
    {
        return false;
    }
    if(strlen(ret)==0)
    {
        dbg_log(("Client mode\n"));
        return true;
    }
    dbg_log(("AP mode\n"));

    return false;
}



/*
    wifi_activation(bool state)
*/
bool wifi_activation(bool state)
{
    NMClient* client = getClient();
    if(!client)
    {
        return false;
    }
    nm_client_wireless_set_enabled(client, state);

    return true;
}



/*
    wifi_get_power_status()
*/
bool wifi_get_power_status()
{
    NMClient* client = getClient();
    if(!client)
    {
        return true;
    }
    bool enabled = nm_client_wireless_get_enabled(client);
    
    return enabled;
}



/*
    wifi_set_defaultpower_mode(bool powermode)
*/
bool wifi_set_defaultpower_mode(bool powermode)
{
    NMClient* client = getClient();
    if(!client)
    {
        return false;
    }

    char cmd[1024];
    char ret[1024]="";

    if(powermode)
    {
        sprintf(cmd,"sed -i 's/nmcli r wifi off/nmcli r wifi on/g' /etc/rc.local");
        if(!runCommand(cmd,ret,255))
        {
            return false;
        }
    }
    else
    {
        sprintf(cmd,"sed -i 's/nmcli r wifi on/nmcli r wifi off/g' /etc/rc.local");
        if(!runCommand(cmd,ret,255))
        {
            dbg_log(("command failed\n"));
            return false;
        }
    }

    return true;
}



/*
    wifi_get_defaultpower_mode(){
*/
bool wifi_get_defaultpower_mode()
{
    NMClient* client = getClient();
    if(!client)
    {
        return true;
    }

    char cmd[1024];
    char ret[1024]="";

    sprintf(cmd,"sed -n '/nmcli r wifi on/{p;q;}' /etc/rc.local");
    if(!runCommand(cmd,ret,255))
    {
        return false;
    }
    if(strlen(ret)==0)
    {
        return false;
    }

    return true;
}



/*
    wifi_connect_network(wifi_info credentials)
*/
int wifi_connect_network(wifi_info credentials)
{ 
    NMClient* client = getClient();
    if(!client)
    {
        return FAILURE;
    }
    if(getIfname()<1)
    {
        return FAILURE;
    }

    char ret[1024]="";
    char cmd[1024];

    if(!wifi_get_power_status())
    {
        dbg_log(("wifi not active\n"));
        return WIFI_INACTIVE;
    }

    if(!wifi_client_mode())
    {
        return WIFI_CLIENT_DISABLED;
    }

    if(!wifi_ssid_available(credentials.ssid))
    {
        return WIFI_SSID_UNAVAILABLE;
    }

    if(wifi_ssid_lock_check())
    {
        dbg_log(("SSID lock set. Disable lock first"));
        return WIFI_LOCK_IS_ON;
    }

    NMDevice *device = nm_client_get_device_by_iface (client,interfaceName);
    NMDeviceState state = nm_device_get_state(device);
    if(state==NM_DEVICE_STATE_ACTIVATED) 
    {
        dbg_log(("Disconnect first and then connect\n"));
        return WIFI_ALREADY_CONNECTED;
    }

    NMRemoteConnection* connection = nm_client_get_connection_by_id(client, credentials.ssid);
    if(!connection)
    {
        sprintf(cmd, "nmcli con add ifname '%s' type wifi con-name '%s' autoconnect off ssid '%s' wifi-sec.key-mgmt wpa-psk wifi-sec.psk '%s'",interfaceName,credentials.ssid,credentials.ssid,credentials.password);
        if(!runCommand(cmd,ret,1024))
        {
            return FAILURE;
        }
    }

    sprintf(cmd, "nmcli con up '%s'",credentials.ssid);
    if(!runCommand(cmd,ret,1024))
    {
        return FAILURE;
    }

    return WIFI_CONNECTED;
}



/*
    wifi_disconnect_network()
*/
bool wifi_disconnect_network()
{
    NMClient* client = getClient();
    if(!client)
    {
        return false;
    }
    if(getIfname()<0)
    {
        return false;
    }

    if(!wifi_get_power_status())
    {
        dbg_log(("wifi not active\n"));
        return false;
    }

    if(!wifi_client_mode())
    {
        return false;
    }

    NMDevice *device = nm_client_get_device_by_iface (client,interfaceName);
    NMDeviceState state = nm_device_get_state(device);
    if(state!=NM_DEVICE_STATE_ACTIVATED) 
    {
        dbg_log(("Already disconnected\n"));
        return false;
    }

    if(wifi_ssid_lock_check())
    {
        dbg_log(("SSID is locked. Disable lock first\n"));
        return false;
    }

    if(!nm_device_disconnect(device, NULL, NULL))
    {
        return false;
    }
     
    dbg_log(("connection disconnected\n"));  
    return true;
}



/*
    wifi_add_to_ssid_preferred_list(wifi_info credentials)
*/
bool wifi_add_to_ssid_preferred_list(wifi_info credentials) 
{   
    NMClient* client = getClient();
    if(!client)
    {
        return false;
    }
    if(getIfname()<0)
    {
        return false;
    }

    char cmd[526];
    char ret[1024];

    if(!wifi_get_power_status())
    {
        dbg_log(("wifi not active\n"));
        return false;
    }

    if(!wifi_client_mode())
    {
        return false;
    }

    //Check if given ssid exist in connection list
    NMRemoteConnection* connection = nm_client_get_connection_by_id(client, credentials.ssid);
    if(!connection)
    {
        dbg_log(("connection does not exist\n"));
        if(wifi_ssid_lock_check())
        {
            sprintf(cmd, "nmcli con add ifname '%s' type wifi con-name '%s' connection.autoconnect no connection.autoconnect-priority 1 ssid '%s' wifi-sec.key-mgmt wpa-psk wifi-sec.psk '%s'",interfaceName,credentials.ssid,credentials.ssid,credentials.password);
            if(!runCommand(cmd,ret,1024))
            {
                return false;
            }
        }
        else
        {
            sprintf(cmd, "nmcli con add ifname '%s' type wifi con-name '%s' connection.autoconnect-priority 1 ssid '%s' wifi-sec.key-mgmt wpa-psk wifi-sec.psk '%s'",interfaceName,credentials.ssid,credentials.ssid,credentials.password);
            if(!runCommand(cmd,ret,1024))
            {
                return false;
            }
        }
    }
    else
    {
        sprintf(cmd, "nmcli con show '%s' | grep connection.autoconnect-priority | awk '{print $2}'",credentials.ssid);
        if(!runCommand(cmd,ret,1024))
        {
            dbg_log(("Command failure\n"));
            return false;
        }
        if(atoi(ret)>=1)
        {
            //ssid already present in preffered list
            dbg_log(("SSID already added to preffered list\n"));
            return false;
        }
        else
        {
            //ssid is in connection list but not in preffered list.
            if(wifi_ssid_lock_check())
            {
                sprintf(cmd, "nmcli con modify '%s' connection.autoconnect off connection.autoconnect-priority 1",credentials.ssid);
                if(!runCommand(cmd,ret,1024))
                {
                    return false;
                }
            }
            else
            {
                sprintf(cmd, "nmcli con modify '%s' connection.autoconnect on connection.autoconnect-priority 1",credentials.ssid);
                if(!runCommand(cmd,ret,1024))
                {
                    return false;
                }
            }
            dbg_log(("Connection added to preffered list\n"));   
        }
    } 

    return true;
}



/*
    wifi_get_signal_strength(char* ssid)
*/
int wifi_get_signal_strength(char* ssid)
{
    NMClient* client = getClient();
    if(!client)
    {
        return false;
    }
    if(getIfname()<1)
    {
        return false;
    }

    char ret[1024]="";
    char cmd[1024];
    int strength;

    if(!wifi_get_power_status())
    {
        dbg_log(("wifi not active\n"));
        return false;
    }

    if(!wifi_client_mode())
    {
        return false;
    }

    if(!runCommand("wpa_cli scan",ret,1024))
    {
        return false;
    }

    sprintf(cmd, "nmcli -g SSID,SIGNAL d wifi | grep -w '%s' | awk -F \":\" '{print $2}'", ssid);
    if(!runCommand(cmd,ret,1024))
    {
        return false;
    }
    if(strlen(ret)==0)
    {
        dbg_log(("network not in range\n"));
        return -1;
    }
    
    strength = atoi(ret);
    return strength;
}



/*
    wifi_scan(vector* v)
*/
bool wifi_scan(vector* v)
{
    NMClient* client = getClient();
    if(!client)
    {
        return false;
    }
    if(getIfname()<1)
    {
        return false;
    }

    char ret[10]="";
    char security[50];
    int count =0;
    int strength;
    int chan;
    gsize size;
    NM80211ApFlags wpa1;
    NM80211ApFlags wpa2;

    if(!wifi_get_power_status())
    {
        dbg_log(("wifi not active\n"));
        return false;
    }
    
    if(!wifi_client_mode())
    {
        return false;
    }

    NMDevice *device = nm_client_get_device_by_iface (client, interfaceName);
    NMDeviceType type = nm_device_get_device_type(device);
    if(type!=NM_DEVICE_TYPE_WIFI)
    {
        printf("type wifi\n");
        return false;
    } 
   
    //request scan
    if(!runCommand("wpa_cli scan",ret,10))
    {
        return false;
    }
    sleep(2);
    const GPtrArray *connections = nm_device_wifi_get_access_points ((NMDeviceWifi *)device);
    for(guint i =0; i < connections->len;i++)
    {
        //SSID
        GBytes *bytes = nm_access_point_get_ssid(connections->pdata[i]);

        if (!bytes)
        {   
            // don't crash if ssid is null
            continue;
        }

        const char *ssid = g_bytes_get_data(bytes,&size);
            
        //Signal strength
        strength = nm_access_point_get_strength(connections->pdata[i]);
        
        //Channel        
        chan = nm_utils_wifi_freq_to_channel(nm_access_point_get_frequency (connections->pdata[i]));
        
        //Security
        wpa1 = nm_access_point_get_wpa_flags(connections->pdata[i]);
        wpa2 = nm_access_point_get_rsn_flags(connections->pdata[i]);
        
        if(wpa1 && !wpa2)
        {
            sprintf(security,"WPA1");
        }
        else if(wpa2 && !wpa1)
        {
            sprintf(security,"WPA2");
        }
        else
        {
            sprintf(security,"WPA1 WPA2");
        }

        dbg_log(("SSID:%s          ",ssid));
        dbg_log(("SIG:%d  ",strength));
        dbg_log(("CHAN: %d  ",chan));
        dbg_log(("SEC: %s\n", security));
        
        wifi_scanresult *res = malloc(sizeof(wifi_scanresult));
        res->strength = strength;
        res->ssid = strdup(ssid);
        res->channel = chan;
        res->security = strdup(security);
        vector_add(v, res);
        count++;        
    }

    return true;
}



/*
    wifi_get_ssid_preferred_list(vector* con_list)
*/
bool wifi_get_ssid_preferred_list(vector* con_list)
{   
    
    NMClient* client = getClient();
    if(!client)
    {
        return false;
    }
    if(getIfname()<1)
    {
        return false;
    }

    char cmd[1024];
    char ret[1024];
    int comp;
    unsigned int i;

    if(!wifi_get_power_status())
    {
        dbg_log(("wifi not active\n"));
        return false;
    }

    if(!wifi_client_mode())
    {
        return false;
    }

    const GPtrArray *connections = nm_client_get_connections(client);
    for(i=0; i < connections->len; i++)
    {
        NMConnection *connection = connections->pdata[i];
        NMSettingConnection *s_con;
        s_con = nm_connection_get_setting_connection(connection);
        if(s_con)
        {
            const char* id = nm_setting_connection_get_id(s_con);
            sprintf(cmd, "nmcli -f NAME,TYPE con show | grep '%s' | awk '{print $NF}'",id);
            if(!runCommand(cmd,ret,1024))
            {
                return false;
            }
            comp = strcmp(ret,"wifi");
            if(comp>=1) 
            {
                sprintf(cmd, "nmcli con show '%s' | grep connection.autoconnect-priority | awk '{print $2}'",id);
                if(!runCommand(cmd,ret,1024))
                {
                    return false;
                }   
                if(atoi(ret)>=1)
                {
                    sprintf(cmd, "nmcli -s -g 802-11-wireless-security.psk connection show '%s'",id);
                    if(!runCommand(cmd,ret,1024))
                    {
                        return false;
                    }

                    wifi_info *con = (wifi_info *)malloc(sizeof(wifi_info));
                    con->ssid = strdup(id);
                    con->password = strdup(ret);
                    vector_add(con_list, con); 
                }
            }
        }
        else
        {
            return false;
        }
    }

    return true;
}



/*
    wifi_remove_from_ssid_preferred_list(char* ssid)
*/
int wifi_remove_from_ssid_preferred_list(char* ssid)
{
    
    NMClient* client = getClient();
    if(!client)
    {
        return false;
    }
    if(getIfname()<1)
    {
        return false;
    }

    char ret[1024];
    char cmd[1024];

    if(!wifi_get_power_status())
    {
        dbg_log(("wifi not active\n"));
        return false;
    }

    if(!wifi_client_mode())
    {
        return false;
    }

    NMRemoteConnection* connection = nm_client_get_connection_by_id(client, ssid);
    if(!connection)
    {
        dbg_log(("connection does not exist\n"));
        return false;
    }
   
    sprintf(cmd, "nmcli con show '%s' | grep connection.autoconnect-priority | awk '{print $2}'",ssid);
    if(!runCommand(cmd,ret,1024))
    {
        return false;
    } 
    if(atoi(ret)==2)
    {
        //SSID is locked. Cannot remove
        dbg_log(("SSID is locked. Disbale lock first\n"));
        return WIFI_LOCK_IS_ON;
    }
    else if(atoi(ret)==1)
    {
        sprintf(cmd, "nmcli con modify '%s' connection.autoconnect off connection.autoconnect-priority 0", ssid);
        if(!runCommand(cmd,ret,1024))
        {
            return false;
        }
        dbg_log(("connection modified\n"));  
    }
    else
    {
        dbg_log(("connection not found in preferred list\n"));
    }

    return true;
}



/*
    wifi_clean_ssid_preferred_list()
*/
bool wifi_clean_ssid_preferred_list()
{
    int i;
    vector v;

    if(!wifi_get_power_status())
    {
        dbg_log(("wifi not active\n"));
        return false;
    }

    if(!wifi_client_mode())
    {
        return false;
    }

    vector_init(&v);
    wifi_get_ssid_preferred_list(&v);
    for(i=0; i<vector_count(&v); i++)
    {
        wifi_info *con = (wifi_info *)v.data[i];
        if(!wifi_remove_from_ssid_preferred_list(con->ssid))
        {
            free(con->ssid);
            free(con->password);
            free(con);
            vector_free(&v);
            return false;
        }
        free(con->ssid);
        free(con->password);
        free(con);
    }
    
    vector_free(&v);

    return true;
}



/*
    wifi_reconnect()
*/
bool wifi_reconnect()
{
    NMClient* client = getClient();
    if(!client)
    {
        return false;
    }
    if(getIfname()<0)
    {
        return false;
    }

    char ret[1024]="";
    char cmd[1024];

    if(!wifi_get_power_status())
    {
        dbg_log(("wifi not active\n"));
        return false;
    }

    if(!wifi_client_mode())
    {
        return false;
    }

    sprintf(cmd, "nmcli device connect %s",interfaceName);
    if(!runCommand(cmd,ret,1024))
    {
        return false;
    }
    if(strlen(ret)==0)
    {
        return false;
    }

    return true;
}



/*
    wifi_set_static(char* ipaddress, char* subnetmask, char* gateway, char* dns)
*/
bool wifi_set_static(char* ipaddress, char* prefixnetmask, char* gateway, char* dns)
{
    NMClient* client = getClient();
    if(!client)
    {
        return false;
    }
    if(getIfname()<1)
    {
        return false;
    }

    char cmd[1024];
    char ret[1024]=""; 

    if(!wifi_get_power_status())
    {
        dbg_log(("wifi not active\n"));
        return false;
    }

    if(!wifi_client_mode())
    {
        return false;
    }

    sprintf(cmd, "nmcli -t -f name,device connection show --active | grep '%s'", interfaceName);
    if(!runCommand(cmd,ret,1024))
    {
        return false;
    }

    char *tmp_ssid = strtok(ret,":");

    if(tmp_ssid==NULL)
    {
        dbg_log(("No network connected\n"));
        return false;
    }

    char ssid[strlen(tmp_ssid)];

    strcpy(ssid,tmp_ssid);

    sprintf(cmd,"nmcli con down '%s'", ssid);
    if(!runCommand(cmd,ret,1024))
    {
        return false;
    }

    sprintf(cmd,"nmcli con modify '%s' ipv4.addresses '%s'/'%s' ipv4.gateway '%s' ipv4.dns '%s' ipv4.method manual", ssid, ipaddress, prefixnetmask, gateway, dns);
    if(!runCommand(cmd,ret,1024))
    {
        return false;
    }
   
    sprintf(cmd,"nmcli con up '%s'", ssid);
    if(!runCommand(cmd,ret,1024))
    {
        return false;
    }
    
    return true;
}



/*
    wifi_use_dhcp(bool enable)
*/
bool wifi_use_dhcp(bool enable)
{
    if(getIfname()<0)
    {
        return false;
    }

    char ret[1024]="";
    char cmd[1024];

    if(!wifi_get_power_status())
    {
        dbg_log(("wifi not active\n"));
        return false;
    }

    if(!wifi_client_mode())
    {
        return false;
    }

    if(enable)
    {
        sprintf(cmd, "nmcli -t -f name,device connection show --active | grep '%s'", interfaceName);
        if(!runCommand(cmd,ret,1024))
        {
            return false;
        }

        char *tmp_ssid = strtok(ret,":");
        if(tmp_ssid==NULL)
        {
            dbg_log(("No network connected\n"));
            return false;
        }

        char ssid[strlen(tmp_ssid)];

        strcpy(ssid,tmp_ssid);
        sprintf(cmd,"nmcli con down '%s'", ssid);
        if(!runCommand(cmd,ret,1024))
        {
            return false;
        }
        
        sprintf(cmd,"nmcli con modify '%s' ipv4.addresses \"\" ipv4.gateway \"\" ipv4.dns \"\" ipv4.method auto", ssid);
        if(!runCommand(cmd,ret,1024))
        {
            return false;
        }

        sprintf(cmd,"nmcli con up '%s'", ssid);
        if(!runCommand(cmd,ret,1024))
        {
            return false;
        }
    }
    else
    {
        sprintf(cmd,"dhclient '%s' && dhclient -r %s", interfaceName, interfaceName);
        if(!runCommand(cmd, ret, 1024))
        {
            return false;
        }
    }

    return true;
}



/*
    Select mode Client/Access Point
    wifi_mode(bool mode)
*/
bool wifi_mode(bool mode, wifi_info apn)
{
    if(getIfname()<0)
    {
        return false;
    } 
    
    char cmd[2048];
    char ret[1024]="";

    if(!wifi_get_power_status())
    {
        dbg_log(("wifi not active\n"));
        return false;
    }

    if(mode)
    {         
        sprintf(cmd, "nmcli con add type wifi ifname %s mode ap con-name HOTSPOT ssid '%s'",interfaceName, apn.ssid);
        if(!runCommand(cmd,ret,1024)) 
        {
            return false;
        }

        sprintf(cmd, "nmcli con modify HOTSPOT connection.autoconnect-priority 3 802-11-wireless.band bg");
        if(!runCommand(cmd,ret,1024)) 
        {
            return false;
        }

        sprintf(cmd, "nmcli con modify HOTSPOT 802-11-wireless.channel 1");
        if(!runCommand(cmd,ret,1024))
        {
            return false;
        }

        sprintf(cmd, "nmcli con modify HOTSPOT 802-11-wireless-security.key-mgmt wpa-psk");
        if(!runCommand(cmd,ret,1024))
        {
            return false;
        }

        sprintf(cmd, "nmcli con modify HOTSPOT 802-11-wireless-security.proto rsn");
        if(!runCommand(cmd,ret,1024))
        {
            return false;
        }

        sprintf(cmd, "nmcli con modify HOTSPOT 802-11-wireless-security.group ccmp");
        if(!runCommand(cmd,ret,1024))
        {
            return false;
        } 

        sprintf(cmd, "nmcli con modify HOTSPOT 802-11-wireless-security.pairwise ccmp");
        if(!runCommand(cmd,ret,1024))
        {
            return false;
        }

        sprintf(cmd, "nmcli con modify HOTSPOT 802-11-wireless-security.psk '%s'",apn.password); //password size between 8 to 63 chars
        if(!runCommand(cmd,ret,1024))
        {
            return false;
        }

        sprintf(cmd, "nmcli con modify HOTSPOT ipv4.method shared");
        if(!runCommand(cmd,ret,1024))
        {
            return false;
        }

        sprintf(cmd, "nmcli con up HOTSPOT");
        if(!runCommand(cmd,ret,1024))
        {
            return false;
        }

        return true;
    }
    else
    {
        sprintf(cmd, "nmcli device disconnect '%s'",interfaceName);
        if(!runCommand(cmd,ret,1024))
        {
            return false;
        } 

        sprintf(cmd, "nmcli con delete HOTSPOT");
        if(!runCommand(cmd,ret,1024))
        {
            return false;
        }

        return true;        
    }
}



/*
    wifi_set_ssid_lock(char *ssid_lock,bool enable)
*/
bool wifi_set_ssid_lock(char *ssid_lock,bool enable)
{
    char cmd[1024];
    char ret[1024];
    int pri=0;

    if(!wifi_get_power_status())
    {
        dbg_log(("wifi not active\n"));
        return false;
    }

    if(!wifi_client_mode())
    {
        return false;
    }

    sprintf(cmd, "nmcli con show '%s' | grep connection.autoconnect-priority | awk '{print $2}'",ssid_lock);
    if(!runCommand(cmd,ret,1024))
    {
        return false;
    }
    pri = atoi(ret);
    if(pri==1 || pri==2)
    {
        if(enable) 
        {
            if(wifi_ssid_lock_check()) 
            {
                dbg_log(("already ssid locked\n"));
                return false;
            }
            else 
            {
                if(!wifi_ssid_available(ssid_lock))
                {
                    return false;
                }

                vector con;
                vector_init(&con);
                wifi_get_ssid_preferred_list(&con);
                for(int j=0;j<con.count;j++)
                {
                    wifi_info *wificon = (wifi_info *)con.data[j];  
                
                    if(strcmp(ssid_lock, wificon->ssid)==0)
                    {
                        sprintf(cmd, "nmcli -t -f name,device connection show --active | grep '%s'", interfaceName);
                        if(!runCommand(cmd,ret,255))
                        {
                            free(wificon->ssid);
                            free(wificon->password);
                            free(wificon);
                            vector_free(&con);
                            return false;
                        }

                        char *cr_ssid = strtok(ret,":");
                        if(cr_ssid==NULL)
                        {
                            sprintf(cmd, "nmcli con up '%s'",ssid_lock);
                            if(!runCommand(cmd,ret,1024)) 
                            {
                                free(wificon->ssid);
                                free(wificon->password);
                                free(wificon);
                                vector_free(&con);
                                return false;
                            }
                            dbg_log(("Currenlty disconnected. Connecting to locked ssid\n"));
                        }
                        else if(strcmp(cr_ssid,ssid_lock))
                        {
                            sprintf(cmd, "nmcli device disconnect '%s'",interfaceName);
                            if(!runCommand(cmd,ret,1024)) 
                            {
                                free(wificon->ssid);
                                free(wificon->password);
                                free(wificon);
                                vector_free(&con);
                                return false;
                            }
                            
                            sprintf(cmd, "nmcli con up '%s'",ssid_lock);
                            if(!runCommand(cmd,ret,1024)) 
                            {
                                free(wificon->ssid);
                                free(wificon->password);
                                free(wificon);
                                vector_free(&con);
                                return false;
                            }
                            dbg_log(("currently connected to other ssid. Disconnecting and connecting to locked ssid\n"));
                        }
                        sprintf(cmd, "nmcli connection modify '%s' connection.autoconnect yes connection.autoconnect-priority 2",ssid_lock);
                        if(!runCommand(cmd,ret,1024)) 
                        {
                            free(wificon->ssid);
                            free(wificon->password);
                            free(wificon);
                            vector_free(&con);
                            return false;
                        }
                    }
                    else
                    {
                        sprintf(cmd, "nmcli connection modify '%s' connection.autoconnect no",wificon->ssid);
                        if(!runCommand(cmd,ret,1024)) 
                        {
                            free(wificon->ssid);
                            free(wificon->password);
                            free(wificon);
                            vector_free(&con);
                            return false;
                        }
                    }
                    free(wificon->ssid);
                    free(wificon->password);
                    free(wificon);                    
                }
                
                vector_free(&con);
            }
        } 
        else 
        {
            if(pri==2)
            {
                vector con;
                vector_init(&con);
                wifi_get_ssid_preferred_list(&con);
                for(int j=0;j<con.count;j++)
                {
                    wifi_info *wificon = (wifi_info *)con.data[j];
                    sprintf(cmd, "nmcli connection modify '%s' connection.autoconnect yes connection.autoconnect-priority 1", wificon->ssid);
                    if(!runCommand(cmd,ret,1024))
                    {
                        free(wificon->ssid);
                        free(wificon->password);
                        free(wificon);
                        vector_free(&con);
                        return false;
                    }
                    free(wificon->ssid);
                    free(wificon->password);
                    free(wificon);  
                }
                
                vector_free(&con);
            }
            else
            {
                dbg_log(("This ssid is not locked\n"));
                return false;
            }
        }
    }
    else
    {
        dbg_log(("SSID not avaible in preferred list\n"));
        return false;
    }

    return true;
} 


