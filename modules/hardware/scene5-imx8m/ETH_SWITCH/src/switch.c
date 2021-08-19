
#include <stdio.h>
#include <glib.h>
#include <NetworkManager.h>
#include <string.h>
#include <nmcommon.h>
#include <sys/wait.h>
#include <switch.h>


static void nm_add_connection(NMClient *client, GMainLoop *loop, const char *conname, const char *ifname);
static void added_cb(GObject *client, GAsyncResult *result, gpointer user_data);
static void mod_cb(NMRemoteConnection *connection, GAsyncResult *result, gpointer user_data);


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



bool switch_set_config(SW_PORT port, port_config config)
{
    char cmd[1024];
    char ret[1024]="";
    char con[1024] = "";
    char conname[255] = "";
    //char addresses[255] = "";

    NMClient *client; 
    GMainLoop *loop;
    GError *   error = NULL;
    NMRemoteConnection* connection;
    gboolean status;
    NMIPAddress *addresses;
    guint prefix;

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

    NMSettingIP4Config *newip4 = (NMSettingIP4Config *) nm_setting_ip4_config_new();

    
    switch (config.port_mode)
    {
        case MANUAL:
            prefix = (guint)config.port_prefix;
            if((nm_utils_ipaddr_valid(AF_INET, config.port_ip) &&
                nm_utils_ipaddr_valid(AF_INET, config.port_dns) &&
                nm_utils_ipaddr_valid(AF_INET, config.port_gateway) &&
                (prefix <= 32)) != 1)
            {
                return false;
            }
            addresses = nm_ip_address_new(AF_INET, config.port_ip, prefix, NULL);     
            g_object_set(G_OBJECT(newip4),
                NM_SETTING_IP_CONFIG_METHOD,
                NM_SETTING_IP4_CONFIG_METHOD_MANUAL,
                NM_SETTING_IP_CONFIG_GATEWAY,
                config.port_gateway,
                NULL);

            if(!nm_setting_ip_config_add_address(newip4, addresses))
            {
                return false;
            }

            if(!nm_setting_ip_config_add_dns(newip4, config.port_dns))
            {
                return false;
            }
            break;
        
        case DYNAMIC:
            g_object_set(G_OBJECT(newip4),
                NM_SETTING_IP_CONFIG_METHOD,
                NM_SETTING_IP4_CONFIG_METHOD_AUTO,
                NULL);
            break;
        
        case SHARED:
            g_object_set(G_OBJECT(newip4),
                NM_SETTING_IP_CONFIG_METHOD,
                NM_SETTING_IP4_CONFIG_METHOD_SHARED,
                NULL);
            break;
        
        default:
            return false;
            break;
    }

    /*Remove existing ipv4 setting*/
    nm_connection_remove_setting(connection, NM_TYPE_SETTING_IP4_CONFIG);
    
    /*Add new configured ipv4 setting*/
    nm_connection_add_setting(connection, NM_SETTING(newip4));

    /*Save connection*/
    nm_remote_connection_commit_changes_async(connection, TRUE, NULL, mod_cb, NULL);

    g_object_unref(connection);

    return port_reset(port);
}



bool switch_get_config(SW_PORT port, port_config *config)
{
    char cmd[1024];
    char ret[1024] = "";
    char con[1024] = "";
    char conname[255] = "";

    NMClient *client; 
    GMainLoop *loop;
    GError *   error = NULL;
    NMRemoteConnection* connection;
    gboolean status;
    NMIPAddress *addresses;
    guint prefix;
    NMSettingIP4Config *setting;
    const char *str;

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
    
    setting = (NMSettingIP4Config *)nm_connection_get_setting_ip4_config((NMConnection *)connection);
    if(!setting)
    {
        return false;
    }

    /*Get mode*/
    str = nm_setting_ip_config_get_method((NMSettingIPConfig *)setting);

    if(strcmp(str, "auto") == 0)
    {
        config->port_mode = DYNAMIC;
    }
    else if(strcmp(str, "manual") == 0)
    {
        config->port_mode = MANUAL;
    }
    else if(strcmp(str, "shared") == 0)
    {
        config->port_mode = SHARED;
    }
    else
    {
        return false;
    }

    /*Get IP and subnet*/
    addresses = nm_setting_ip_config_get_address(setting, 0);
    config->port_ip = nm_ip_address_get_address(addresses);    
    config->port_prefix = (int)nm_ip_address_get_prefix(addresses);

    /*Get gateway*/
    config->port_gateway = nm_setting_ip_config_get_gateway((NMSettingIPConfig *)setting);
    
    /*Get dns*/
    config->port_dns = nm_setting_ip_config_get_dns ((NMSettingIPConfig *)setting, 0);
    
    return true;
}



