
#include <stdbool.h>
#include <vector.h>
#include <stdio.h>
#include <nmcommon.h>

typedef enum 
{ 
    MANUAL, 
    SHARED, 
    DYNAMIC,
}SW_MODE;

typedef enum
{ 
    ETH0=0,
    PORT1=1, 
    PORT2=2,
    PORT3=3, 
    PORT4=4, 
    PORT5=5, 
    PORT6=6, 
    PORT7=7,
}SW_PORT;

typedef struct{
    //Port port_num;
    SW_MODE port_mode;
    const char *port_ip;
    const char *port_subnet;
    const char *port_gateway;
    const char *port_dns;
}port_config;

bool switch_init();
bool port_set_config( SW_PORT port_num, port_config config);
bool port_get_config( SW_PORT port_num, vector *v);
bool port_reset(SW_PORT port_num);