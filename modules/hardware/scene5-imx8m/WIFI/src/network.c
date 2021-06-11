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
    //int ret;
    char cmd[255];
    char ret[1024]="";
    vector a;
    vector_init(&a);
    wifi_get_ssid_preferred_list(&a);
    for(int i=0;i< a.count;i++) 
    {
        char* ssid =a.data[i];
        sprintf(cmd, "nmcli con show '%s' | grep connection.autoconnect-priority: | awk '{print $2}' ",ssid);
        if(!runCommand(cmd,ret,1024)) 
        {
            vector_free(&a);
            return false;
        }
        if(atoi(ret) == 2)
        {
            //lock is set
            vector_free(&a);
            return true;
        }
    }
    vector_free(&a);
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
    if(getIfname()<1)
    {
        return false;
    }

    char cmd[1024];
    char ret[1024];
    if(state)
    {
        sprintf(cmd, "ifconfig %s up", interfaceName);
        if(!runCommand(cmd,ret,1024))
        {
            return false;
        }
    }
    else
    {
        sprintf(cmd, "ifconfig %s down", interfaceName);
        if(!runCommand(cmd,ret,1024))
        {
            return false;
        }
    }
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
        return false;
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
    nm_client_wireless_set_enabled (client, powermode);
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
    bool enabled = nm_client_wireless_get_enabled(client);
    return enabled;
}



/*
    wifi_connect_network(struct wifiinfo credentials)
*/
int wifi_connect_network(struct wifiinfo credentials)
{ 
    NMClient* client = getClient();
    if(!client)
    {
        return 10600;
    }
    if(getIfname()<1)
    {
        return 10600;
    }
    if(wifi_ssid_lock_check())
    {
        printf("SSID lock set. Disable lock first");
        return false;
    }

    char ret[1024]="";
    char cmd[1024]="";

    sprintf(cmd, "nmcli -t -f name,device connection show --active | grep '%s'", interfaceName);
    if(!runCommand(cmd,ret,255))
    {
        printf("command failed\n");
        return false;
    }

    char *ssid  = strtok(ret,":");
    printf("p before scanf: %s\n", ssid);
    if(ssid!=NULL)
    {
        printf("Disconnect first and then connect\n");
        return false;
    }

    if(!runCommand("wpa_cli scan",ret,1024))
    {
        printf("Command failure\n");
        return 10600;
    }
    sprintf(cmd, "wpa_cli scan_results | awk -F \" \" ' {print $5 }' - | grep -E '^%s$'", credentials.ssid);
    if(!runCommand(cmd,ret,1024))
    {
        printf("Command failure\n");
        return 10600;
    }
    if(strlen(ret)==0)
    {
        printf("SSID not available\n");
        return 10605;
    }

    NMRemoteConnection* connection = nm_client_get_connection_by_id(client, credentials.ssid);
    if(!connection)
    {
        sprintf(cmd, "nmcli con add ifname '%s' type wifi con-name '%s' autoconnect off ssid '%s' wifi-sec.key-mgmt wpa-psk wifi-sec.psk '%s'",interfaceName,credentials.ssid,credentials.ssid,credentials.password);
        if(!runCommand(cmd,ret,1024))
        {
            printf("Command failure\n");
            return 10600;
        }
    }

    sprintf(cmd, "nmcli con up '%s'",credentials.ssid);
    if(!runCommand(cmd,ret,1024))
    {
        printf("adding network failed\n");
        return 10600;
    }
    return true;
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
    
    char cmd[1024];
    char ret[1024];

    if(wifi_ssid_lock_check())
    {
        printf("SSID is locked. Disable lock first");
        return false;
    }

    NMDevice *device = nm_client_get_device_by_iface (client,interfaceName);
    NMDeviceState state = nm_device_get_state(device);
    if(state != NM_DEVICE_STATE_ACTIVATED) 
    {
        printf("Already disconnected\n");
        return false;
    }
    if(!nm_device_disconnect(device, NULL, NULL))
    {
        return false;
    }
     
    printf("connection disconnected\n");  
    return true;
}



