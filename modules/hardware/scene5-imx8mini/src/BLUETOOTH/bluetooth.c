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
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <bluetooth.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <vector.h>

#define Success 0
#define Failure -107000
unsigned char* File_Path[1048];
int system(const char *command);

/*
*	Internal function used  
*/


int match(char *a, char *b) {
	int position = 0;
	char *x, *y;
	x = a;
	y = b;
	while(*a) {
		while(*x==*y) {
			x++;
			y++;
			if(*x=='\0'||*y=='\0')
			break;	
		}
		if(*y=='\0')
		break;
		a++;
		position++;
		x = a;
		y = b;
	}
	if(*a)
	return 0;
	else
	return -1;
}

int BT_getDeviceName(unsigned char *deviceName, int size ) {
	int dev_id, sock, len;
	dev_id = hci_get_route(NULL);
	sock = hci_open_dev( dev_id );
	if (dev_id < 0 || sock < 0) {
		perror("opening socket");
		return Failure;	
	}
	if (hci_read_local_name(sock, size, (char*)deviceName, 1000) < 0) {
		fprintf(stderr, "Can't read local name on hci%d: %s (%d)\n",
		dev_id, strerror(errno), errno);
		return Failure;
	} 
	else {
		int s = strlen((char*)deviceName);
	}
	close( sock );
	return len;
}


int BT_setDeviceName(unsigned char *deviceName, int size )
{
	int dev_id, sock, len;
	dev_id = hci_get_route(NULL);
	sock = hci_open_dev( dev_id );
	if (dev_id < 0 || sock < 0) {
		perror("opening socket");
		return -1;
	}
	if (hci_write_local_name(sock,(const char*)deviceName, size) < 0){
		fprintf(stderr, "Can't change local name on hci%d: %s (%d)\n",
		dev_id, strerror(errno), errno);
		return -1;
	}	 
	else {
		int s = strlen((char*)deviceName);
	}
	close( sock );
	return len;
}


bool BT_visibilityOn() {
	FILE *fp;
	char path[100];		
	fp = popen("echo 'discoverable on' | bluetoothctl", "r");
	if (fp == NULL) {
		return false;
	}	
	/* Read the output a line at a time - output it. */
	while (fgets(path, sizeof(path)-1, fp) != NULL) {   
	}
	/* close */
	pclose(fp);		       		
	return true;
}

bool BT_visibilityOff(){
	FILE *fp;
	char path[100];
	fp = popen("echo 'discoverable off' | bluetoothctl", "r");
	if (fp == NULL) {
		return false;
	}	
	/* Read the output a line at a time - output it. */
	while (fgets(path, sizeof(path)-1, fp) != NULL) {   
	}
	/* close */
	pclose(fp);		       		
	return true;
}

int BT_getLocalAddress(unsigned char *MAC, int size) {
    bdaddr_t *bdaddr=NULL;
    int dev_id, sock, max_rsp=255;
    dev_id = hci_get_route(NULL);
   	sock = hci_open_dev( dev_id );
    if (dev_id < 0 || sock < 0) {
        perror("opening socket");
        return Failure;
    }
	bdaddr = (bdaddr_t*)malloc(max_rsp * sizeof(bdaddr_t));
        if (hci_read_bd_addr(sock, bdaddr, size) >= 0){
			ba2str(bdaddr, (char*)MAC);
			int s = strlen((char*)MAC);
			return s;
    	}
		else {
			return Failure;
		}
    close(sock);
}

int BT_setSaveFilePath(unsigned char *path, int size)
{
	const char* folder;
    folder = (const char*)path;
    struct stat sb;
    if (stat(folder, &sb) == 0 && S_ISDIR(sb.st_mode)) {	
		snprintf( (char*)File_Path,sizeof(File_Path),"%s",folder );
		return Success;
    }
    else {
		return Failure;
    }

}

/*
*	bool bluetooth_on()
*	Activate Bluetooth
*/

bool bluetooth_on()
{

	int a = system("hciconfig hci0 up");	

		if(!a){
			return true;
		} else {
				return false;
			 }
}

/*
*	bool bluetooth_off()
*	Deactivate Bluetooth
*/

