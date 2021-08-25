/*
***************************************************************************
*
* Author: Nikhil Anjane
*
* Copyright (C) 2021 TRUNEXA INC
*
* Email: nikhil.anjane@trunexa.com
*
***************************************************************************
*/

#include <stdio.h>
#include <glib.h>
#include <NetworkManager.h>
#include <string.h>
#include <nmcommon.h>
#include <sys/wait.h>
#include <switch.h>
#include <unistd.h>

/*Private functions*/
static void nmAddConnection(NMClient *client, GMainLoop *loop, const char *conname, const char *ifname);
static void addedCb(GObject *client, GAsyncResult *result, gpointer user_data);
static void modCb(NMRemoteConnection *connection, GAsyncResult *result, gpointer user_data);
static void activateCb(GObject *client, GAsyncResult *result, gpointer user_data);
static void reapplyCb(NMDevice *device, GAsyncResult *result, gpointer user_data);
static void reloadCb(GObject *client, GAsyncResult *result, gpointer user_data);

static ERROR_CODE ERROR;

/*
 * bool switch_port_reset(SW_PORT port_num)
 * function to reset switch ports
 */
bool switch_port_reset(SW_PORT port_num)
{
    char cmd[1024];
    char ret[1024] = "";

    if(port_num < ETH0 || port_num > PORT7)
    {
        return false;
    }

    if(port_num == ETH0)
    {
        sprintf(cmd, "ifconfig -a | grep eth0");
    }
    else
    {
        sprintf(cmd, "ifconfig -a | grep port%d", port_num);
    }
    if(!runCommand(cmd, ret, 1024))
    {
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
        if(!runCommand(cmd, ret, 1024))
        {
            return false;              //command not run error
        }
        usleep(50000);
    }

    if(port_num == ETH0)
    {
        sprintf(cmd, "ifconfig eth0 up");
    }
    else
    {
        sprintf(cmd, "ifconfig port%d up", port_num);
    }
    if(!runCommand(cmd, ret, 1024))
    {
        return false;              //command not run error
    }

    return true;
}



/*
 * bool switch_init()
 * function to init switch
 */
int switch_init()
{
    char conname[255] = "";

    NMClient *client;
    NMDevice *device;
    GMainLoop *loop1;
    GMainLoop *loop2;
    GMainLoop *loop3;
    NMRemoteConnection *connection;
    ERROR = SUCCESS;

    client = getClient();
    if(!client)
    {
        return false;
    }

    loop1 = g_main_loop_new(NULL, FALSE);
    loop2 = g_main_loop_new(NULL, FALSE);
    loop3 = g_main_loop_new(NULL, FALSE);

    /*Reload all connections from disk*/
    nm_client_reload_connections_async(client, NULL, reloadCb, loop1);
    g_main_loop_run(loop1);

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
        connection = (NMRemoteConnection *)nm_client_get_connection_by_id(client, conname);
        if(!connection)
        {
            /* Ask NM to add the new connection */
            nmAddConnection(client, loop2, conname, conname);

            /* Wait for the connection to be added */
            g_main_loop_run(loop2);

            connection = (NMRemoteConnection *)nm_client_get_connection_by_id(client, conname);
        }

        device = nm_client_get_device_by_iface(client, conname);
        const char *path = nm_object_get_path(NM_OBJECT(connection));
        nm_client_activate_connection_async(client, (NMConnection *)connection, device, path, NULL, activateCb, loop3);
        /* Wait for the connection to be added */
        g_main_loop_run(loop3);

        if(ERROR != SUCCESS)
        {
            switch_port_reset(port);
        }
        else
        {
            ERROR = switch_port_reset(port);
        }
    }

    /* Clean up */
    g_object_unref(client);
    return ERROR;
}



/*
 * bool switch_set_config(SW_PORT port, port_config config)
 * function to set the switch port config
 */
