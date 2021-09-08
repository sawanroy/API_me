/*
***************************************************************************
*
* Author: Sawan Roy
*
* Copyright (C) 2021 Trunexa INC
*
* Email: sawan.roy@trunexa.com
*
***************************************************************************
*/

#include <stdio.h>
#include <errno.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <sys/param.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <signal.h>
#include <sys/time.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "bluetooth.h"

/*Global variabled*/
int system(const char *command);
static int maxpairinglimit = 5; // Bt can pair and connect to max 5 device's. Note: should be >= 1
static int paircounts = 0;
static int discoverabletimeout = 0;



/*Private functions*/
static void writeConfig(const char *fd_conf, bt_config cfg);
static bt_config readConfig(const char *fd_conf);
static bool checkIfFileExists(const char *filename);
static int bluetooth_setdevicename(unsigned char *devicename, int size);
static bool bluetoothVisibilityOn();
static bool bluetoothVisibilityOff();
static bool runCommandbt(const char *cmd, char *output, int size);
int getPaircount(void);



/*
 * bool bluetooth_on()
 */
bool bluetooth_on()
{
    char cmd[255];
    char ret[1024] = "";
    bt_config conf;
    int a;

    a = system("hciconfig hci0 up");
    if(!a)
    {
        if(checkIfFileExists("/etc/cibest_bt.cfg"))
        {
            conf = readConfig("/etc/cibest_bt.cfg");
            maxpairinglimit = conf.maxpaired;
            
            //bluetooth device name
            sprintf(cmd, "echo 'show' | bluetoothctl | grep Alias | awk '{print $2}'");
            if(runCommandbt(cmd, ret, 1024))
            {
                if(strcmp(conf.name, ret))
                {
                    bluetooth_set_config(conf);
                }
            }
        }
        return true;
    }
    else
    {
        return false;
    }
}



/*
 * bool bluetooth_off()
 */
bool bluetooth_off()
{
    int a = system("hciconfig hci0 down");
    if(!a)
    {
        return true;
    }
    else
    {
        return false;
    }
}



/*
 * bool bluetooth_pair_device(unsigned char *devicename)
 */
bool bluetooth_pair_device(unsigned char *devicename)
{
    char cmd[255];
    char ret[4096] = "";
    char cmd1[255];
    char ret1[4096] = "";
    bt_config conf;

    paircounts = getPaircount();
    conf = readConfig("/etc/cibest_bt.cfg");
    maxpairinglimit = conf.maxpaired;

    if(maxpairinglimit <= paircounts)
    {
        dbg_log(("limit exceed\n"));
        return false;
    }

    sprintf(cmd, "bluetoothctl devices");
    if(!runCommandbt(cmd, ret, 4096))
    {
        return false;
    }

    if(strstr(ret, (const char *)devicename) != NULL)
    {
        system("bluetoothctl agent off");
        system("bluetoothctl agent NoInputNoOutput");
        sprintf(cmd1, "timeout 15 bluetoothctl pair '%s'", devicename);
        if(!runCommandbt(cmd1, ret1, 4096))
        {
            return false;
        }
        if(strstr(ret1, "Pairing successful") == NULL)
        {
            return false;
        }
    }
    else
    {
        return false;
    }

    return true;
}



 /*
  * bool bluetooth_unpair_device(unsigned char *devicename)
  */
bool bluetooth_unpair_device(unsigned char *devicename)
{
    char cmd[100];
    char ret[1024];

    sprintf(cmd, "timeout 5 bluetoothctl remove '%s'", devicename);
    if(!runCommandbt(cmd, ret, 1024))
    {
        return false;
    }

    if(strstr(ret, "Device has been removed") == NULL)
    {
        return false;
    }

    return true;
}



/*
 * vector bluetooth_scan()
 */
