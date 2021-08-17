#include <stdio.h>
#include <string.h>
#include <stdbool.h>

typedef enum { 
manual, 
shared, 
dynamic
} Mode;

typedef enum { 
port1=1, 
port2=2,
port3=3, 
port4=4, 
port5=5, 
port6=6, 
port7=7
} Port;

struct PortData{
    //Port port_num;
    Mode port_mode;
    const char *port_ip;
    const char *port_subnet;
    const char *port_gateway;
    const char *port_dns;
};


/*Internal function*/
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


//Port init fucntion
int Port_Init(){
	//function to port up if not availble
	char cmd[1024];
	char ret[1024]="";
	
	//for(int i=1;i<=7;i++)
	//{
		
		//sprintf(cmd, "ifconfig | grep port%d",i);      //condition to check port available using command
		sprintf(cmd, "ifconfig -a | grep eth0"); 
		if(!runcommand(cmd,ret,1024))
		{
			printf("command1 not run");
			return -1;			   //command not run error
	
		}
	if(strlen(ret)!=0)
	{
		//for(int i=1;i<=7;i++){

			//sprintf(cmd,"sudo ifconfig port%d up",i); 
			sprintf(cmd,"ifconfig eth0 up");   //to make port1 to port7 are up using command
			if(!runcommand(cmd,ret,1024))
			{

				printf("error2");
                		return -1;		//command not run error
			
			}
			
		

		//sprintf(cmd, "nmcli con show | grep port%d",i);      //condition to check port available using command
        	sprintf(cmd, "nmcli con show | grep eth0");
        	if(!runcommand(cmd,ret,1024))
        	{
        		printf("error4");
                	return -1;                         //command not run error

        	}
        	if(strlen(ret)!=0)
        	{

                	//for(int i=1;i<=7;i++){

                        //sprintf(cmd, "nmcli con add con-name port%d ifname port%d type ethernet ethernet.mtu 1500",i,i);    //to make port1 to port7 are up using command
                        sprintf(cmd, "nmcli con add con-name eth0-new ifname eth0 type ethernet ethernet.mtu 1500");
                        if(!runcommand(cmd,ret,1024))
                        {
                        	printf("error5");
                                return -1;              //command not run error
                        }
			if(strlen(ret)==0)
			{
				printf("error6");
				return 1;
			}			

                	//}
                	printf("connections created for all ports!!!!\n");  //all ports are up and running
                	return 1;

        	}
	}
	
	//}
	
	//return -1;	
	return 2;
	
}


/*int Set_Port_Config(Port port_num, struct PortData *pconfig){
	//set port configuration
	if(Port_Init()==1){
		
		char cmd[1024];
	        char ret[1024]="";
		
		if(pconfig->port_mode==manual){	
		//check condition for given mode
		{
		//set mode for port		
		sprintf(cmd, "nmcli conn mod port%d ipv4.method manual",port_num);      //condition to check port available using command
	        if(!runcommand(cmd,ret,1024))
        	{

                	return -1;                         //command not run error

        	}
		//set static ip and subnet
		sprintf(cmd, "nmcli conn mod port%d ipv4.addresses %s/%s",port_num,pconfig->port_ip,pconfig->port_subnet);      //condition to check port available using command
                if(!runcommand(cmd,ret,1024))
                {

                        return -1;                         //command not run error

                }
		//set gateway 
		sprintf(cmd, "nmcli conn mod port%d ipv4.gateway %s",port_num,pconfig->port_gateway);      //condition to check port available using command
                if(!runcommand(cmd,ret,1024))
                {

                        return -1;                         //command not run error

                }
		//set dns
		sprintf(cmd, "nmcli conn mod port%d ipv4.dns %s",port_num,pconfig->port_dns);      //condition to check port available using command
                if(!runcommand(cmd,ret,1024))
                {

                        return -1;                         //command not run error

                }
			
		printf("port%d is set to static configuration!!!\n",port_num);
		return 1; //return for static configuration
		}
		}
		//set configuration for dynamic
		else if(pconfig->port_mode==dynamic) 
		{
		//set mode for port             
                sprintf(cmd, "nmcli conn mod port%d ipv4.method manual",port_num);      //condition to check port available using command
                if(!runcommand(cmd,ret,1024))
                {

                        return -1;                         //command not run error

                }
                //set ip and subnet
                sprintf(cmd, "nmcli conn mod port%d ipv4.addresses %s/%s",port_num,pconfig->port_ip,pconfig->port_subnet);      //condition to check port available using command
                if(!runcommand(cmd,ret,1024))
                {

                        return -1;                         //command not run error

                }
                //set gateway 
                sprintf(cmd, "nmcli conn mod port%d ipv4.gateway %s",port_num,pconfig->port_gateway);      //condition to check port available using command
                if(!runcommand(cmd,ret,1024))
                {

                        return -1;                         //command not run error

                }
                //set dns
                sprintf(cmd, "nmcli conn mod port%d ipv4.dns %s",port_num,pconfig->port_dns);      //condition to check port available using command
                if(!runcommand(cmd,ret,1024))
                {

                        return -1;                         //command not run error

                }
	
		printf("port%d is set to dynammic configuration!!!\n",port_num);
		return 2; 	//return for dynamic configuration
		}

		//set configuration for shared
		else
		{
		
		//set mode for port             
                sprintf(cmd, "nmcli conn mod port%d ipv4.method manual",port_num);      //condition to check port available using command
                if(!runcommand(cmd,ret,1024))
                {

                        return -1;                         //command not run error

                }
                //set ip and subnet
                sprintf(cmd, "nmcli conn mod port%d ipv4.addresses %s/%s",port_num,pconfig->port_ip,pconfig->port_subnet);      //condition to check port available using command
                if(!runcommand(cmd,ret,1024))
                {

                        return -1;                         //command not run error

                }
                //set gateway 
                sprintf(cmd, "nmcli conn mod port%d ipv4.gateway %s",port_num,pconfig->port_gateway);      //condition to check port available using command
                if(!runcommand(cmd,ret,1024))
                {

                        return -1;                         //command not run error

                }
                //set dns
                sprintf(cmd, "nmcli conn mod port%d ipv4.dns %s",port_num,pconfig->port_dns);      //condition to check port available using command
                if(!runcommand(cmd,ret,1024))
                {

                        return -1;                         //command not run error

                }

		
		
		printf("port%d is set to shared configuration!!!\n",port_num);
                return 3;    //return for shared configuration
		}	
		
			
	}else{
		return -1;	//error for port init fail
	}
}*/




