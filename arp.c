#include "arp.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <libgen.h>
#include <rawnet.h>
#include <timerms.h>
#include <string.h>
#include <netinet/in.h>

/* Tamaño de la cabecera ARP (sin incluir el campo FCS) */
#define ARP_FULL_LENGTH 28

//El hardware type que siempre es 1 en ethernet
#define HARDWARE_TYPE 1

//Protocolo de IPv4
#define IPV4_PROTOCOL 0x800

//Protocolo de ARP
#define ARP_PROTOCOL 0x806

//El operation type de ARP -> 1 va a ser para request
#define OPERATION_CODE_REQUEST_ARP 1

//El operation type de ARP -> 2 va a ser para reply
#define OPERATION_CODE_REPLY_ARP 2




/* Cabecera de una trama ARP*/
struct arp_frame {  //A nivel de red se llama paquete

	uint16_t hardware_type; //Indica el tipo de dirección Física (en este caso Ethernet=1).
	uint16_t protocol_type; //Indica el protocolo de la dirección que se quiere resolver (IPv4=0x0800).
	uint8_t hardware_length; //Indica el nombre de bytes de la dirección MAC -> 6.
	uint8_t protocol_length; /*Longitud (en octetos) de direcciones utilizadas en el protocolo de capa superior.
					   Indica el nombre de bytes de la dirección que se quiere resolver (IPv4 -> 4)*/
	uint16_t operation_code; //La operación que el emisor está realizando. 1 para request, 2 para reply.
	mac_addr_t source_hardware_address; /*Dirección física del emisor del mensaje.
							   Si el ARP es tipo Reply aquí se encuentra la dirección física buscada*/
	ipv4_addr_t source_protocol_address; //Dirección del Protocolo del emisor (IP).
	mac_addr_t target_hardware_address; /*Dirección MAC del receptor del mensaje.
							   En un ARP request este campo es irrelevante (es la dirección buscada)*/
	ipv4_addr_t target_protocol_address; //Dirección del Protocolo del receptor(IP).
};


//Función resolve que me da la dirección MAC de una dirección IP
int arp_resolve (eth_iface_t * iface, ipv4_addr_t dest, mac_addr_t mac){

    int err=0; //Mirar el send de esta función

    mac_addr_t mac_del_que_responda;

    int is_my_ip;
    int is_a_reply;

	/* Crear la Trama Ethernet y rellenar todos los campos */
  struct arp_frame arp_frame;
  arp_frame.hardware_type=htons(HARDWARE_TYPE);
  arp_frame.protocol_type = htons(IPV4_PROTOCOL);
  arp_frame.hardware_length = MAC_ADDR_SIZE;
  arp_frame.protocol_length = IPv4_ADDR_SIZE;
  arp_frame.operation_code = htons(OPERATION_CODE_REQUEST_ARP);

  eth_getaddr(iface, arp_frame.source_hardware_address);  
  memcpy(arp_frame.source_protocol_address, IPv4_ZERO_ADDR, IPv4_ADDR_SIZE);
  memset(arp_frame.target_hardware_address, 0x00, MAC_ADDR_SIZE);
  memcpy(arp_frame.target_protocol_address, dest, IPv4_ADDR_SIZE);

  // Hay que enviar la trama a la dirección de broadcast
  err = eth_send(iface, MAC_BCAST_ADDR, ARP_PROTOCOL, &arp_frame, ARP_FULL_LENGTH);
  if (err == -1) {
    fprintf(stderr, "%s: ERROR en arp_send()\n", myself);
    return -1; 
  }

  /* Recibir trama Ethernet del Servidor y procesar errores */
  long int timeout = 2000;

    /* Inicializar temporizador para mantener timeout si se reciben tramas con
     tipo incorrecto. */
  timerms_t timer;
  timerms_reset(&timer, timeout);

    int buffer_arp_length = ARP_FULL_LENGTH;
    unsigned char arp_buffer[buffer_arp_length];


    do {

        timerms_elapsed(&timer); //Devuelve el tiempo cada iteración
        long int time_left = timerms_left(&timer);

        /* Recibir trama del interfaz Ethernet y procesar errores */
        longitud_datos_recibidos = eth_recv (iface, mac_del_que_responda, ARP_PROTOCOL, arp_buffer, buffer_arp_length, time_left);
        if (longitud_datos_recibidos < 0) {
        fprintf(stderr, "Los datos recibidos de ARP ERRORRRRRRR\n"); 
        return -1;
        } else if (longitud_datos_recibidos == 0) {
        /* Timeout! */
        return 0;
        } else if (longitud_datos_recibidos < ARP_FULL_LENGTH) {
        fprintf(stderr, "Trama de tamaño invalido: %d bytes\n",
              longitud_datos_recibidos);
        continue;
    }

    /* Comprobar si es la trama que estamos buscando */
    arp_frame_ptr = (struct arp_frame *) arp_buffer;
    is_my_ip = (memcmp(arp_frame_ptr->source_protocol_address, dest, IPv4_ADDR_SIZE) == 0);
    is_a_reply = (ntohs(arp_frame_ptr->operation_code) == OPERATION_CODE_REPLY_ARP);

  } while ( ! (is_my_ip && is_a_reply));

    fprintf("ESTA ES LA MAC QUE HEMOS OBTENIDO: %c\n",arp_frame_ptr->target_hardware_address); //PREGUNTAR


  return 1; //Ha salido todo bien

}
