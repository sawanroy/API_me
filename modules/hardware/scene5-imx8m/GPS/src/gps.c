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
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <termios.h>
#include <sys/file.h>
#include <time.h>
#include <sys/types.h>  
#include <sys/stat.h>
#include <getopt.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <linux/watchdog.h>
#include <serial.h>
#include <gps.h>

/*
	open_port()
	Open GPS serial link

*/

int open_port() {
    FILE* fd;
    fd=serial_open(GPS_PORT,GPS_Baud_Rate);
	if(fd<0) {
		return  -1;
	}
    	else {
		return fd;
	}
}
/*

	state_gps(int filedescriptor)
	State of GPS

*/
int state_gps(int filedescriptor) {
    char *buf2;
    unsigned char *buf[255];
   int size=sizeof(buf);
   int Timeout=100000;
   if(filedescriptor>0)
	{
		int usbrd;
		usbrd=serial_read(filedescriptor, buf, size,Timeout);
		printf("serial_read %d\n",usbrd);
   }  
   FILE * fPtr;
     fPtr = fopen("/tmp/status.txt", "w");


    /* fopen() return NULL if last operation was unsuccessful */
    if(fPtr == NULL)
    {
        /* File not created hence exit */
        printf("Unable to create file.\n");
        exit(EXIT_FAILURE);
    }
    fputs(buf, fPtr);
    fclose(fPtr);
    FILE * fP;
     fP = fopen("/tmp/status.txt", "r");

   int line_num = 1;
	int find_result = 0;
	char temp[10000];

    while(fgets(temp, 10000, fP ) != NULL){
      // printf("line 2\n%s",temp);
		if((strstr(temp, "$PSTMANTENNASTATUS")) != NULL) {

			printf("A match found on line: %d\n", line_num);
			printf("\n%s\n", temp);
         strcpy(buf2,temp);
			find_result++;
		}
		line_num++;
       
    }
    printf("\n%s\n", buf2);
	if(find_result == 0) {
		printf("\n didnt get the data \n");
      return -1;
	}  
   fclose(fP);
   int del = remove("/tmp/status.txt");
    return 0;
}

/*

	char* read_data_gprmc(int filedescriptor)
	output full GPRMC of 1 trame

*/

char* read_data_gprmc(int filedescriptor) { 
    char *buf_temp = malloc(60);
    unsigned char *buf[2000];
    int size=sizeof(buf);
    int Timeout=10000;
    if(filedescriptor>0) {
		int usbrd;
		usbrd=serial_read(filedescriptor, buf, size,Timeout);
    }  
    FILE * fPtr;
    fPtr = fopen("/tmp/gprmc.txt", "w");
    /* fopen() return NULL if last operation was unsuccessful */
    if(fPtr == NULL) {
        /* File not created hence exit */
        printf("Unable to create file.\n");
        exit(EXIT_FAILURE);
    }
    fputs(buf, fPtr);
    fclose(fPtr);
    FILE * fP;
    fP = fopen("/tmp/gprmc.txt", "r");

    int line_num = 1;
	int find_result = 0;
	char temp[10000];

    while(fgets(temp, 10000, fP ) != NULL){
      // printf("line 2\n%s",temp);
		if((strstr(temp, "$GPRMC")) != NULL) {
			printf("A match found on line: %d\n", line_num);
			printf("\n%s\n", temp);
         strcpy(buf_temp,temp);
			find_result++;
		}
		line_num++;
       
    }
    printf("\n%s\n", buf_temp);
	if(find_result == 0) {
		printf("\n didnt get the data \n");
      return -1;
	}  
   fclose(fP);
   int del = remove("/tmp/gprmc.txt");
    return buf_temp;
}

/*

	char* read_data_gprmc_parse(int filedescriptor, int gprmc_index)
	extract part of GPRMC of 1 trame

*/

char* read_data_gprmc_parse(int filedescriptor, int gprmc_index){
   char *buf_tmp = malloc(100);
   int i;
   char *str1;
   char *gpsdata[14][14];
   buf_tmp = read_data_gprmc(filedescriptor);
   char *original;
   original = strdup(buf_tmp);
   while ((str1 = strsep(&original,","))!= NULL) {
   // loop[i] = strdup(str1);
    strcpy(gpsdata[i], str1);
    printf("Token %d: %s \n", i,str1);
    i++;
}
    printf("GPS return data %s ", gpsdata[gprmc_index]);
    strcpy(buf_tmp,gpsdata[gprmc_index]);
    return buf_tmp;


}

/*
	char* read_data_gpgga(int filedescriptor)
	output full GPGGA of 1 trame

*/

char* read_data_gpgga(int filedescriptor) {
    unsigned char *buf_tmp = malloc(100);
    unsigned char *buf[2000];
    int size=sizeof(buf);
    int Timeout=100000;
    FILE *fPtr;
    if(filedescriptor>0) {
		int usbrd;
		usbrd=serial_read(filedescriptor, buf, size,Timeout);
    }  
    fPtr = fopen("/tmp/gpgga.txt", "w");
    /* fopen() return NULL if last operation was unsuccessful */
    if(fPtr == NULL) {
        /* File not created hence exit */
        printf("Unable to create file.\n");
        exit(EXIT_FAILURE);
    }
    fputs(buf, fPtr);
    fclose(fPtr);
    FILE * fP;
    fP = fopen("/tmp/gpgga.txt", "r");
    int line_num = 1;
	int find_result = 0;
	char temp[10000];

    while(fgets(temp, 10000, fP ) != NULL) {
      // printf("line 2\n%s",temp);
		if((strstr(temp, "$GPGGA")) != NULL) {
            strcpy(buf_tmp,temp);
			find_result++;
		}
		line_num++;   
    }

	if(find_result == 0) {
		printf("\n didnt get the data, check for port  \n");
        return -1;
	}  
   fclose(fP);
   int del = remove("/tmp/gpgga.txt");   
   return buf_tmp;
} 

/*

	char* read_data_gpgga_parse(int filedescriptor, int gpgga_index)
	extract part of GPGGA of 1 trame

*/

char* read_data_gpgga_parse(int filedescriptor, int gpgga_index) {
    char *buf_tmp = malloc(100);
    char *str1;
    int i;
    char *gpsdata[14][14];
    buf_tmp = read_data_gpgga(filedescriptor);
    char *original;
    original = strdup(buf_tmp);
    
    while ((str1 = strsep(&original,","))!= NULL) {
    // loop[i] = strdup(str1);
        strcpy(gpsdata[i], str1);
        printf("Token %d: %s \n", i,str1);
        i++;
    }
    printf("GPS return data %s ", gpsdata[gpgga_index]);
    strcpy(buf_tmp,gpsdata[gpgga_index]);
    return buf_tmp;


}


/*

	close_port(int filedescriptor)
	Close GPS serial link

*/

int close_port(int filedescriptor) {
	if(filedescriptor>0) {
		if(!serial_close(filedescriptor)) {
			return 0;	
		}
		else {
			return -1;
		}
	}
	else {
		return -1;
	}
   return 0;
}
	