/*int Get_Port_Config(struct PortData *pconfig){
	//get port configuration
	/ *if(Port_Init()){
                
                if(get_config vi ifconfig command){ //condition to check if configuration is available for port or not
                
                        return pconfig;   //return data for port
                }
                else{
                        return -1; //error for port configuration not available
                }
                        
        }else{
                return -1;      //error for port init failed
        }* /

}
*/
int Port_Reset(Port port_num){

	//reset ports
	//function to port up if not availble
        char cmd[1024];
        char ret[1024]="";

        sprintf(cmd, "ifconfig -a | grep eth0"); 
		if(!runcommand(cmd,ret,1024))
		{
			printf("command1 not run");
			return -1;			   //command not run error
	
		}
	if(strlen(ret)!=0)
	{
        //for(int i=1;i<=7;i++){

		sprintf(cmd, "ifconfig eth0 down"); 
        	//sprintf(cmd, "ifconfig port%d down",i);    //to make port1 to port7 are up using command
        	if(!runcommand(cmd,ret,1024)){

        		return -1;              //command not run error
        	}
        	printf("port is down\n");
        	sprintf(cmd, "ifconfig eth0 up"); 
		//sprintf(cmd, "ifconfig port%d up",i);
		if(!runcommand(cmd,ret,1024)){

        		return -1;              //command not run error
		}
		printf("port is up\n");
        //}
	}	
	return 1;
}


int main()
{	
	struct PortData marvell_ports;

	Port port_num =port4;
	marvell_ports.port_mode = manual;
	marvell_ports.port_ip = "192.168.1.1";
        marvell_ports.port_gateway = "255.255.255.0";
	marvell_ports.port_subnet = "24";
	marvell_ports.port_dns = "8.8.8.8";
     
	printf("marvell_ports.port_num is: %d\n", port_num);
	printf("marvell_ports.port_mode is: %d\n", marvell_ports.port_mode);
	printf("marvell_ports.port_num is: %s\n", marvell_ports.port_ip);
	printf("marvell_ports.port_subnet is: %s\n", marvell_ports.port_subnet);
        printf("marvell_ports.port_gateway is: %s\n", marvell_ports.port_gateway);
        printf("marvell_ports.port_dns is: %s\n", marvell_ports.port_dns);

	//int val=Set_Port_Config(port_num, &marvell_ports);
	//printf("value ==%d\n",val);
	
	//int val=Port_Init();
	//printf("value ==%d\n",val);

	int val= Port_Reset(port_num);
	printf("value ==%d\n",val);

return 0;
}
