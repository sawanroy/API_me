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

#define Success 0
#define Failure -107000

unsigned char* File_Path[1048];
int system(const char *command);
struct bluetoothconfig conf;
/*
*	Internal function used  
*/

int vector_count_bt(vector *v) {
	return v->count;
}

void *vector_get_bt(vector *v, int index) {
	if (index >= v->count) {
		return;
	}

	return v->data[index];
}

void vector_init_bt(vector *v) {
	v->data = NULL;
	v->size = 0;
	v->count = 0;
}

void vector_add_bt(vector *v, void *e) {
		if (v->size == 0) {
		v->size = 30;
		v->data = malloc(sizeof(void*) * v->size);
		memset(v->data, '\0', sizeof(void) * v->size);
	}

	// condition to increase v->data:
	// last slot exhausted
	if (v->size == v->count) {
		v->size *= 2;
		v->data = realloc(v->data, sizeof(void*) * v->size);
	}

	v->data[v->count] = e;
	v->count++;
}

void vector_free_bt(vector *v) {
	free(v->data);
}

int match_bt(char *a, char *b) {
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
	printf("%s",deviceName);
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

bool runCommand_bt( const char* cmd, char* output,int size){
	printf("Entered into runCommand function");
	memset(output,'\0', size);
	int n;
 	char path[1035];
	FILE* fp = popen(cmd,"r");
	if (fp == NULL) {
    		exit(-1);
  		}

  		/* Read the output a line at a time - output it. */
  		while (fgets(path, sizeof(path)-1, fp) != NULL) {
			  printf("%s",path);
			  strcpy(output,path);
		  }
		  
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
			printf("name %s",name);
			printf("devicename %s",deviceName);
			a=strcmp(name, deviceName);       			
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
				if((n=match_bt(path, (char*) deviceName)==0)){
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
				if((n=match_bt(path, (char*) str1)==0)){
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
	if((n=match_bt(path, (char*) deviceName)==0)){
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
		if((n=match_bt(path, (char*) str1)==0)){		
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
	printf("*************** BT SCAN STARTED *********************\n");
    dev_id = hci_get_route(NULL);
	printf("hci_get_route %d\n",dev_id);
    sock = hci_open_dev( dev_id );
	printf("hci_open_dev %d\n",sock);
    if (dev_id < 0 || sock < 0) {
        perror("opening socket");
        return Failure; 
    }
    len  = 8; 
    max_rsp = 255;
    flags = IREQ_CACHE_FLUSH;
	printf("line5\n");
    ii = (inquiry_info*)malloc(max_rsp * sizeof(inquiry_info));
	printf("line6\n %x",ii);
    num_rsp = hci_inquiry(dev_id, len, max_rsp, NULL, &ii, flags);
    printf("line7\n");
	if( num_rsp < 0 ) perror("hci_inquiry");
    printf("line8\n");
	for (i = 0; i < num_rsp; i++) {
		printf("line9\n");
        ba2str(&(ii+i)->bdaddr, addr);
		printf("line10\n");
        memset(name, 0, sizeof(name));
		printf("line11\n");
        if (hci_read_remote_name(sock, &(ii+i)->bdaddr, sizeof(name), name, 0) < 0){
			printf("line12\n");
			return Failure;
			printf("line13\n"); 
        }
		else {
			Bt_scanResult *res = (Bt_scanResult*)malloc(sizeof(Bt_scanResult));
        	res->dev_numb = i;
        	res->dev_name = strdup(name);
			printf("line17%s\n",res->dev_name);
        	vector_add_bt(v, res);
			printf("line18\n");					
		}
    }
    free(ii);
	printf("line19\n");
    close( sock );
	printf("line20\n");
	return num_rsp;
}

/*
*
*	bool bluetooth_set_config(struct bluetoothconfig conf)
*	Configure Bluetooth
*
*/

bool bluetooth_set_config(struct bluetoothconfig conf){
     printf("%d",conf.radioOn);
	 printf("%d",conf.enabled);
	 printf("%s",conf.name);
	 printf("%d",conf.discoverable);

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
	if(BT_setDeviceName(conf.name ,255)){
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
	
	char ret[1024]="";
	char cmd[255];
	char ret1[1024]="";
	char cmd1[255];
	char ret2[1024]="";
	char cmd2[255];
	char ret3[1024]="";
	char cmd3[255];
    sprintf(cmd, "rfkill list bluetooth | grep Soft | awk '{print $3}'");
    if(!runCommand_bt(cmd,ret,2048)){
        //return NULL;
		exit(-1);
    }
	printf("ret %s",ret);
	if(ret == "no"){
		conf.radioOn=true;
	}
	else{
		conf.radioOn=false;
	}
	//read bluetooth is enable or not 
	sprintf(cmd1, "echo 'show' | bluetoothctl | grep Powered | awk '{print $2}'");
    if(!runCommand_bt(cmd1,ret1,2048)){
    	//return NULL;
		exit(0);
    }
	printf("ret1 %s",ret1);
	if(ret1 == "yes"){
		conf.enabled=true;
		printf("radio %s",ret);
	}
	else{
		conf.enabled=false;
		printf("radio %s",ret);
	}
	//discoverable state
	sprintf(cmd2,"echo 'show' | bluetoothctl | grep Discoverable | awk '{print $2}'");
	 if(!runCommand_bt(cmd2,ret2,2048)){
    	//return NULL;
		exit(-1);
    }
	printf("ret2 %d",ret2);
	if(ret2 == "yes"){
		conf.discoverable=true;
	}
	else{
		conf.discoverable=false;
	}
	//bluetooth device name
	sprintf(cmd3,"echo 'show' | bluetoothctl | grep Name | awk '{print $2}'");
	 if(!runCommand_bt(cmd3,ret3,2048)){
        //return NULL;
		exit(-1);
    }
	//printf("ret3 %s",ret3);
	printf("BT_NAME %s",ret3);
      //conf.name=ret3;
	//strcpy(conf.name,ret3);
     
	 return conf;
	 
	// exit(0);
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
    if(!runCommand_bt(cmd,ret,2048)){
		return false;
    }

	return true;
}