/*
    wifi_add_to_ssid_preferred_list(struct wifiinfo credentials)
*/
bool wifi_add_to_ssid_preferred_list(struct wifiinfo credentials) 
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
    //Check if given ssid exist in connection list
    NMRemoteConnection* connection = nm_client_get_connection_by_id(client, credentials.ssid);
    if(!connection)
    {
        printf("connection does not exist\n");
        if(wifi_ssid_lock_check())
        {
            sprintf(cmd, "nmcli con add ifname '%s' type wifi con-name '%s' connection.autoconnect no connection.autoconnect-priority 1 ssid '%s' wifi-sec.key-mgmt wpa-psk wifi-sec.psk '%s'",interfaceName,credentials.ssid,credentials.ssid,credentials.password);
            if(!runCommand(cmd,ret,1024))
            {
                printf("Command failure\n");
                return false;
            }
        }
        else
        {
            sprintf(cmd, "nmcli con add ifname '%s' type wifi con-name '%s' connection.autoconnect-priority 1 ssid '%s' wifi-sec.key-mgmt wpa-psk wifi-sec.psk '%s'",interfaceName,credentials.ssid,credentials.ssid,credentials.password);
            printf("%s\n",cmd);
            if(!runCommand(cmd,ret,1024))
            {
                printf("Command failure\n");
                return 10600;
            }
        }
    }
    else
    {
        sprintf(cmd, "nmcli con show '%s' | grep connection.autoconnect-priority | awk '{print $2}'",credentials.ssid);
        if(!runCommand(cmd,ret,1024))
        {
            printf("Command failure\n");
            return false;
        }
        if(atoi(ret)>=1)
        {
            //ssid already present in preffered list
            printf("SSID already added to preffered list\n");
            return 10603;
        }
        else
        {
            //ssid is in connection list but not in preffered list.
            if(wifi_ssid_lock_check())
            {
                sprintf(cmd, "nmcli con modify '%s' connection.autoconnect off connection.autoconnect-priority 1",credentials.ssid);
                if(!runCommand(cmd,ret,1024))
                {
                    printf("Command failure\n");
                    return false;
                }
            }
            else
            {
                sprintf(cmd, "nmcli con modify '%s' connection.autoconnect on connection.autoconnect-priority 1",credentials.ssid);
                if(!runCommand(cmd,ret,1024))
                {
                    printf("Command failure\n");
                    return false;
                }
            }
            printf("Connection added to preffered list\n");   
        }
    } 
    return true;
}



/*
    wifi_getsignal_strength(char* SSID)
*/
int wifi_getsignal_strength(char* SSID)
{
    NMClient* client = getClient();
    if(!client)
    {
        return 10600;
    }
    if(getIfname()<1)
    {
        return 10600;
    }

    char ret[1024]="";
    char cmd[255];
    int dBm;
    int strength;
    if(!runCommand("wpa_cli scan",ret,1024))
    {
        printf("scan failed\n");
        return 10600;
    }

    sprintf(cmd, "wpa_cli scan_results | awk -F \" \" ' {print $3 $5 }' - | grep -E '[-0-9]+%s$'", SSID);
    if(!runCommand(cmd,ret,1024))
    {
        printf("scan result failed\n");
        return 10600;
    }
    if(strlen(ret)==0)
    {
        printf("network not in range\n");
        return 10600;
    }
    sscanf(ret,"%3d%s\n", &dBm,ret);
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
    {
        return 10600;
    }
    if(getIfname()<1)
    {
        return 10600;
    }

    char ret[2048]="";
    char cmd[255];
    char ssid[128]="";
    int count =0;
    int dbm;
    
    if(!runCommand("wpa_cli scan",ret,1024))
    {
        printf("scan failed\n");
        return 10600;
    }

    sprintf(cmd, "wpa_cli scan_results | awk 'NR>2 {print $3,$5 }' - ");
    if(!runCommand(cmd,ret,2048))
    {
        printf("scan result failed\n");
        return 10600;
    }
    sleep(0.3);
    if(strlen(ret)==0)
    {
        printf("network not in range");
        return 10600;
    }
    char* p = strtok(ret,"\n");
    while(p!=NULL)
    {
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
    {
        return 10600;
    }
    if(getIfname()<1)
    {
        return 10600;
    }

    char cmd[1024];
    char ret[1024];
    int comp;
    unsigned int i;

    const GPtrArray *connections = nm_client_get_connections (client);
    for(i =0; i < connections->len;i++)
    {
        NMConnection *connection = connections->pdata[i];
        NMSettingConnection *s_con;
        s_con = nm_connection_get_setting_connection (connection);
        if (s_con)
        {
            const char* id = nm_setting_connection_get_id (s_con);
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
                    vector_add(SSIDs,(void *)id);
                    printf("filter ssid - %s \n",id);
                }
            }
        }
        else
        {
            return 10600;
        }
    }
    return 0;
}



