
#include <stdio.h>
#include <glib.h>
#include <NetworkManager.h>
#include <string.h>
#include <vector.h>
#include <nmcommon.h>
#include <vector.h>
#include <sys/wait.h>
#include <switch.h>



bool port_reset(SW_PORT port_num)
{
    //reset ports
	//function to port up if not availble
    char cmd[1024];
    char ret[1024]="";

    if(port_num == ETH0)
    {
        sprintf(cmd, "ifconfig -a | grep eth0"); 
    }
    else
    {
        sprintf(cmd, "ifconfig -a | grep port%d", port_num);
    }
    if(!runcommand(cmd,ret,1024))
    {
        printf("command1 not run");
        return false;			   //command not run error
    }

	if(strlen(ret) != 0)
	{
        if(port_num == ETH0)
        {
            sprintf(cmd, "ifconfig eth0 down"); 
        }
        else
        {
		    sprintf(cmd, "ifconfig port%d down", port_num); 
        }
        if(!runcommand(cmd,ret,1024))
        {
            return false;              //command not run error
        }
        printf("port is down\n");
    }

    if(port_num == ETH0)
    {
        sprintf(cmd, "ifconfig eth0 up"); 
    }
    else
    {
        sprintf(cmd, "ifconfig port%d up", port_num); 
    }
    if(!runcommand(cmd,ret,1024))
    {
            return false;              //command not run error
    }
    
	return true;
}



bool switch_init()
{
    char cmd[1024];
    char ret[1024]="";
    char conname[255] = "";
    char ifname[255] = "";

    NMClient *client; 
    GMainLoop *loop;
    GError *   error = NULL;
    NMRemoteConnection* connection;
    client = getClient();
    if(!client)
    {
        return false;
    }

    loop = g_main_loop_new(NULL, FALSE);

    for(SW_PORT port = ETH0; port <= PORT7; port++)
    {   
        if(port == ETH0)
        {
            sprintf(conname, "eth0");
        }
        else
        {
            sprintf(conname, "port%d", port);
        }

        /*check if conname already exist*/
        connection = nm_client_get_connection_by_id(client, conname);
        if(!connection)
        {
            /* Ask NM to add the new connection */
            nm_add_connection(client, loop, conname, conname);

            /* Wait for the connection to be added */
            g_main_loop_run(loop);
        }

        port_reset(port);

    }

    /* Clean up */
    g_object_unref(client);
    return true;
}



bool port_set_config(SW_PORT port, port_config config)
{
    char cmd[1024];
    char ret[1024]="";
    char con[1024] = "";
    char conname[255] = "";
    char addresses[255] = "";

    NMClient *client; 
    GMainLoop *loop;
    GError *   error = NULL;
    NMRemoteConnection* connection;
    
    client = getClient();
    if(!client)
    {
        return false;
    }

    if(port == ETH0)
    {
        sprintf(conname, "eth0");
    }
    else
    {
        sprintf(conname, "port%d", port);
    }

    connection = nm_client_get_connection_by_id(client, conname);
    if(!connection)
    {
        return false;
    }

    NMSettingIPConfig *newip4 = (NMSettingIP4Config *) nm_setting_ip4_config_new();

    sprintf(addresses, "%s/%s", config.port_ip, config.port_subnet);
    switch (config.port_mode)
    {
        case MANUAL:          
            g_object_set(G_OBJECT(newip4),
                NM_SETTING_IP_CONFIG_ADDRESSES,
                addresses,
                NM_SETTING_IP_CONFIG_GATEWAY,
                config.port_gateway,
                NM_SETTING_IP_CONFIG_DNS,
                config.port_dns,
                NM_SETTING_IP_CONFIG_METHOD,
                NM_SETTING_IP4_CONFIG_METHOD_MANUAL,
                NULL);
            break;
        
        case DYNAMIC:
            g_object_set(G_OBJECT(newip4),
                NM_SETTING_IP_CONFIG_ADDRESSES,
                "",
                NM_SETTING_IP_CONFIG_GATEWAY,
                "",
                NM_SETTING_IP_CONFIG_DNS,
                "",
                NM_SETTING_IP_CONFIG_METHOD,
                NM_SETTING_IP4_CONFIG_METHOD_AUTO,
                NULL);
            break;
        
        case SHARED:
            g_object_set(G_OBJECT(newip4),
                NM_SETTING_IP_CONFIG_ADDRESSES,
                "",
                NM_SETTING_IP_CONFIG_GATEWAY,
                "",
                NM_SETTING_IP_CONFIG_DNS,
                "",
                NM_SETTING_IP_CONFIG_METHOD,
                NM_SETTING_IP4_CONFIG_METHOD_SHARED,
                NULL);
            break;
        
        default:
            return false;
            break;
    }

    nm_connection_replace_settings (connection, newip4, NULL);
    if(!ret)
    {
        return false;
    }

    return port_reset(port);
}