vector bluetooth_scan()
{
    vector list;
    vector_init(&list);

    inquiry_info *ii = NULL;
    int maxrsp, numrsp;
    int devid, sock, len, flags;
    int i;
    char addr[248] = {0};
    char name[248] = {0};
    char cmd[255];
    char ret[4096] = "";

    dbg_log(("*************** BT SCAN STARTED *********************\n"));
    devid = hci_get_route(NULL);
    sock = hci_open_dev(devid);

    if(devid < 0 || sock < 0)
    {
        perror("opening socket");
        return list;
    }

    len = 8;
    maxrsp = 255;
    flags = IREQ_CACHE_FLUSH;
    ii = (inquiry_info *)malloc(maxrsp * sizeof(inquiry_info));
    numrsp = hci_inquiry(devid, len, maxrsp, NULL, &ii, flags);

    if(numrsp < 0)
    {
        perror("hci_inquiry");
    }

    for(i = 0; i < numrsp; i++)
    {
        ba2str(&(ii + i)->bdaddr, addr);
        memset(name, 0, sizeof(name));
        if(hci_read_remote_name(sock, &(ii + i)->bdaddr, sizeof(name), name, 0) < 0)
        {
            return list;
        }
        else
        {
            bt_scanresult *res = (bt_scanresult *)malloc(sizeof(bt_scanresult));
            res->devname = strdup(name);
            res->devaddress = strdup(addr);

            vector_add(&list, res);
            //free(res->devname);
            //free(res->devaddress);
        }
    }

    free(ii);
    close(sock);

    sprintf(cmd, "timeout 10 bluetoothctl scan on");
    runCommandbt(cmd, ret, 4096);
    return list;
}



/*
 * bool bluetooth_set_config(bt_config conf)
 */
bool bluetooth_set_config(bt_config conf)
{
    char cmd[255];
    char ret[1024] = "";
    bool retval = true;

    maxpairinglimit = conf.maxpaired;
    discoverabletimeout = conf.discoverabletimeout;

    writeConfig("/etc/cibest_bt.cfg", conf);

    if(conf.radioOn)
    {
        if(system("rfkill unblock bluetooth"))
        {
            retval = false;
        }
    }
    else
    {
        if(system("rfkill block bluetooth"))
        {
            retval = false;
        }
    }

    if(conf.enabled)
    {
        if(system("hciconfig hci0 up") == 1)
        {
            retval = false;
        }
    }
    else
    {
        if(!bluetooth_off())
        {
            retval = false;
        }
    }

    if(conf.discoverable)
    {
        if(!bluetoothVisibilityOn())
        {
            retval = false;
        }
    }
    else
    {
        if(!bluetoothVisibilityOff())
        {
            retval = false;
        }
    }

    if(bluetooth_setdevicename((unsigned char *)conf.name, 255) <= 0)
    {
        retval = false;
    }

    sprintf(cmd, "echo 'discoverable-timeout %d' | bluetoothctl", discoverabletimeout);
    if(!runCommandbt(cmd, ret, 1024))
    {
        retval = false;
    }

    return retval;
}



/*
 * bt_config bluetooth_get_config()
 */
bt_config bluetooth_get_config()
{
    bt_config conf;
    char ret[1024] = "";
    char cmd[255];
    char ret1[1024] = "";
    char cmd1[255];
    char ret2[1024] = "";
    char cmd2[255];
    char ret3[1024] = "";
    char cmd3[255];
    char ret4[1024] = "";
    char cmd4[255];

    if(checkIfFileExists("/etc/cibest_bt.cfg"))
    {
        conf = readConfig("/etc/cibest_bt.cfg");
        maxpairinglimit = conf.maxpaired;
    }

    sprintf(cmd, "rfkill list bluetooth | grep Soft | awk '{print $3}'");
    if(!runCommandbt(cmd, ret, 1024))
    {
        strcpy(conf.name, "");
        return conf;
    }
    if(strstr(ret, "no") != NULL)
    {
        conf.radioOn = true;
    }
    else
    {
        conf.radioOn = false;
    }

    //read bluetooth is enable or not
    sprintf(cmd1, "echo 'show' | bluetoothctl | grep Powered | awk '{print $2}'");
    if(!runCommandbt(cmd1, ret1, 1024))
    {
        strcpy(conf.name, "");
        return conf;
    }

     if(strstr(ret1, "yes") != NULL)                     // if successful
     {
         conf.enabled = true;
     }
     else
     {
         conf.enabled = false;
     }

    //discoverable state
    sprintf(cmd2, "echo 'show' | bluetoothctl | grep 'Discoverable:' | awk '{print $2}'");
    if(!runCommandbt(cmd2, ret2, 1024))
    {
        strcpy(conf.name, "");
        return conf;
    }

    if(strstr(ret2, "yes") != NULL)
    {
        conf.discoverable = true;
    }
    else
    {
        conf.discoverable = false;
    }

    //bluetooth device discoverableTimeout
    sprintf(cmd3, "echo 'show' | bluetoothctl | grep 'DiscoverableTimeout:' | awk '{print $2}'");
    if(!runCommandbt(cmd3, ret3, 1024))
    {
        strcpy(conf.name, "");
        return conf;
    }

        //bluetooth device name
    sprintf(cmd4, "echo 'show' | bluetoothctl | grep Alias | awk '{print $2}'");
    if(!runCommandbt(cmd4, ret4, 1024))
    {
        strcpy(conf.name, "");
        return conf;
    }

    conf.discoverabletimeout = (int)strtol(ret3, NULL, 16);

    strcpy(conf.name,ret4); /* allocate a 5-byte block of memory for name */
    conf.maxpaired = maxpairinglimit;

    return conf;
}