int switch_set_config(SW_PORT port, port_config config)
{
    char conname[255] = "";

    NMClient *client;
    NMRemoteConnection *connection;
    NMIPAddress *addresses;
    NMSettingIP4Config *newip4;
    guint prefix;
    NMDevice *device;
    GMainLoop *loop1;
    GMainLoop *loop2;
    GMainLoop *loop3;
    ERROR = SUCCESS;

    if(port < ETH0 || port > PORT7)
    {
        return FAILURE;
    }

    client = getClient();
    if(!client)
    {
        return FAILURE;
    }

    if(port == ETH0)
    {
        sprintf(conname, "eth0");
    }
    else
    {
        sprintf(conname, "port%d", port);
    }

    loop1 = g_main_loop_new(NULL, FALSE);
    loop2 = g_main_loop_new(NULL, FALSE);
    loop3 = g_main_loop_new(NULL, FALSE);

    connection = (NMRemoteConnection *)nm_client_get_connection_by_id(client, conname);
    if(!connection)
    {
        return false;
    }

    newip4 = (NMSettingIP4Config *)nm_setting_ip4_config_new();

    switch (config.port_mode)
    {
        case MANUAL:
            prefix = (guint)atoi(config.port_prefix);
            if((nm_utils_ipaddr_valid(AF_INET, config.port_ip) &&
                nm_utils_ipaddr_valid(AF_INET, config.port_dns) &&
                nm_utils_ipaddr_valid(AF_INET, config.port_gateway) &&
                (prefix <= 32)) != 1)
            {
                return FAILURE;
            }
            addresses = nm_ip_address_new(AF_INET, config.port_ip, prefix, NULL);
            g_object_set(G_OBJECT(newip4),
                NM_SETTING_IP_CONFIG_METHOD,
                NM_SETTING_IP4_CONFIG_METHOD_MANUAL,
                NM_SETTING_IP_CONFIG_GATEWAY,
                config.port_gateway,
                NULL);

            if(!nm_setting_ip_config_add_address((NMSettingIPConfig *)newip4, addresses))
            {
                return FAILURE;
            }

            if(!nm_setting_ip_config_add_dns((NMSettingIPConfig *)newip4, config.port_dns))
            {
                return FAILURE;
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
            return FAILURE;
            break;
    }

    /*Remove existing ipv4 setting*/
    nm_connection_remove_setting((NMConnection *)connection, NM_TYPE_SETTING_IP4_CONFIG);

    /*Add new configured ipv4 setting*/
    nm_connection_add_setting((NMConnection *)connection, NM_SETTING(newip4));

    /*Save connection*/
    nm_remote_connection_commit_changes_async(connection, TRUE, NULL, (GAsyncReadyCallback)modCb, loop1);
    /* Wait for the connection to be added */
    g_main_loop_run(loop1);

    /*Reapply changes to device*/
    device = nm_client_get_device_by_iface(client, conname);

    /*Load new connection and reset port*/
    const char *path = nm_object_get_path(NM_OBJECT(connection));

    nm_client_activate_connection_async(client, (NMConnection *)connection, device, path, NULL, activateCb, loop2);
    /* Wait for the connection to be added */
    g_main_loop_run(loop2);

    nm_device_reapply_async(device, (NMConnection *)connection, 0, 0, NULL, (GAsyncReadyCallback)reapplyCb, loop3);
    /* Wait for the connection to be added */
    g_main_loop_run(loop3);

    g_object_unref(connection);

    if(ERROR != SUCCESS)
    {
        switch_port_reset(port);
        return ERROR;
    }
    else
    {
        return switch_port_reset(port);
    }

}



/*
 * bool switch_get_config(SW_PORT port, port_config *config)
 * function to get the switch port config
 */
bool switch_get_config(SW_PORT port, port_config *config)
{

    char conname[255] = "";

    NMClient *client;
    NMRemoteConnection *connection;
    NMIPAddress *addresses;
    NMSettingIP4Config *setting;
    const char *str;
    int prefix;
    char prefixstr[10];

    if(port < ETH0 || port > PORT7)
    {
        return false;
    }

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

    connection = (NMRemoteConnection *)nm_client_get_connection_by_id(client, conname);
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

    if(config->port_mode == MANUAL)
    {
        /*Get IP and subnet*/
        addresses = nm_setting_ip_config_get_address((NMSettingIPConfig *)setting, 0);
        config->port_ip = nm_ip_address_get_address(addresses);
        prefix = (int)nm_ip_address_get_prefix(addresses);
        sprintf((char *)prefixstr, "%d", prefix);
        config->port_prefix = strdup(prefixstr);

        /*Get gateway*/
        config->port_gateway = nm_setting_ip_config_get_gateway((NMSettingIPConfig *)setting);

        /*Get dns*/
        config->port_dns = nm_setting_ip_config_get_dns((NMSettingIPConfig *)setting, 0);
    }
    else
    {
        config->port_ip = "";
        config->port_prefix = "";
        config->port_gateway = "";
        config->port_dns = "";
    }

    return true;
}



/*
 * void reloadCb(GObject *client, GAsyncResult *result, gpointer user_data)
 * callback function for reload api
 */
void reloadCb(GObject *client, GAsyncResult *result, gpointer user_data)
{
    GError *error = NULL;
    GMainLoop *loop = user_data;

    nm_client_reload_connections_finish(NM_CLIENT(client), result, &error);

    if(error)
    {
        ERROR = RELOAD_FAILURE;
        dbg_log(("Error reload connection: %s", error->message));
        g_error_free(error);
    }
    else
    {
        dbg_log(("reloaded connections\n"));
    }

    /* Tell the mainloop we're done and we can quit now */
    g_main_loop_quit(loop);
}





/*
 * void reapplyCb(NMDevice *device, GAsyncResult *result, gpointer user_data)
 * callback function for reapply api
 */
void reapplyCb(NMDevice *device, GAsyncResult *result, gpointer user_data)
{
    GError *error = NULL;
    GMainLoop *loop = user_data;

    sleep(1);
    nm_device_reapply_finish(device, result, &error);

    if(error)
    {
        ERROR = REAPPLY_FAILURE;
        dbg_log(("Error reapply connection: %s", error->message));
        g_error_free(error);
    }
    else
    {
        dbg_log(("reapplied connection\n"));
    }

    /* Tell the mainloop we're done and we can quit now */
    g_main_loop_quit(loop);
}



/*
 * void activateCb(GObject *client, GAsyncResult *result, gpointer user_data)
 * callback function for activate api
 */
void activateCb(GObject *client, GAsyncResult *result, gpointer user_data)
{
    GError *error = NULL;
    NMActiveConnection *connection;
    GMainLoop *loop = user_data;

    sleep(1);
    connection = nm_client_activate_connection_finish(NM_CLIENT(client), result, &error);

    if(error)
    {
        ERROR = ACTIVATION_FAILURE;
        dbg_log(("Error activating connection: %s", error->message));
        g_error_free(error);
    }
    else
    {
        dbg_log(("activated connection\n"));
        g_object_unref(connection);
    }

    /* Tell the mainloop we're done and we can quit now */
    g_main_loop_quit(loop);
}



/*
 * void modCb(NMRemoteConnection *connection, GAsyncResult *result, gpointer user_data)
 * callback function for save and commit api
 */
void modCb(NMRemoteConnection *connection, GAsyncResult *result, gpointer user_data)
{
    GError *error = NULL;
    GMainLoop *loop = user_data;

    nm_remote_connection_commit_changes_finish(connection, result, &error);

    if(error)
    {
        ERROR = MODIFY_FAILURE;
        dbg_log(("Error modifying connection: %s", error->message));
        g_error_free(error);
    }
    else
    {
        dbg_log(("Modified: %s\n", nm_connection_get_path(NM_CONNECTION(connection))));
    }

    g_main_loop_quit(loop);
}



/*
 * void addedCb(GObject *client, GAsyncResult *result, gpointer user_data)
 * callback function for add connection api
 */
void addedCb(GObject *client, GAsyncResult *result, gpointer user_data)
{
    GMainLoop *loop = user_data;
    NMRemoteConnection *remote;
    GError *error = NULL;

    remote = nm_client_add_connection_finish(NM_CLIENT(client), result, &error);

    if(error)
    {
        ERROR = ADDITION_FAILURE;
        dbg_log(("Error adding connection: %s", error->message));
        g_error_free(error);
    }
    else
    {
        dbg_log(("Added: %s\n", nm_connection_get_path(NM_CONNECTION(remote))));
        g_object_unref(remote);
    }

    /* Tell the mainloop we're done and we can quit now */
    g_main_loop_quit(loop);
}



/*
 * static void nmAddConnection(NMClient *client, GMainLoop *loop, const char *conname, const char *ifname)
 * Internal function to handle addition of connection
 */
static void nmAddConnection(NMClient *client, GMainLoop *loop, const char *conname, const char *ifname)
{
    NMConnection *connection;
    NMSettingConnection *s_con;
    NMSettingWired *s_wired;
    NMSettingIP4Config *s_ip4;
    char *uuid;

    /* Create a new connection object */
    connection = nm_simple_connection_new();

    /* Build up the 'connection' Setting */
    s_con = (NMSettingConnection *)nm_setting_connection_new();
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
    s_wired = (NMSettingWired *)nm_setting_wired_new();
    nm_connection_add_setting(connection, NM_SETTING(s_wired));

    /* Build up the 'ipv4' Setting */
    s_ip4 = (NMSettingIP4Config *)nm_setting_ip4_config_new();
    g_object_set(G_OBJECT(s_ip4),
                 NM_SETTING_IP_CONFIG_METHOD,
                 NM_SETTING_IP4_CONFIG_METHOD_AUTO,
                 NULL);
    nm_connection_add_setting(connection, NM_SETTING(s_ip4));

    /* Ask the settings service to add the new connection; we'll quit the
     * mainloop and exit when the callback is called.
     */
    nm_client_add_connection_async(client, connection, TRUE, NULL, addedCb, loop);

    g_object_unref(connection);
}