bool bluetooth_off()
{

	int a = system("hciconfig hci0 down");
	if(!a){
		return true;
	} else {
		return false;
	}
}

/*
*
*	bool bluetooth_pair_to_device(unsigned char *deviceName, int size )
*	Pair device with another by bluetooth
*
*/

bool bluetooth_pair_to_device(unsigned char *deviceName, int size ) {
	FILE *fp;
	char path[1035];
	char channel[1035];
	char cmd[50];
	inquiry_info *ii = NULL;
	int max_rsp, num_rsp;
	int dev_id, sock, len, flags, flag;
	int i,n,a;
	char addr[50] = { 0 };
	char name[248] = { 0 };
	const char* str1 = "Already Exists";
	dev_id = hci_get_route(NULL);
	sock = hci_open_dev( dev_id );
	if (dev_id < 0 || sock < 0) {
		perror("opening socket");
		return false;	
	}
	len  = 8;
	max_rsp = 255;
	flags = IREQ_CACHE_FLUSH;
	ii = (inquiry_info*)malloc(max_rsp * sizeof(inquiry_info)); 
	num_rsp = hci_inquiry(dev_id, len, max_rsp, NULL, &ii, flags);
	if( num_rsp < 0 ) perror("hci_inquiry");
	for (i = 0; i < num_rsp; i++) {
		ba2str(&(ii+i)->bdaddr, addr);
		memset(name, 0, sizeof(name));
		if (hci_read_remote_name(sock, &(ii+i)->bdaddr, sizeof(name), name, 0) < 0){
			return false;
		} 
		else {
			a=strcmp(name, (const char*)deviceName);       			
			if (!a){   // if name mached                
				flag =1;
				fp = popen("timeout 20 bt-adapter -d", "r");
				if (fp == NULL) {
					return false;
				}
				/* Read the output a line at a time - output it. */
				while (fgets(channel, sizeof(channel)-1, fp) != NULL) {    
				}
				/* close */
				pclose(fp);			
				sprintf(cmd,"bt-device -l | grep '%s'",deviceName);
				fp = popen(cmd, "r");
				if (fp == NULL) {
		
					return false;
				}	
				/* Read the output a line at a time - output it. */
				while (fgets(path, sizeof(path)-1, fp) != NULL) {    
				}
				if((n=match(path, (char*) deviceName)==0)){
				}
				else {
					pclose(fp);
					return false;		
				}
				/* close */
				pclose(fp);
				sprintf(cmd,"echo 'yes' | bt-device -c %s",addr);
				fp = popen(cmd, "r");
				if (fp == NULL) {
		
					return false;
				}	
				/* Read the output a line at a time - output it. */
				while (fgets(path, sizeof(path)-1, fp) != NULL) {    
				}
				if((n=match(path, (char*) str1)==0)){
					pclose(fp);
					return false;
				}
				/* close */
				pclose(fp);	
			}
		}

	}
	if (flag==0){
		return false;
	}
	free( ii );
	close( sock );
	return 0;
}

/*
*
*	bool bluetooth_unpair_to_device(unsigned char *deviceName, int size )
*	Unpair device with another bt bluetooth
*
*/

bool bluetooth_unpair_to_device(unsigned char *deviceName, int size ) {
	FILE *fp;
	char path[1035];
	char cmd[50];
	int n, flag=0;
	const char* str1 = "Done";                          		
	sprintf(cmd,"bt-device -l | grep '%s'",deviceName);
	fp = popen(cmd, "r");
	if (fp == NULL) {
		return false;
	}	
	/* Read the output a line at a time - output it. */
	while (fgets(path, sizeof(path)-1, fp) != NULL) {    
	}
	printf("yus :%s", path);
	if((n=match(path, (char*) deviceName)==0)){
		flag=1;
	}
	else {
		pclose(fp);
		return false;	
	}
	/* close */
	pclose(fp);       
	if(flag==1){
		sprintf(cmd,"bt-device -r '%s'",deviceName);
		fp = popen(cmd, "r");
		if (fp == NULL) {

			return false;	
		}	
		/* Read the output a line at a time - output it. */
		while (fgets(path, sizeof(path)-1, fp) != NULL) {    
		}
		if((n=match(path, (char*) str1)==0)){		
		}
		else {
			pclose(fp);
			return false;
		}
		/* close */
		pclose(fp);        
	}
	return true;
} 

