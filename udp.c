#include "ipv4.h"
#include "eth.h"
#include "arp.h"
#include "udp.h"

#include <timerms.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <unistd.h>
#include <libgen.h>

#define UDP_MTU 



//Esto es el manejador de udp
typedef struct udp_layer {
    ipv4_layer_t * ipv4_layer;
    
} udp_layer_t;



//Este es el frame de IP
struct udp_frame{


    unsigned char payload_udp[UDP_MTU];


};





udp_layer_t* udp_open(){





}





int udp_close (){




}





int udp_send (){



}

int udp_recv(){





}







int funcion_numero_aleatorio_puerto(){

    /* Inicializar semilla para rand() */
    unsigned int seed = time(NULL);
    srand(seed);

    /* Generar número aleatorio entre 0 y RAND_MAX */
	int dice = rand();
	/* Número entero aleatorio entre 1 y 10 */
	puerto_aleatorio = 1024 + (int) (3000.0 * dice / (RAND_MAX));
	printf("%i\n", dice);

    return puerto_aleatorio;

}




