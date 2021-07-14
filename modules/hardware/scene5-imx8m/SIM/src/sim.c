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



/*Internal function*/
int get_dbm(unsigned int rssi)
{
    int dbm = (int)rssi;

    if(rssi >= 2 || rssi <= 30)
    {
        dbm = 2*(rssi-2)-109;
    }
    
    return dbm;
}



/*
bool sim_unlock(int pincode)
This function activates the sim card by using sim pin
*/
bool sim_unlock(int pincode)
{
    unsigned char cmd[17];
    unsigned char ret[bsize];
    int len = sizeof(cmd);
    int fd;
    bool status = true;

    snprintf((char *)cmd, sizeof(cmd), "AT+CPIN=%d\r\n", pincode);//AT+CPIN="1234"
    fd = sim_open_port();
    tcflush(fd, TCIOFLUSH);
    if(usb_write(fd, cmd, len))
    {
        if(usb_read(fd, ret, bsize) < 0)
        {
            sim_close_port(fd);
            return false;
        }
        else
        {
            if(strstr((const char *)ret, "ERROR") != NULL)
            {
                status = false;
            } 
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
Dialup_connect(int pincode)
This function tries to connect to telephone network
*/
bool sim_dialup_connect(unsigned char phone_no[])
{
    unsigned char cmd[17];
    unsigned char ret[bsize];
    int len = sizeof(cmd);
    int fd;
    bool status = true;

    if(!sim_card_available())
    {
        return false;
    }

    snprintf((char *)cmd, len, "ATD%s;\r\n", phone_no);
    fd = sim_open_port();
    tcflush(fd, TCIOFLUSH);
    if(usb_write(fd, cmd, len))
    {
        if(usb_read(fd, ret, bsize) < 0)
        {
            sim_close_port(fd);
            return false;
        }
        else
        {
            if(strstr((const char *)ret, "ERROR") != NULL)
            {
                status = false;
            } 
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
Dialup_disconnect()
This function disconnect from telephone network
*/
bool sim_dialup_disconnect()
{
    unsigned char cmd[] = "AT+CHUP\r\n";
    unsigned char ret[bsize];
    int len = sizeof(cmd);
    int fd;
    bool status = true;

    if(!sim_card_available())
    {
        return false;
    }

    fd = sim_open_port();
    tcflush(fd, TCIOFLUSH);
    if(usb_write(fd, cmd, len))
    {
        if(usb_read(fd, ret, bsize) < 0)
        {
            sim_close_port(fd);
            return false;
        }
        else
        {
            if(strstr((const char *)ret, "ERROR") != NULL)
            {
                status = false;
            } 
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



/*Internal function*/
int sim_file_open(char *filename)
{
    unsigned char cmd[255];
    unsigned char ret[bsize];
    int filehandle;

    sprintf(cmd, "AT+QFOPEN=\"%s\",0\r\n", filename);
    printf("cmd:%s\n",cmd);
    if(usb_write(fd, cmd, sizeof(cmd)))
    {
        if(usb_read(fd, ret, bsize) < 0)
        {
            printf("log3\n");
            return false;
        }
        else
        {
            printf("%s\n", ret);
            if(strstr((const char *)ret, "ERROR") != NULL)
            {
                return false;
            }
            //AT response: 1st line blank, so take data from 2nd line
            char *tmp = strtok((char *)ret, "\n");
            //printf("tmp1:%s\n", tmp);
            if(tmp != NULL)
            {   
                tmp = strtok(NULL, "\n");
            }
            char *tmp2 = strtok(tmp, ":");
            if(tmp != NULL)
            {
                tmp2 = strtok(NULL, ":");
            }
            filehandle = atoi(tmp2);
            //printf("filehandle = %d\n",filehandle);
            return filehandle;
        }
    }
    else
    {
        return false;
    }
}



/*Internal function*/
bool sim_file_close(int filehandle)
{
    unsigned char cmd[25];
    unsigned char ret[255];

    sprintf(cmd, "AT+QFCLOSE=%d\r\n", filehandle);
    printf("cmd4:%s\n",cmd);
    if(usb_write(fd, cmd, sizeof(cmd)))
    {
        if(usb_read(fd, ret, bsize) < 0)
        {
            return false;
        }
        else
        {
            printf("ret:%s\n",ret);
            if(strstr((const char *)ret, "ERROR") != NULL)
            {
                return false;
            }
            else
            {
                return true;
            }
        }
    }
    else
    {
        return false;
    }
}



/*
set_AP(struct apn setapn)
This function creates or update an AP (identified by its name)
*/
bool sim_set_ap(sim_apn setapn)
{
    // unsigned char ret[bsize];
    // unsigned char ret2[bsize];
    // int fd;
    // bool status = true;
    // unsigned char cmd[100];
    // unsigned char cmd2[100];
    // int protocol = 1;
    // int auth = 0;

    // int filehandle;

    // fd = sim_open_port();
    // if(fd < 0)
    // {
	// 	sim_close_port(fd);
    //     return false;
    // }

    // int len = strlen(setapn.apn_name) + strlen(setapn.username) + strlen(setapn.password) + strlen(setapn.authentication) + strlen(setapn.apnProtocol) + strlen(setapn.mcc) + strlen(setapn.mnc);
    // unsigned char cmddata[len+7];    
    // sprintf((char *)cmddata, "%s,%s,%s,%s,%s,%s,%s", setapn.apn_name, setapn.username, setapn.password, setapn.authentication, setapn.apnProtocol, setapn.mcc, setapn.mnc);
    
    // tcflush(fd, TCIOFLUSH);
    
    // filehandle = sim_file_open(setapn.apn_name);

    // sprintf(cmd, "AT+QFWRITE=%d,%d,2\r\n", filehandle, sizeof(cmddata));
    // if(usb_write(fd, cmd, sizeof(cmd)))
    // {
    //     if(usb_read(fd, ret, bsize) < 0)
    //     {
    //         printf("log6\n");
    //         sim_file_close(filehandle);
    //         sim_close_port(fd);
    //         return false;
    //     }
    //     else
    //     {
    //         if(strstr((const char *)ret, "ERROR") != NULL)
    //         {
    //             printf("log7\n");
    //             sim_file_close(filehandle);
    //             sim_close_port(fd);
    //             return false;
    //         }
    //         usleep(1000000);
    //         if(strstr((const char *)ret, "CONNECT") != NULL)
    //         {
    //             if(usb_write(fd, cmddata, sizeof(cmddata)))
    //             {
    //                 usleep(2000000);
    //                 if(usb_read(fd, ret, bsize) < 0)
    //                 {
    //                     printf("log8\n");
    //                     sim_file_close(filehandle);
    //                     sim_close_port(fd);
    //                     return false;
    //                 }
    //                 else
    //                 {
    //                     printf("ret1:%s\n",ret);
    //                     if(strstr((const char *)ret, "ERROR: 421") != NULL || strstr((const char *)ret, "OK") != NULL)
    //                     {
    //                         printf("log11\n");
    //                         status = true;
    //                     }
    //                     else
    //                     {
    //                         printf("log10\n");
    //                         status = false;
    //                         sim_file_close(filehandle);
    //                         sim_close_port(fd);
    //                         return status;
    //                     }
    //                 }
    //             }
    //         }
    //     }
    // }
    // else
    // {
    //     printf("log9\n");
    //     sim_file_close(filehandle);
    //     sim_close_port(fd);
    //     return false;
    // }

    // if(!sim_file_close(filehandle))
    // {
    //     sim_close_port(fd);
    //     return false;
    // }
    
    // if(strcmp(setapn.authentication, "PAP") == 0)
    // {
    //     auth = 1;
    //     printf("PAP\n");
    // }
    // else if(strcmp(setapn.authentication, "CHAP") == 0)
    // {
    //     auth = 2;
    //     printf("CHAP\n");
    // }

    // if(strcmp(setapn.apnProtocol, "IPV4") == 0)
    // {
    //     protocol = 1;
    //     printf("IPV4\n");
    // }
    // else if(strcmp(setapn.apnProtocol, "IPV4V6") == 0)
    // {
    //     protocol = 2;
    //     printf("IPV4V6\n");
    // }

    // sprintf(cmd2, "AT+QICSGP=1,%d,\"%s\",\"%s\",\"%s\",%d\r\n", protocol, setapn.apn_name, setapn.username, setapn.password, auth);
    // printf("cmd2:%s\n",cmd2);
    // if(usb_write(fd, cmd2, sizeof(cmd)))
    // {
    //     if(usb_read(fd, ret2, bsize) < 0)
    //     {
    //         sim_close_port(fd);
    //         return false;
    //     }
    //     else
    //     {
    //         printf("ret:%s\n",ret2);
    //         if(strstr((const char *)ret2, "ERROR") != NULL)
    //         {
    //             sim_close_port(fd);
    //             return false;
    //         }
    //         else
    //         {
    //             sim_close_port(fd);
    //             return true;
    //         }
    //     }
    // }
    // else
    // {
    //     sim_close_port(fd);
    //     return false;
    // }
    
}



/*list configgured AP*/
bool sim_list_ap(vector *aplist)
{
    unsigned char cmd[100];
    unsigned char cmd2[100];
    unsigned char ret[bsize];
    char datafield[5][255];
    int id = 0;
    int fd;

    fd = sim_open_port();
    if(fd < 0)
    {
		sim_close_port(fd);
        return false;
    }

    sprintf(cmd, "AT+CGDCONT?\r\n");
    printf("cmd:%s\n", cmd);

    tcflush(fd, TCIOFLUSH);

    if(usb_write(fd, cmd, sizeof(cmd)))
    {
        if(usb_read(fd, ret, bsize) < 0)
        {
            sim_close_port(fd);
            return false;
        }
        else
        {
            printf("ret:%s\n",ret);
            if(strstr((const char *)ret, "ERROR") != NULL)
            {
                sim_close_port(fd);
                return false;
            }
            
            char *tmp = strtok(ret, "\n");
            while(tmp != NULL)
            {
                if(strstr(tmp, "+CGDCONT") != NULL)
                {
                    id++;
                }

                tmp = strtok(NULL, "\n");
            }
        }
    }
    else
    {
        sim_close_port(fd);
        return false;
    }

    for(int count = 1; count <= id; count++)
    {
        unsigned char ret2[bsize];
        unsigned char cmd2[50];

        tcflush(fd, TCIOFLUSH);

        sprintf(cmd2, "AT+QICSGP=%d\r\n", count);
        if(usb_write(fd, cmd2, sizeof(cmd)))
        {
            

            if(usb_read(fd, ret2, bsize) < 0)
            {
                sim_close_port(fd);
                return false;
            }
            else
            {
                printf("%s\n",ret2);
                if(strstr((const char *)ret2, "ERROR") != NULL)
                {
                    sim_close_port(fd);
                    return false;
                }
                char *tmp = strtok(ret2, "\n");
                if(tmp != NULL)
                {
                    tmp = strtok(NULL, "\n");
                    if(tmp != NULL)
                    {
                        char *tp = strtok(tmp, ":");
                        if(tp != NULL)
                        {
                            tp = strtok(NULL, ":");
                            if(tp != NULL)
                            {
                                int i = 0;
                                char *td = strtok(tp, ",");
                                while(td != NULL)
                                {
                                    strcpy(datafield[i], td);
                                    td = strtok(NULL, ",");
                                    i++;
                                }

                                for(int j = 1; j <= 3; j++)
                                {
                                    char *tc = strtok(datafield[j], "\"");
                                    if(tc == NULL)
                                    {
                                        strcpy(datafield[j], "");
                                    }
                                    else
                                    {
                                        strcpy((char *)datafield[j], tc);
                                    }
                                }

                                sim_apn *getapn = (sim_apn *)malloc(sizeof(sim_apn));
                                getapn->apnProtocol = strdup(datafield[0]);
                                getapn->apn_name = strdup(datafield[1]);
                                getapn->username = strdup(datafield[2]);
                                getapn->password = strdup(datafield[3]);
                                getapn->authentication = strdup(datafield[4]);
                                
                                if(strcmp(getapn->apnProtocol, " 1") == 0)
                                {
                                    strcpy(getapn->apnProtocol, "IPV4");
                                }
                                else if(strcmp(getapn->apnProtocol, " 2") == 0)
                                {
                                    strcpy(getapn->apnProtocol, "IPV4V6");
                                }

                                if(strstr(getapn->authentication, "0") != NULL)
                                {
                                    strcpy(getapn->authentication, "NONE");
                                }
                                else if(strstr(getapn->authentication, "1") != NULL)
                                {
                                    strcpy(getapn->authentication, "PAP");
                                } 
                                else if(strstr(getapn->authentication, "2") != NULL)
                                {
                                    strcpy(getapn->authentication, "CHAP");
                                } 

                                vector_add(aplist, getapn);
                            }
                        }
                    }
                }
            }
        }
        else
        {
            sim_close_port(fd);
            return false;
        }
    }
    
    return true;
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
    unsigned char ret[bsize];
    int len = sizeof(cmd);
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
        if((size = usb_read(fd, ret, bsize)) < 0)
        {   
            sim_close_port(fd);
            printf("log1\n");
            return false;
        }
        else
        {
			if(strstr((const char *)ret, "ERROR") != NULL)
			{
				ip = sim_get_ipaddress();
				if(strcmp(ip, "") == 0 || strstr(ip, "ERROR") != NULL)
				{
					sim_close_port(fd);
            		return false;
				}
			}
			
            sprintf((char *)cmd, "dhclient -i eth1");
            if(!runCommand((const char *)cmd, (char *)ret, 20))
            {
                sim_close_port(fd);
                return false;
            }
            ip = sim_get_ipaddress();
            if(strcmp(ip, "") == 0 || strstr(ip, "ERROR") != NULL)
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
    const char *cmd = "ifconfig eth1 | grep 'inet addr' | cut -d: -f2 | awk '{print $1}'";
    char *ret = malloc(50);

    if(!runCommand(cmd, ret, 50))
    {
        strcpy(ret, "ERROR");
        return ret;
    }
       
    return ret;     
}



/*
int sim_get_signal_strength()
This function gets the strengh of the signal (in dbm)
*/
int sim_get_signal_strength()
{
    unsigned char cmd[] = "AT+CSQ\r\n";
    unsigned char ret[bsize];
    int dbm = 99;
    unsigned int rssi = 0;
    int len = sizeof(cmd);
    int fd;

    fd = sim_open_port();
    tcflush(fd, TCIOFLUSH);
    if(usb_write(fd, cmd, len))
    {
        if((size = usb_read(fd, ret, bsize)) < 0)
        {
            sim_close_port(fd);
            return dbm;
        }
        else
        {
            if(strstr((const char *)ret, "ERROR") != NULL)
            {
                sim_close_port(fd);
                return dbm;
            }

            char *tmp = strtok((char *)ret, "\n");
            if(tmp != NULL)
            {   
                tmp = strtok(NULL, "\n");
            }
            char *tmp2 = strtok(tmp, ":");
            if(tmp2 != NULL)
            {
                tmp2 = strtok(NULL, ",");
            }

            rssi = atoi(tmp2);
            dbm = get_dbm(rssi);
            sim_close_port(fd);
            return dbm;
        }
    }
    else
    {
        sim_close_port(fd);
        return dbm;
    }
}



/*
char* get_imsi()
This function gets the IMSI
*/
char* sim_get_imsi()
{
    unsigned char cmd[] = "AT+CIMI\r\n";
    unsigned char ret[bsize];
    char *imsi = malloc(20);
    int len = sizeof(cmd);
    int fd;

    fd = sim_open_port();
    tcflush(fd, TCIOFLUSH);
    if(usb_write(fd, cmd, len))
    {
        if((size = usb_read(fd, ret, bsize)) < 0)
        {
            sim_close_port(fd);
            strcpy(imsi, "");
            return imsi;
        }
        else
        {
            if(strstr((const char *)ret, "ERROR") != NULL)
            {
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
            sim_close_port(fd);
            return imsi;
        }
    }
    else
    {
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
    unsigned char ret[bsize];
    int len = sizeof(cmd);
    char *imei = malloc(20);
    int fd;

    fd = sim_open_port();
    tcflush(fd, TCIOFLUSH);

    if(usb_write(fd, cmd, len))
    {
        tcflush(fd, TCIOFLUSH);
        if((size = usb_read(fd, ret, bsize)) < 0)
        {
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
            sim_close_port(fd);
            return imei;
        }
    }
    else
    {
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
    char cmd[3][30];
    char msg[165];
    unsigned char ret[bsize];
    int len[3];
    int lenmsg = sizeof(msg);
    int fd;
    
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

    for(int i = 0; i < 3; i++)
    {
        tcflush(fd, TCIOFLUSH);
        if(usb_write(fd, (unsigned char *)cmd[i], len[i]))
        {
            if((size = usb_read(fd, ret, bsize)) < 0)
            {
                sim_close_port(fd);
                return false;
            }
            else
            {	
                if(strstr((const char *)ret, "ERROR") != NULL)
                {
                    sim_close_port(fd);
                    return false;
                }

                continue;
            }
        }
        else
        {
            sim_close_port(fd);
            return false;
        }
    }

    if(usb_write(fd, (unsigned char *)msg, lenmsg))
    {
        if((size = usb_read(fd, ret, bsize)) < 0)
        {
            sim_close_port(fd);
            return false;
        }
        else
        {
            if(strstr((const char *)ret, "ERROR") != NULL)
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
