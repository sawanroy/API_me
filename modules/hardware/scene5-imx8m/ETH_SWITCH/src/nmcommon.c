#include <NetworkManager.h>
#include <nmcommon.h>
#include <stdio.h>


/*Internal function*/
bool runcommand(const char *cmd, char *output, int size)
{
    memset(output,'\0', size);
    FILE* fp = popen(cmd, "r");
    if(!fp)
    {
        printf("opening pipe failed\n");
        return false;
    }
    while(fread(output, size , sizeof(char), fp) != 0)
    {

    }
    if(!feof(fp))
    {
        printf("error while reading output\n");
        pclose(fp);
        return false;
    }
    int status = pclose(fp);
    if(status < 0)
    {
        printf("command exist status%d\n",status);
        return false;
    }

    return true;
}



NMClient* getClient()
{
    NMClient *client = NULL;
    GError *error = NULL;

    if(!(client=nm_client_new(NULL, &error))) 
    {
        g_message("Error: Could not connect to NetworkManager: %s.", error->message);
        g_error_free(error);
        return NULL;
    }

    if(!nm_client_get_nm_running(client)) 
    {
        g_message("Error: Can't obtain connections: NetworkManager is not running.");
        return NULL;
    }

    return client;
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
