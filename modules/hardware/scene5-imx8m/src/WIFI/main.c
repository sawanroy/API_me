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

#include <stdio.h>
#include <stdlib.h>
#include <glib.h>

#include <NetworkManager.h>

void showConnection(){
	NMClient *client;
	GError *error = NULL;
	const GPtrArray *connections;
	unsigned int i;

	if (!(client = nm_client_new (NULL, &error))) {
		g_message ("Error: Could not connect to NetworkManager: %s.", error->message);
		g_error_free (error);
		return;
	}

	if (!nm_client_get_nm_running (client)) {
		g_message ("Error: Can't obtain connections: NetworkManager is not running.");
		return;
	}

	/* Now the connections can be listed. */
	connections = nm_client_get_connections (client);

	printf ("Connections:\n===================\n");
	for(i =0; i < connections->len;i++){
		NMConnection *connection = connections->pdata[i];
		NMSettingConnection *s_con;
		guint64 timestamp;
		char *timestamp_str;
		char timestamp_real_str[64];
		const char *val1, *val2, *val3, *val4, *val5;

		s_con = nm_connection_get_setting_connection (connection);
		if (s_con) {
			/* Get various info from NMSettingConnection and show it */
			timestamp = nm_setting_connection_get_timestamp (s_con);
			timestamp_str = g_strdup_printf ("%" G_GUINT64_FORMAT, timestamp);
			strftime (timestamp_real_str, sizeof (timestamp_real_str), "%c", localtime ((time_t *) &timestamp));

			val1 = nm_setting_connection_get_id (s_con);
			val2 = nm_setting_connection_get_uuid (s_con);
			val3 = nm_setting_connection_get_connection_type (s_con);
			val4 = nm_connection_get_path (connection);
			val5 = timestamp ? timestamp_real_str : "never";

			printf ("%-25s | %s | %-15s | %-43s | %s\n", val1, val2, val3, val4, val5);

			g_free (timestamp_str);
		}
	}
	g_object_unref(client);
	return;
}
void sayHello(char *tag){
	printf("%s: Hello!\n",tag);
}
int main(){
	printf("start");
	//sayHello("test");
	return 0;
}
