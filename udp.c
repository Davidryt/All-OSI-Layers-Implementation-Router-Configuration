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
#include <time.h>


#define UDP_HEADER_SIZE 8

#define UDP_MTU (IPV4_MTU-UDP_HEADER_SIZE)

#define UDP_PROTOCOL 0x11



//Esto es el manejador de udp
typedef struct udp_layer {
    ipv4_layer_t * ipv4_layer;
    uint16_t puerto_escucha;
    
} udp_layer_t;



//Este es el frame de UDP
struct udp_frame{
    uint16_t puerto_origen;
    uint16_t puerto_destino;
    uint16_t longitud_total_udp;
    uint16_t checksum_udp;
    unsigned char payload_udp[UDP_MTU];
};





udp_layer_t* udp_open(char * file_conf, char * file_conf_route, uint16_t puerto_origen){

    udp_layer_t * layer = malloc(sizeof(udp_layer_t));

    if (layer == NULL) {
        fprintf(stderr, "udp_open(): ERROR en malloc()\n");
        return NULL;
    } 


    /* Abrir el interfaz subyacente: en este caso IPv4 */
    ipv4_layer_t * ipv4_layer = ipv4_open(file_conf, file_conf_route);
    if (ipv4_layer == NULL) { 
        fprintf(stderr, "udp_open(): ERROR en ipv4_open()\n");
        return NULL;
    }  
    layer->ipv4_layer = ipv4_layer;

    //Elegir el puerto origen de escucha
    if(puerto_origen != 0){
        layer->puerto_escucha=puerto_origen;
    }
    else{
        layer->puerto_escucha=funcion_numero_aleatorio_puerto();
    }


    return layer;
}





int udp_close (udp_layer_t * layer){

    /* Cerrar capa IPv4 con ipv4_close() */
    int cerrar_capa = -1;
    cerrar_capa = ipv4_close(layer->ipv4_layer);
    free(layer);

    if(cerrar_capa == -1) {
        fprintf(stderr, "udp_close(): ERROR en ipv4_close()\n");
        return -1;   
    }

    return cerrar_capa;
}





int udp_send (udp_layer_t * layer, ipv4_addr_t dst, uint16_t puerto_destino, unsigned char * payload, int payload_len){

    /* Comprobar parámetros */
    if (layer == NULL) {
        fprintf(stderr, "udp_send(): ERROR: layer == NULL\n");
        return -1;
    }

    /* Crear la Trama UDP y rellenar todos los campos */
    struct udp_frame udp_frame;

    udp_frame.puerto_origen=htons(layer->puerto_escucha);
    udp_frame.puerto_destino=htons(puerto_destino);
    udp_frame.longitud_total_udp=htons(UDP_HEADER_SIZE+payload_len);
    udp_frame.checksum_udp=htons(0);
    memcpy(udp_frame.payload_udp, payload, payload_len);



    //Enviar con udp_send 
    /* Enviar la trama UDP creada con ipv4_send() y comprobar errores */
    int datos_enviados=0;
    //ipv4_send devuelve el número de bytes que han podido ser enviados
    datos_enviados = ipv4_send(layer->ipv4_layer, dst, UDP_PROTOCOL, (unsigned char *) &udp_frame, (UDP_HEADER_SIZE+payload_len));
    if (datos_enviados != (UDP_HEADER_SIZE+payload_len)) { //Si manda menos datos de los que hemos pedido está MAL
        fprintf(stderr, "udp_send(): ERROR en ipv4_send()\n");
        return -1;
    }

    /* Devolver el número de bytes de datos recibidos */
    printf("Los datos enviados en la función udp_send() son: %d\n", datos_enviados);
    return (datos_enviados - UDP_HEADER_SIZE);


}





int udp_recv(udp_layer_t * layer, unsigned char buffer [], uint16_t *puerto_origen, ipv4_addr_t sender, int buf_len, long int timeout){ 

    /* Inicializar temporizador para mantener timeout si se reciben tramas con tipo incorrecto. */
    timerms_t timer;
    timerms_reset(&timer, timeout);

    int buffer_udp_length = UDP_HEADER_SIZE+buf_len;
    unsigned char udp_buffer[buffer_udp_length];

    struct udp_frame * udp_frame_ptr = NULL;

    do {
        timerms_elapsed(&timer); //Devuelve el tiempo cada iteración
        long int time_left = timerms_left(&timer);

        /* Recibir trama del interfaz IPv4 y procesar errores */
        int longitud_datos_recibidos = ipv4_recv (layer->ipv4_layer, UDP_PROTOCOL, udp_buffer, sender, buffer_udp_length, time_left);
        if (longitud_datos_recibidos < 0) {
            fprintf(stderr, "Los datos recibidos de UDP ERRORRRRRRR\n");
            return -1;
        } else if (longitud_datos_recibidos == 0) {
            /* Timeout! */
            fprintf(stderr, "En udp_recv ha habido un timeout en udp_recv() \n");
            return 0;
        } else if (longitud_datos_recibidos < UDP_HEADER_SIZE) {
            fprintf(stderr, "Trama de tamaño invalido: %d bytes\n", longitud_datos_recibidos);
            continue;
        }
        printf("OJOOOO: La longitud recibida en udp_recv() es %d\n", buffer_udp_length);


        /* Comprobar si es la trama que estamos buscando */
        udp_frame_ptr = (struct udp_frame *) udp_buffer;


    } while ( ! (ntohs(udp_frame_ptr->puerto_destino)==(layer->puerto_escucha)) );

    /* Trama recibida con 'tipo' indicado. Copiar datos y dirección MAC origen */
    //memcpy(sender, udp_frame_ptr->direccion_ip_origen, IPv4_ADDR_SIZE);
    int payload_len = ntohs((udp_frame_ptr->longitud_total_udp))-UDP_HEADER_SIZE;
    if (buf_len > payload_len) {
        buf_len = payload_len; //Reduce el tamaño del buffer al tamaño de datos útiles recibidos
    }

    *puerto_origen=ntohs(udp_frame_ptr->puerto_origen);

    memcpy(buffer, udp_frame_ptr->payload_udp, buf_len);

    return payload_len;

}





int funcion_numero_aleatorio_puerto(){

    /* Inicializar semilla para rand() */
    unsigned int seed = time(NULL);
    srand(seed);

    /* Generar número aleatorio entre 0 y RAND_MAX */
	int dice = rand();
	/* Número entero aleatorio entre 1 y 10 */
	int puerto_aleatorio = 1024 + (int) (65535.0 * dice / (RAND_MAX));
	printf("%i\n", dice);

    return puerto_aleatorio;

}




