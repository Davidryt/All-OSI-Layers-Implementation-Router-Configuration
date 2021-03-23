#include "ripv2.h"
#include "ripv2_route_table.h"
#include "ripv2_config.h"
#include "ipv4.h"
#include "arp.h"
#include "eth.h"


#include <timerms.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <unistd.h>
#include <libgen.h>


//Esto es el manejador de rip
typedef struct rip_distance_vector {
    uint16_t familia_de_direcciones; //qué tipo de direcciones usamos
    uint16_t etiqueta_ruta; //no lo usamos y va a 0
    ipv4_addr_t direcccion_ipv4; 
    ipv4_addr_t netmask;
    ipv4_addr_t siguiente_salto;
    uint32_t metrica;
} ripv2_distance_vector_t;



//Este es el frame de RIP
struct rip_frame{

    uint8_t tipo_request_response;
    uint8_t version;
    uint16_t dominio_de_encaminamiento; //este no lo usamos va a ser 0
    ripv2_distance_vector_t entradas[25]; 

};



//En esta función metemos el número de entradas y devuelve la longitud
int funcion_ripv2_entradas_a_longitud(int numero_entradas){

    int longitud=RIP_HEADER_SIZE+numero_entradas*LONGITUD_ENTRADA;

    return longitud;

}

//En esta función metemos la longitud y devuelve el número de entradas
int funcion_ripv2_longitud_a_entradas(int longitud_total){

    int numero_entradas=(longitud_total-RIP_HEADER_SIZE)/LONGITUD_ENTRADA;

    return numero_entradas;

}


void imprimir_rip_frame(rip_frame *rip_frame, int longitud){

    printf("ESTE ES UN RIP_FRAME:\n");

    if((rip_frame->tipo_request_response)==REQUEST){
        printf("Tipo: REQUEST\n");
    }
    else{
        printf("Tipo: RESPONSE\n");
    }

    printf("Version: %d\n", rip_frame->version);

    printf("Dominio de encaminamiento: %d\n", rip_frame->dominio_de_encaminamiento);

    printf("ESTAS SON LAS ENTRADAS DE RIP_FRAME:\n");

    
    char direcccion_ipv4[IPv4_STR_MAX_LENGTH];
    char netmask[IPv4_STR_MAX_LENGTH];
    char siguiente_salto[IPv4_STR_MAX_LENGTH];

    for(int i=0; i<funcion_ripv2_entradas_a_longitud(longitud); i++){

        ipv4_addr_str(rip_frame->entradas[i].direcccion_ipv4, direcccion_ipv4);
        ipv4_addr_str(rip_frame->entradas[i].netmask, netmask);
        ipv4_addr_str(rip_frame->entradas[i].siguiente_salto, siguiente_salto);

        printf("Métrica: %d\n", ntohs(rip_frame->entradas[i].familia_de_direcciones));
        printf("Métrica: %d\n", ntohs(rip_frame->entradas[i].etiqueta_ruta));

        printf("Dirección IPv4: %s\n", direcccion_ipv4);
        printf("Netmask: %s\n", netmask);
        printf("Siguiente salto: %s\n", siguiente_salto);
        printf("Métrica: %d\n", ntohl(rip_frame->entradas[i].metrica));

    }
    

}