/*
*
*	int bluetooth_scan( vector* v )
*	Scan for bluetooth devices
*
*/

int bluetooth_scan( vector* v ) {
    inquiry_info *ii = NULL;
    int max_rsp, num_rsp;
    int dev_id, sock, len, flags;
    int i;
    char addr[248] = { 0 };
    char name[248] = { 0 };
    dev_id = hci_get_route(NULL);
    sock = hci_open_dev( dev_id );
    if (dev_id < 0 || sock < 0) {
        perror("opening socket");
        return Failure;
    }
    len  = 8;
    max_rsp = 255;
    flags = IREQ_CACHE_FLUSH;
    ii = (inquiry_info*)malloc(max_rsp * sizeof(inquiry_info));
    num_rsp = hci_inquiry(dev_id, len, max_rsp, NULL, &ii, flags);
    if( num_rsp < 0 ) perror("hci_inquiry");
    for (i = 0; i < num_rsp; i++) {
        ba2str(&(ii+i)->bdaddr, addr);
        memset(name, 0, sizeof(name));
        if (hci_read_remote_name(sock, &(ii+i)->bdaddr, sizeof(name), name, 0) < 0){
			return Failure;
        }
		else {
			Bt_scanResult *res = (Bt_scanResult*)malloc(sizeof(Bt_scanResult));
        	res->dev_numb = i;
        	res->dev_name = strdup(name);
        	vector_add(v, res);					
		}
    }
    free( ii );
    close( sock );
	return num_rsp;
}

/*
*
*	bool bluetooth_set_config(struct bluetoothconfig conf)
*	Configure Bluetooth
*
*/

bool bluetooth_set_config(struct bluetoothconfig conf){
	if(conf.radioOn){
		int a = system("rfkill unblock bluetooth");	
		if(!a){
			return true;
		}
		else {
			return false;
		}
	}
	else{
		int a = system("rfkill block bluetooth");	
		if(!a){
			return true;
		} 
		else {
			return false;
		}
	}
	if(conf.enabled){
		return bluetooth_on();
	}
	else{
		return bluetooth_off();
	}
	if(conf.discoverable){
		return BT_visibilityOn();
	}
	else{
		return BT_visibilityOff();
	}	
	if(BT_setDeviceName(conf.name ,20)){
		return true;
	}else{
		return false;
	}
}

/*
*
*	struct bluetoothconfig bluetooth_get_config()
*	Get Bluetotth configuration
*
*/

struct bluetoothconfig bluetooth_get_config(){
	//reading rfkill
	struct bluetoothconfig conf;
	char ret[1024]="";
	char cmd[255];
    conf.radioOn = sprintf(cmd, "rfkill list bluetooth | grep Soft | awk '{print $3}'");
    if(!runCommand(cmd,ret,2048)){
        //return NULL;
    }
	//read bluetooth is enable or not 
	conf.enabled= sprintf(cmd, "echo 'show' | bluetoothctl | grep Powered | awk '{print $2}'");
    if(!runCommand(cmd,ret,2048)){
    	//return NULL;
    }
	//discoverable state
	conf.discoverable=sprintf(cmd,"echo 'show' | bluetoothctl | grep Discoverable | awk '{print $2}'");
	 if(!runCommand(cmd,ret,2048)){
    	//return NULL;
    }
	//bluetooth device name
	conf.name=sprintf(cmd,"echo 'show' | bluetoothctl | grep Name | awk '{print $2}'");
	 if(!runCommand(cmd,ret,2048)){
        //return NULL;
    }
	return conf;
}

/*
*
*	bool bluetooth_connect_to_device(char *name)
*	Connect the device to another by bluetooth
*	
*/

bool bluetooth_connect_to_device(char *name){
	char ret[1024]="";
	char cmd[255];
	sprintf(cmd, "echo 'connect %s ' | bluetoothctl",name);
    if(!runCommand(cmd,ret,2048)){
		return false;
    }
	return true;
}

