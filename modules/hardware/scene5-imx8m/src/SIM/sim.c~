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

#define bsize 256

int fd ;
unsigned char *buf;
int size;

int SIM_openPort(){
    fd=USB_open(SIM_PORT, SIM_Baud_Rate);
	if(fd<0){
		return -1;
	}
  	else {
		return fd;
	}	
}

/* 

bool unlock_sim(int pincode)

This function activates the sim card by using sim pin

*/

bool unlock_sim(int pincode) {
	unsigned char cmd[17];
	snprintf((char*)cmd, sizeof(cmd), "AT+CPIN=%d?;\r\n",pincode);//AT+CPIN="1234?
    int len = sizeof(cmd);
	tcflush(fd, TCIOFLUSH);
    if(USB_write(fd, cmd, len)) {
		if((size=USB_read(fd,buf,bsize))<0) {
			return false;
		} 
		else {
			return true;
		}
	} 
	else {
		return false;
	}
}

/*

Dialup_connect(int pincode)

This function tries to connect to telephone network

*/

bool dialup_connect(unsigned char phone_no[]){
	unsigned char cmd[17];
	snprintf((char*)cmd, sizeof(cmd), "ATD%s;\r\n",phone_no);
	int len = sizeof(cmd);
	tcflush(fd, TCIOFLUSH);
	if(USB_write(fd, cmd, len)) {
		
		if((size=USB_read(fd,buf,bsize))<0){
			return false;
		}
		else{
			return true;
		}
	}
	else{
		return false;
	}	
}

/*

Dialup_disconnect()

This function disconnect from telephone network

*/

bool dialup_disconnect(){
	unsigned char cmd[]="AT+CHUP\r\n";
	int len = sizeof(cmd);
	tcflush(fd, TCIOFLUSH);
	if(USB_write(fd, cmd, len)){
		
		if((size=USB_read(fd,buf,bsize))<0){
			return false;
		}
		else {
			return true;
		}
	}
	else {
		return false;
	}
}

/*

set_AP(struct apn setapn)

This function creates or update an AP (identified by its name)

*/

bool set_ap(struct apn setapn){
	system("nmcli connection delete gprs");
 	FILE *fp;
 	int n;
 	char path[1035];
 	const char* str1 = "successfully added"; 
 	char cmd[50];
  	sprintf(cmd,"nmcli connection add type gsm con-name gprs ifname ttyUSB%d apn %s",SIM_PORT,setapn.apn_name);
  
  	/* Open the command for reading. */
  	fp = popen(cmd, "r");
  		if (fp == NULL) {
    		exit(1);
  		}

  		/* Read the output a line at a time - output it. */
  		while (fgets(path, sizeof(path)-1, fp) != NULL) {
  		}
  
		if((n=match(path, (char*) str1)==0)) {
			return true;
		}
 		else {
			return false;
		} 
	pclose(fp);
}

/*

remove_ap(char* ap_name)

This function remove an AP from the list (identified by its name)

*/

bool remove_ap(char* ap_name){
	FILE *fp;
  	int n;
  	char path[1035];
  	const char* str1 = "deleted";
  	char cmd[50];
  	/* Open the command for reading. */
  	sprintf(cmd,"nmcli connection delete %s",ap_name);
  	fp = popen(cmd, "r");
  	if (fp == NULL) {
    	exit(1);
  	}

 	 /* Read the output a line at a time - output it. */
  	while (fgets(path, sizeof(path)-1, fp) != NULL) {
  	}
  
	if((n=match(path, (char*) str1)==0)) {
		return true;
	}
 	else {
		return false;
	}
	pclose(fp);
}

/*

bool Connect_to_internet()

This function tries to connect to internet

*/

bool connect_to_internet(){
  	FILE *fp;
 	int n;
  	char path[1035];
  	const char* str1 = "Device 'ttyUSB4' successfully activated"; 
  	char cmd[50];
  	/* Open the command for reading. */
  	sprintf(cmd,"nmcli device connect ttyUSB%d",SIM_PORT);
  	fp = popen(cmd,"r");
  	if (fp == NULL) {
    	exit(1);
  	}

  	/* Read the output a line at a time - output it. */
  	while (fgets(path, sizeof(path)-1, fp) != NULL) {
	}
  
	if((n=match(path, (char*) str1)==0)){
		return true;
	}
 	else{
		return false;		
	} 
	pclose(fp);
}

/*

get_signal_strength()

This function gets the strengh of the signal (in dbm)

*/

unsigned int get_signal_strength(){
	unsigned char cmd[]="AT+CSQ\r\n";
	int len = sizeof(cmd);
	char* sig_strenght;
	tcflush(fd, TCIOFLUSH);
	if(USB_write(fd, cmd, len)){
		
		if((size=USB_read(fd,sig_strenght,bsize))<0) {
			return false;
		}
		else{
			int signal = atoi(sig_strenght);
			return signal;
		}
	}
	else{
		return false;
	}			
}

/*

char* get_imsi()

This function gets the IMSI

*/

char* get_imsi(){
	unsigned char cmd[]="AT+CIMI\r\n";
	char *imsi = malloc(20);	
	int len = sizeof(cmd);
	tcflush(fd, TCIOFLUSH);
	if(USB_write(fd, cmd, len)){
		
		if((size=USB_read(fd,imsi,bsize))<0){
			return -1;
		}
		else{
			return imsi;
		}
	}
	else{
		return -1;
	}
}

/*

get_imei()

This function gets the IMEI

*/

char* get_imei(){
	unsigned char cmd[]="AT+CGSN\r\n";
	int len = sizeof(cmd);
	tcflush(fd, TCIOFLUSH);
	char *imei = malloc(20);
	if(USB_write(fd, cmd, len)){
		tcflush(fd, TCIOFLUSH);

		if((size=USB_read(fd,imei,bsize))<0){
			return -1;
		}
		else{		
			return imei;
		}
	}
	else{
		return -1;
	}
}