void mod_cb(NMRemoteConnection *connection, GAsyncResult *result, gpointer user_data)
{
    GMainLoop *         loop = user_data;
    NMRemoteConnection *remote;
    GError *            error = NULL;
    gboolean            status;
    /* NM responded to our request; either handle the resulting error or
     * print out the object path of the connection we just added.
     */
    status = nm_remote_connection_commit_changes_finish(connection, result, &error);

    if (error) {
        g_print("Error adding connection: %s", error->message);
        g_error_free(error);
    } else {
        g_print("Added: %s\n", nm_connection_get_path(NM_CONNECTION(remote)));
        g_object_unref(remote);
    }

    /* Tell the mainloop we're done and we can quit now */
    //g_main_loop_quit(loop);
}


void added_cb(GObject *client, GAsyncResult *result, gpointer user_data)
{
    GMainLoop *         loop = user_data;
    NMRemoteConnection *remote;
    GError *            error = NULL;

    /* NM responded to our request; either handle the resulting error or
     * print out the object path of the connection we just added.
     */
    remote = nm_client_add_connection_finish(NM_CLIENT(client), result, &error);

    if (error) {
        g_print("Error adding connection: %s", error->message);
        g_error_free(error);
    } else {
        g_print("Added: %s\n", nm_connection_get_path(NM_CONNECTION(remote)));
        g_object_unref(remote);
    }

    /* Tell the mainloop we're done and we can quit now */
    g_main_loop_quit(loop);
}



void nm_add_connection(NMClient *client, GMainLoop *loop, const char *conname, const char *ifname)
{
    NMConnection *       connection;
    NMSettingConnection *s_con;
    NMSettingWired *     s_wired;
    NMSettingIP4Config * s_ip4;
    char *               uuid;

    /* Create a new connection object */
    connection = nm_simple_connection_new();

    /* Build up the 'connection' Setting */
    s_con = (NMSettingConnection *) nm_setting_connection_new();
    uuid  = nm_utils_uuid_generate();
    g_object_set(G_OBJECT(s_con),
                 NM_SETTING_CONNECTION_UUID,
                 uuid,
                 NM_SETTING_CONNECTION_ID,
                 conname,
                 NM_SETTING_CONNECTION_TYPE,
                 "802-3-ethernet",
                 NM_SETTING_CONNECTION_INTERFACE_NAME,
                 ifname,
                 NULL);
    g_free(uuid);
    nm_connection_add_setting(connection, NM_SETTING(s_con));

    /* Build up the 'wired' Setting */
    s_wired = (NMSettingWired *) nm_setting_wired_new();
    nm_connection_add_setting(connection, NM_SETTING(s_wired));

    /* Build up the 'ipv4' Setting */
    s_ip4 = (NMSettingIP4Config *) nm_setting_ip4_config_new();
    g_object_set(G_OBJECT(s_ip4),
                 NM_SETTING_IP_CONFIG_METHOD,
                 NM_SETTING_IP4_CONFIG_METHOD_AUTO,
                 NULL);
    nm_connection_add_setting(connection, NM_SETTING(s_ip4));

    /* Ask the settings service to add the new connection; we'll quit the
     * mainloop and exit when the callback is called.
     */
    nm_client_add_connection_async(client, connection, TRUE, NULL, added_cb, loop);
    g_object_unref(connection);
}
