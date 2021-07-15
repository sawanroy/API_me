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
#include <gpio.h>


#define bsize 256



/*Internal function*/
int sim_open_port()
{
    int fd;

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



/*Deactivate sim*/
bool sim_deactivate()
{
    //Reset pin is accesible on hardware side.
    if(write_value_to_output_gpio(3, false))
    {
        usleep(500000); //delay recommended in datasheet
        return (write_value_to_output_gpio(3, true));
    }
    else 
    {
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



/*
set_AP(struct apn setapn)
This function creates or update an AP (identified by its name)
*/
bool sim_set_ap(sim_apn setapn)
{
    char datafield[5][255];
    unsigned char cmd[bsize];
    unsigned char ret[bsize];
    int len = sizeof(cmd);
    int proto = 1;
    int auth = 0;
    int cid = 0;
    int fd;

    if(strcmp(setapn.apnProtocol, "IPV4") == 0)
    {
        proto = 1;
    }
    else if(strcmp(setapn.apnProtocol, " 2") == 0)
    {
        proto = 2;
    }

    if(strcmp(setapn.authentication, "PAP") == 0)
    {
        auth = 1;
    }
    else if(strcmp(setapn.authentication, "CHAP") == 0)
    {
        auth = 2;
    }
    else
    {
        auth = 0;
    }


    fd = sim_open_port();
    if(fd < 0)
    {
        return false;
    }

    //check if apn already exists; only 16 apn can be configured at a time.
    for(int count = 1; count <= 16; count++)
    {
        unsigned char ret2[bsize];
        unsigned char cmd2[50];

        tcflush(fd, TCIOFLUSH);

        sprintf((char *)cmd2, "AT+QICSGP=%d\r\n", count);

        if(usb_write(fd, cmd2, sizeof(cmd2)))
        {
            if(usb_read(fd, ret2, bsize) < 0)
            {
                sim_close_port(fd);
                return false;
            }
            else
            {
                if(strstr((const char *)ret2, "ERROR") != NULL)
                {
                    sim_close_port(fd);
                    return false;
                }
                char *tmp = strtok((char *)ret2, "\n");
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

                                if(strcmp(datafield[0], " 0") == 0)
                                {
                                    //skip if blank data//
                                    cid = count;
                                }
                                if(strcmp(setapn.apn_name, datafield[1]) == 0)
                                {
                                    cid = count;
                                    break;
                                }
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
    
    if(cid)
    {
        sprintf((char *)cmd, "AT+QICSGP=%d,%d,\"%s\",\"%s\",\"%s\",%d\r\n", cid, proto, setapn.apn_name, setapn.username, setapn.password, auth);

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
                    sim_close_port(fd);
                    return false;
                } 

                unsigned char cmdact[100];
                unsigned char retact[bsize];
                sprintf((char *)cmdact, "AT+CGACT=1,%d\r\n", cid);
                if(usb_write(fd, cmdact, sizeof(cmdact)))
                {
                    if(usb_read(fd, retact, bsize) < 0)
                    {
                        sim_close_port(fd);
                        return false;
                    }
                    else
                    {
                        if(strstr((const char *)retact, "ERROR") != NULL)
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
        }
        else
        {
            sim_close_port(fd);
            return false;
        }
    }
    else
    {
        sim_close_port(fd);
        return false;
    }

}



/*list configgured AP*/
bool sim_list_ap(vector *aplist)
{   
    char datafield[5][255];
    int fd;
    bool valid = true;

    fd = sim_open_port();
    if(fd < 0)
    {
		sim_close_port(fd);
        return false;
    }

    tcflush(fd, TCIOFLUSH);

    for(int count = 1; count <= 16; count++)
    {
        unsigned char ret2[bsize];
        unsigned char cmd2[50];
        valid = true;

        tcflush(fd, TCIOFLUSH);

        //Get the datafields using below AT command
        sprintf((char *)cmd2, "AT+QICSGP=%d\r\n", count);

        if(usb_write(fd, cmd2, sizeof(cmd2)))
        {
            if(usb_read(fd, ret2, bsize) < 0)
            {
                sim_close_port(fd);
                return false;
            }
            else
            {
                if(strstr((const char *)ret2, "ERROR") != NULL)
                {
                    sim_close_port(fd);
                    return false;
                }
                char *tmp = strtok((char *)ret2, "\n");
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

                                if(strcmp(datafield[0], " 0") == 0)
                                {
                                    //skip if blank data//
                                    valid = false;;
                                }

                                if(valid)
                                {
                                    //create structure and add to list
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
        }
        else
        {
            sim_close_port(fd);
            return false;
        }
    }
    
    sim_close_port(fd);
    return true;
}



/*
remove_ap(char* ap_name)
This function remove an AP from the list (identified by its name)
*/
bool sim_remove_ap(char *ap_name)
{
    char datafield[5][255];
    unsigned char cmd[100];
    unsigned char ret[bsize];
    int len = sizeof(cmd);
    int cid = 0;
    int fd;

    if(!sim_card_available())
    {
        return false;
    }

    fd = sim_open_port();
    if(fd < 0)
    {
		sim_close_port(fd);
        return false;
    }

    //check if apn name mathches in the apn list
    for(int count = 1; count <= 16; count++)
    {
        unsigned char ret2[bsize];
        unsigned char cmd2[50];

        tcflush(fd, TCIOFLUSH);

        sprintf((char *)cmd2, "AT+QICSGP=%d\r\n", count);

        if(usb_write(fd, cmd2, sizeof(cmd2)))
        {
            if(usb_read(fd, ret2, bsize) < 0)
            {
                sim_close_port(fd);
                return false;
            }
            else
            {
                if(strstr((const char *)ret2, "ERROR") != NULL)
                {
                    sim_close_port(fd);
                    return false;
                }
                char *tmp = strtok((char *)ret2, "\n");
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

                                if(strcmp(ap_name, datafield[1]) == 0) //apn name matched
                                {
                                    cid = count;
                                    break;
                                }
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

    //delete the required apn
    if(cid)
    {
        sprintf((char *)cmd, "AT+CGDCONT=%d\r\n", cid);

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
    else
    {
        sim_close_port(fd);
        return false;
    }
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

    /*run the AT$QCRMCALL at command
    * if success, get ip by dhclient command. Further check if ip address alloted or not.
    * if at command fails, check if already connected by reading ip
    */
    tcflush(fd, TCIOFLUSH);
    if(usb_write(fd, cmd, len))
    {
        if((usb_read(fd, ret, bsize)) < 0)
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
    if(fd < 0)
    {
		sim_close_port(fd);
        return false;
    }

    /*get the rssi data from AT+CSQ*/
    tcflush(fd, TCIOFLUSH);
    if(usb_write(fd, cmd, len))
    {
        if((usb_read(fd, ret, bsize)) < 0)
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
    if(fd < 0)
    {
		sim_close_port(fd);
        return false;
    }

    tcflush(fd, TCIOFLUSH);
    if(usb_write(fd, cmd, len))
    {
        if((usb_read(fd, ret, bsize)) < 0)
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
    if(fd < 0)
    {
		sim_close_port(fd);
        return false;
    }

    tcflush(fd, TCIOFLUSH);

    if(usb_write(fd, cmd, len))
    {
        tcflush(fd, TCIOFLUSH);
        if((usb_read(fd, ret, bsize)) < 0)
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
    if(fd < 0)
    {
		sim_close_port(fd);
        return false;
    }

    tcflush(fd, TCIOFLUSH);

    /*configure the sms setting*/
    for(int i = 0; i < 3; i++)
    {
        tcflush(fd, TCIOFLUSH);
        if(usb_write(fd, (unsigned char *)cmd[i], len[i]))
        {
            if((usb_read(fd, ret, bsize)) < 0)
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

    /*enter the message to be send*/
    if(usb_write(fd, (unsigned char *)msg, lenmsg))
    {
        if((usb_read(fd, ret, bsize)) < 0)
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