/*
 * bool bluetooth_connect_to_device(char *name)
 */
bool bluetooth_connect_to_device(char *name)
{
    char cmd[100];
    char ret[1024];

    sprintf(cmd, "timeout 5 bluetoothctl connect '%s'", name);
    if(!runCommandbt(cmd, ret, 1024))
    {
        return false;
    }

    if(strstr(ret, "Connection successful") != NULL)
    {
        return true;
    }
    else
    {
        return false;
    }
}



/*
*	Private functions 
*/

void writeConfig(const char *fd_conf, bt_config cfg)
{
    FILE *f = fopen(fd_conf, "w");
    fwrite(&cfg, sizeof(cfg), 1, f);
    fclose(f);
}



bt_config readConfig(const char *fd_conf)
{
   FILE *fpread;
   bt_config b;

   fpread = fopen(fd_conf, "r");
   while(fread(&b, sizeof(b), 1, fpread) > 0)
   {
   }

   fclose(fpread);
   return b;
}



bool checkIfFileExists(const char *filename)
{
    struct stat buffer;

    int exist = stat(filename, &buffer);
    if(exist == 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}



int bluetooth_setdevicename(unsigned char *devicename, int size)
{
    int devid, sock, len;
    devid = hci_get_route(NULL);
    sock = hci_open_dev(devid);

    if(devid < 0 || sock < 0)
    {
        perror("opening socket");
        return -1;
    }

    if(hci_write_local_name(sock, (const char *)devicename, size) < 0)
    {
        fprintf(stderr, "Can't change local name on hci%d: %s (%d)\n",
                devid, strerror(errno), errno);
        return -1;
    }
    else
    {
        len = strlen((char *)devicename);
    }

    close(sock);
    return len;
}



bool bluetoothVisibilityOn()
{
    FILE *fp;
    char path[100];

    fp = popen("echo 'discoverable on' | bluetoothctl", "r");
    if(fp == NULL)
    {
        return false;
    }
    /* Read the output a line at a time - output it. */
    while(fgets(path, sizeof(path) - 1, fp) != NULL)
    {
    }
    /* close */
    pclose(fp);
    return true;
}



bool bluetoothVisibilityOff()
{
    FILE *fp;
    char path[100];

    fp = popen("echo 'discoverable off' | bluetoothctl", "r");
    if(fp == NULL)
    {
        return false;
    }
    /* Read the output a line at a time - output it. */
    while(fgets(path, sizeof(path) - 1, fp) != NULL)
    {
    }
    /* close */
    pclose(fp);
    return true;
}



bool runCommandbt(const char *cmd, char *output, int size)
{
    memset(output,'\0', size);
    FILE *fp = popen(cmd,"r");

    if(!fp)
    {
        dbg_log(("opening pipe failed\n"));
        return false;
    }

    while(0 != fread(output, size , sizeof(char), fp))
    {
    }

    if(!feof(fp))
    {
        pclose(fp);
        return false;
    }

    int status = pclose(fp);
    if(status < 0)
    {
        return false;
    }
    return true;
}



int getPaircount(void)
{
    char cmd[255];
    char ret[1024] = "";

    sprintf(cmd, "bluetoothctl paired-devices | wc -l");
    if(!runCommandbt(cmd, ret, 1024))
    {
        return -1;
    }

    return atoi(ret);
}