/*
    wifi_remove_from_ssid_preferred_list(char* SSID)
*/
bool wifi_remove_from_ssid_preferred_list(char* SSID)
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

    NMRemoteConnection* connection = nm_client_get_connection_by_id(client, SSID);
    if(!connection)
    {
        printf("connection does not exist\n");
        return false;
    }
   
    sprintf(cmd, "nmcli con show '%s' | grep connection.autoconnect-priority | awk '{print $2}'",SSID);
    if(!runCommand(cmd,ret,1024))
    {
        printf("command failure\n");
        return false;
    } 
    if(atoi(ret)==2)
    {
        //SSID is locked. Cannot remove
        printf("SSID is locked. Disbale lock first\n");
        return false;
    }  
    else if(atoi(ret)==1)
    {
        sprintf(cmd, "nmcli con modify '%s' connection.autoconnect off connection.autoconnect-priority 0", SSID);
        if(!runCommand(cmd,ret,1024))
        {
            printf("fail to remove from preffered list\n");
            return false;
        }
        printf("connection modified\n");  
    }
    else
    {
        printf("connection not found in preferred list\n");
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
    vector_init(&v);
    wifi_get_ssid_preferred_list(&v);
    for(i=0;i<vector_count(&v);i++)
    {
        if(!wifi_remove_from_ssid_preferred_list(vector_get(&v,i)))
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
    wifi_set_static_ip_Address(char* ipaddress, char* subnetmask, char* gateway, char* dns)
*/
bool wifi_set_static_ip_Address(char* ipaddress, char* subnetmask, char* gateway, char* dns)
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

    if(strlen(ipaddress)>0 && strlen(subnetmask)>0)
    {
        sprintf(cmd, "ifconfig %s %s netmask %s up", interfaceName, ipaddress, subnetmask);
        if(!runCommand(cmd,ret,1024))
        {
            printf("setting IP failed\n");
            return false;
        }
    }
    if(strlen(gateway)>0)
    {
        sprintf(cmd, "route add default gw %s", gateway);
        if(!runCommand(cmd,ret,1024))
        {
            printf("setting gateway failed");
            return false;
        }
    }
    if(strlen(dns)>0)
    {
        sprintf(cmd, "echo \"nameserver %s\" >> /etc/resolv.conf", dns);
        if(!runCommand(cmd,ret,1024))
        {
            printf("dns setting failed");
            return false;
        }
    }
    return true;
}



/*
    int WiFi_usedhcp(bool enable)
*/
int WiFi_usedhcp(bool enable)
{
    if(getIfname()<0)
    {
        return 10600;
    }

    char ret[1024]="";
    char cmd[1024];

    if(enable)
    {
        sprintf(cmd,"dhclient %s",interfaceName);
        if(!runCommand(cmd, ret, 1024))
        {
            return 10600;
        }
    }
    else
    {
        sprintf(cmd,"dhclient -r %s",interfaceName);
        if(!runCommand(cmd, ret, 1024))
        {
            return 10600;
        }
    }
    return 0;
}



/*
    Select mode Client/Access Point
    wifi_mode(bool mode)
*/

bool wifi_mode(bool mode)
{
    if(getIfname()<0)
    {
        return 10600;
    } 

    char cmd[2048];
    char ret[1024]="";

    if(mode)
    {         
        sprintf(cmd, "nmcli con add type wifi ifname %s mode ap con-name HOTSPOT ssid SENCE5",interfaceName);
        if(!runCommand(cmd,ret,1024)) 
        {
            printf("fail to change the mode to Access point\n");
            return false;
        }

        sprintf(cmd, "nmcli con modify HOTSPOT 802-11-wireless.band bg");
        if(!runCommand(cmd,ret,1024)) 
        {
            printf("fail to change the mode to Access point\n");
            return false;
        }

        sprintf(cmd, "nmcli con modify HOTSPOT 802-11-wireless.channel 1");
        if(!runCommand(cmd,ret,1024))
        {
            printf("fail to change the mode to Access point\n");
            return false;
        }

        sprintf(cmd, "nmcli con modify HOTSPOT 802-11-wireless-security.key-mgmt wpa-psk");
        if(!runCommand(cmd,ret,1024))
        {
            printf("fail to change the mode to Access point\n");
            return false;
        }

        sprintf(cmd, "nmcli con modify HOTSPOT 802-11-wireless-security.proto rsn");
        if(!runCommand(cmd,ret,1024))
        {
            printf("fail to change the mode to Access point\n");
            return false;
        }

        sprintf(cmd, "nmcli con modify HOTSPOT 802-11-wireless-security.group ccmp");
        if(!runCommand(cmd,ret,1024))
        {
            printf("fail to change the mode to Access point\n");
            return false;
        } 

        sprintf(cmd, "nmcli con modify HOTSPOT 802-11-wireless-security.pairwise ccmp");
        if(!runCommand(cmd,ret,1024))
        {
            printf("fail to change the mode to Access point\n");
            return false;
        }

        sprintf(cmd, "nmcli con modify HOTSPOT 802-11-wireless-security.psk 11223344");
        if(!runCommand(cmd,ret,1024))
        {
            printf("fail to change the mode to Access point\n");
            return false;
        }

        sprintf(cmd, "nmcli con modify HOTSPOT ipv4.method shared");
        if(!runCommand(cmd,ret,1024))
        {
            printf("fail to change the mode to Access point\n");
            return false;
        }

        sprintf(cmd, "nmcli con up HOTSPOT");
        if(!runCommand(cmd,ret,1024))
        {
            printf("fail to change the mode to Access point\n");
            return false;
        }             
        return true;
    }
    else
    {
        sprintf(cmd, "nmcli device disconnect '%s'",ret);
        if(!runCommand(cmd,ret,1024))
        {
            printf("fail to change the mode to client\n");
            return false;
        } 
        sprintf(cmd, "nmcli con delete HOTSPOT");
        if(!runCommand(cmd,ret,1024))
        {
            printf("fail to change the mode to client\n");
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
    char *ssid = (char *)malloc(50);
    int status=0;
    int idx=0;

    vector a;
    vector_init(&a);
    wifi_get_ssid_preferred_list(&a);
    for(idx=0;idx< a.count;idx++)
    {
        ssid =a.data[idx];
        if(strcmp(ssid_lock,ssid) == 0)
        {
            status = 1;
            break;
        }
    }
    if(status)
    {
        if(enable) 
        {
            //int count = search_autoconnect();
            if(wifi_ssid_lock_check() == true) 
            {
                printf("already ssid locked ");
                free(ssid);
                vector_free(&a);
                return false;
            }
            else 
            {
                for(int j=0;j< a.count;j++)
                {
                    ssid =a.data[j];              
                    if(idx==j)
                    {
                        struct wifiinfo credentials;
                        credentials.ssid = ssid_lock;
                        sprintf(cmd, "nmcli -t -f name,device connection show --active | grep '%s'", interfaceName);
                        if(!runCommand(cmd,ret,255))
                        {
                            printf("command failed\n");
                            free(ssid);
                            vector_free(&a);
                            return false;
                        }

                        char *cr_ssid  = strtok(ret,":");
                        printf("current ssid: %s\n", cr_ssid);
                        if(cr_ssid==NULL)
                        {
                            if(wifi_connect_network(credentials)!=1)
                            {
                                printf("Not able to lock ssid\n");
                                free(ssid);
                                vector_free(&a);
                                return false;
                            }
                            printf("Currenlty disconnected. Connecting to locked ssid\n");
                        }
                        else if(strcmp(cr_ssid,ssid_lock))
                        {
                            wifi_disconnect_network();
                            if(wifi_connect_network(credentials)!=1)
                            {
                                wifi_reconnect();
                                printf("Not able to lock ssid\n");
                                free(ssid);
                                vector_free(&a);
                                return false;
                            }
                            printf("currently connected to other ssid. Disconnecting and connecting to locked ssid");
                        }
                        sprintf(cmd, "nmcli connection modify '%s' connection.autoconnect yes connection.autoconnect-priority 2",ssid_lock);
                        if(!runCommand(cmd,ret,1024)) 
                        {
                            free(ssid);
                            vector_free(&a);
                            return false;
                        }
                    }
                    else
                    {
                        sprintf(cmd, "nmcli connection modify '%s' connection.autoconnect no",ssid);
                        if(!runCommand(cmd,ret,1024)) 
                        {
                            free(ssid);
                            vector_free(&a);
                            return false;
                        }
                    }                      
                }
            }
        } 
        else 
        {
            sprintf(cmd, "nmcli con show '%s' | grep connection.autoconnect-priority | awk '{print $2}'",ssid_lock);
            if(!runCommand(cmd,ret,1024))
            {
                free(ssid);
                vector_free(&a);
                return false;
            }   
            if(atoi(ret)==2)
            {
                for(int j=0;j< a.count;j++)
                {
                    ssid =a.data[j];
                    sprintf(cmd, "nmcli connection modify '%s' connection.autoconnect yes connection.autoconnect-priority 1",ssid);
                    if(!runCommand(cmd,ret,1024))
                    {
                        printf("Command failed\n");
                        free(ssid);
                        vector_free(&a);
                        return false;
                    }   
                }
            }
            else
            {
                printf("This ssid is not locked\n");
                free(ssid);
                vector_free(&a);
                return false;
            }
        }
    }
    else
    {
        printf("SSID not avaible in preferred list\n");
        free(ssid);
        vector_free(&a);
        return false;
    }

    free(ssid);
    vector_free(&a);
    return true;
} 


