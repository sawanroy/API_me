#include <NetworkManager.h>
#include <nmcommon.h>
#include <stdio.h>


/*Execute a command and get output*/
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


/*Get network manager client*/
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



