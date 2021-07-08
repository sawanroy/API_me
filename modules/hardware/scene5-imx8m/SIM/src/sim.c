/*
***************************************************************************
*
* Author:Sawan Roy
*
* Copyright (C) 2021 TRUNEXA PVT LTD
*
* Email: sawan.roy@trunexa.com
*
*
***************************************************************************
*/



#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <termios.h>
#include <sys/file.h>
#include <sys/wait.h>
#include <ctype.h>
#include <stdbool.h>
#include <sim.h>
#include <usb.h>
#include <string.h>

#define bsize 256

int fd ;
unsigned char *buf;
int size;



/*Internal function*/
int sim_open_port()
{
    fd = usb_open(SIM_PORT_AT, SIM_Baud_Rate);
    if(fd < 0)
    {
        return -1;
    }
    else
    {
        return fd;
    }
}



/*Internal function*/
void sim_close_port(int fd)
{
    usb_close(fd);
}



/*Internal function*/
bool runCommand(const char *cmd, char *output, int size)
{
    memset(output,'\0', size);
    FILE* fp = popen(cmd, "r");
    if(!fp)
    {
        printf("opening pipe failed\n");
        return false;
    }
    while(NULL != fread(output, size , sizeof(char), fp))
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



/*Internal function*/
bool sim_card_available()
{
    char *imsi = sim_get_imsi();
    if(strcmp(imsi, "") == 0)
    {
        dbg_log(("SIM card not available\n"));
        free(imsi);
        return false;
    }

    free(imsi);
    return true;
}



/*
bool sim_unlock(int pincode)
This function activates the sim card by using sim pin
*/
bool sim_unlock(int pincode)
{
    unsigned char cmd[17];
    int len = sizeof(cmd);
    int fd;
    unsigned char *ret = malloc(bsize);
    bool status = true;

    snprintf((char *)cmd, sizeof(cmd), "AT+CPIN=%d\r\n", pincode);//AT+CPIN="1234"
    fd = sim_open_port();
    tcflush(fd, TCIOFLUSH);
    if(usb_write(fd, cmd, len))
    {
        if(usb_read(fd, ret, bsize) < 0)
        {
            free(ret);
            sim_close_port(fd);
            return false;
        }
        else
        {
            if(strstr(ret, "ERROR") != NULL)
            {
                status = false;
            } 
            free(ret);
            sim_close_port(fd);
            return status;
        }
    }
    else
    {
        free(ret);
        sim_close_port(fd);
        return false;
    }
}



/*
Dialup_connect(int pincode)
This function tries to connect to telephone network
*/
bool sim_dialup_connect(unsigned char phone_no[])
{
    unsigned char cmd[17];
    int len = sizeof(cmd);
    int fd;
    unsigned char *ret = malloc(bsize);
    bool status = true;

    if(!sim_card_available())
    {
        free(ret);
        return false;
    }

    snprintf((char *)cmd, len, "ATD%s;\r\n", phone_no);
    fd = sim_open_port();
    tcflush(fd, TCIOFLUSH);
    if(usb_write(fd, cmd, len))
    {
        if(usb_read(fd, ret, bsize) < 0)
        {
            free(ret);
            sim_close_port(fd);
            return false;
        }
        else
        {
            if(strstr(ret, "ERROR") != NULL)
            {
                status = false;
            } 
            free(ret);
            sim_close_port(fd);
            return status;
        }
    }
    else
    {
        free(ret);
        sim_close_port(fd);
        return false;
    }
}



/*
Dialup_disconnect()
This function disconnect from telephone network
*/
bool sim_dialup_disconnect()
{
    unsigned char cmd[] = "AT+CHUP\r\n";
    int len = sizeof(cmd);
    int fd;
    unsigned char *ret = malloc(bsize);
    bool status = true;

    if(!sim_card_available())
    {
        free(ret);
        return false;
    }

    fd = sim_open_port();
    tcflush(fd, TCIOFLUSH);
    if(usb_write(fd, cmd, len))
    {
        if(usb_read(fd, ret, bsize) < 0)
        {
            free(ret);
            sim_close_port(fd);
            return false;
        }
        else
        {
            if(strstr(ret, "ERROR") != NULL)
            {
                status = false;
            } 
            free(ret);
            sim_close_port(fd);
            return status;
        }
    }
    else
    {
        sim_close_port(fd);
        return false;
    }
}



/*
set_AP(struct apn setapn)
This function creates or update an AP (identified by its name)
*/
bool sim_set_ap(struct apn setapn)
{
    system("nmcli connection delete gprs");
    FILE *fp;
    int n;
    char path[1035];
    const char *str1 = "successfully added";
    char cmd[50];
    sprintf(cmd,"nmcli connection add type gsm con-name gprs ifname ttyUSB%d apn %s", SIM_PORT, setapn.apn_name);

    /* Open the command for reading. */
    fp = popen(cmd, "r");
    if(fp == NULL)
    {
        exit(1);
    }

    /* Read the output a line at a time - output it. */
    while(fgets(path, sizeof(path)-1, fp) != NULL)
    {
    }

    if((n = match(path, (char*)str1) == 0))
    {
        return true;
    }
    else
    {
        return false;
    }

    pclose(fp);
}



/*
remove_ap(char* ap_name)
This function remove an AP from the list (identified by its name)
*/
bool sim_remove_ap(char *ap_name)
{
    FILE *fp;
    int n;
    char path[1035];
    const char *str1 = "deleted";
    char cmd[50];
    /* Open the command for reading.*/

    sprintf(cmd, "nmcli connection delete %s", ap_name);
    fp = popen(cmd, "r");
    if(fp == NULL)
    {
        exit(1);
    }

    /* Read the output a line at a time - output it. */
    while(fgets(path, sizeof(path)-1, fp) != NULL)
    {
    }

    if((n = match(path, (char *)str1) == 0))
    {
        return true;
    }
    else
    {
        return false;
    }

    pclose(fp);
}



/*
bool Connect_to_internet()
This function tries to connect to internet
*/
bool sim_connect_to_internet()
{
    unsigned char cmd[] = "AT$QCRMCALL=1,1\r\n";
    int len = sizeof(cmd);
    char ret[20];
    int fd;
	char *ip = "";

    fd = sim_open_port();
    if(fd < 0)
    {
		sim_close_port(fd);
        return false;
    }

    tcflush(fd, TCIOFLUSH);
    if(usb_write(fd, cmd, len))
    {
        if((size = usb_read(fd, (unsigned char *)ret, bsize)) < 0)
        {   
            sim_close_port(fd);
            return false;
        }
        else
        {
			if(strstr(ret, "ERROR") != NULL)
			{
				ip = sim_get_ipaddress();
				if(strcmp(ip, "") != 0 || strcmp(ip, "ERROR") != 0)
				{
					sim_close_port(fd);
            		return true;
				}
			}
			
            sprintf((char *)cmd, "dhclient -i eth1");
            if(!runCommand(cmd, ret, 20))
            {
                sim_close_port(fd);
                return false;
            }
            sim_close_port(fd);
            return true;
        }
    }
    else
    {
        sim_close_port(fd);
        return false;
    }
}


/*
char* sim_get_ipaddress()
This function returns the ip address
*/
char* sim_get_ipaddress()
{
    char *cmd = "ifconfig eth1 | grep 'inet addr' | cut -d: -f2 | awk '{print $1}'";
    char *ret = malloc(50);

    if(!runCommand(cmd, ret, 50))
    {
        strcpy(ret, "ERROR");
        return ret;
    }
       
    return ret;     
}



/*
get_signal_strength()
This function gets the strengh of the signal (in dbm)
*/
unsigned int sim_get_signal_strength()
{
    unsigned char cmd[] = "AT+CSQ\r\n";
    int len = sizeof(cmd);
    char *sig_strenght;

    sim_open_port();
    tcflush(fd, TCIOFLUSH);
    if(usb_write(fd, cmd, len))
    {
        if((size = usb_read(fd, sig_strenght, bsize)) < 0)
        {
            return false;
        }
        else
        {
            int signal = atoi(sig_strenght);
            return signal;
        }
    }
    else
    {
        return false;
    }
}



/*
char* get_imsi()
This function gets the IMSI
*/
char* sim_get_imsi()
{
    unsigned char cmd[] = "AT+CIMI\r\n";
    char *ret = malloc(bsize);
    char *imsi = malloc(20);
    int len = sizeof(cmd);
    int fd;

    fd = sim_open_port();
    tcflush(fd, TCIOFLUSH);
    if(usb_write(fd, cmd, len))
    {
        if((size = usb_read(fd, (unsigned char *)ret, bsize)) < 0)
        {
            free(ret);
            sim_close_port(fd);
            strcpy(imsi, "");
            return imsi;
        }
        else
        {
            if(strstr(ret, "ERROR") != NULL)
            {
                free(ret);
                sim_close_port(fd);
                strcpy(imsi, "");
                return imsi;
            }

            char *tmp = strtok((char *)ret, "\n");
            if(tmp != NULL)
            {
                tmp = strtok(NULL, "\n");
            }
            imsi = strdup(tmp);
            free(ret);
            sim_close_port(fd);
            return imsi;
        }
    }
    else
    {
        free(ret);
        sim_close_port(fd);
        strcpy(imsi, "");
        return imsi;
    }
}



/*
char* get_imei()
This function gets the IMEI
*/
char* sim_get_imei()
{
    unsigned char cmd[] = "AT+CGSN\r\n";
    int len = sizeof(cmd);
    char *ret = malloc(bsize);
    char *imei = malloc(20);
    int fd;

    fd = sim_open_port();
    tcflush(fd, TCIOFLUSH);

    if(usb_write(fd, cmd, len))
    {
        tcflush(fd, TCIOFLUSH);
        if((size = usb_read(fd, (unsigned char *)ret, bsize)) < 0)
        {
            free(ret);
            sim_close_port(fd);
            strcpy(imei, "");
            return imei;
        }
        else
        {
            char *tmp = strtok((char *)ret, "\n");
            if(tmp != NULL)
            {
                tmp = strtok(NULL, "\n");
            }
            imei = strdup(tmp);
            free(ret);
            sim_close_port(fd);
            return imei;
        }
    }
    else
    {
        free(ret);
        sim_close_port(fd);
        strcpy(imei, "");
        return imei;
    }
}



/*
    send_sms(char* phone_no, char* message)
    This function send the sms.
 */
bool sim_send_sms(char *phoneno, char *message)
{
    unsigned char cmd[3][30];
    int len[3];
    unsigned char msg[165];
    int lenmsg = sizeof(msg);
    int fd;
    char sms[20];
    char *imsi = "";
    
    for(int i=0; i < 3; i++)
    {
        len[i] = sizeof(cmd[i]);
    }

    if(!sim_card_available())
    {
        return false;
    }

    sprintf(cmd[0], "AT+CMGF=1\r\n");
    sprintf(cmd[1], "AT+CSCS=\"GSM\"\r\n");
    sprintf(cmd[2], "AT+CMGS=\"%s\"\r\n", phoneno);
    sprintf(msg, "%s\x1A", message); //message string should be max of 160 chars

    fd = sim_open_port();
    tcflush(fd, TCIOFLUSH);

    for(int i=0; i < 3; i++)
    {
        tcflush(fd, TCIOFLUSH);
        if(usb_write(fd, cmd[i], len[i]))
        {
            if((size = usb_read(fd, (unsigned char *)sms, bsize)) < 0)
            {
                sim_close_port(fd);
                return false;
            }
            else
            {	
                continue;
            }
        }
        else
        {
            sim_close_port(fd);
            return false;
        }
    }

    if(usb_write(fd, msg, lenmsg))
    {
        if((size = usb_read(fd, (unsigned char *)sms, bsize)) < 0)
        {
            sim_close_port(fd);
            return false;
        }
        else
        {
            sim_close_port(fd);
            return true;
        }
    }
    else
    {
        sim_close_port(fd);
        return false;
    }
}
