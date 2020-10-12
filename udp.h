#ifndef _UDP_H
#define _UDP_H

#include <stdint.h>


typedef struct udp_layer udp_layer_t;

udp_layer_t* udp_open(char * file_conf, char * file_conf_route, uint16_t puerto_origen);

int udp_close (udp_layer_t * layer);

int udp_send (udp_layer_t * layer, ipv4_addr_t dst, uint16_t puerto_destino, unsigned char * payload, int payload_len);

int udp_recv(udp_layer_t * layer, unsigned char buffer [], ipv4_addr_t sender, int buf_len, long int timeout); //faltan cosas

int funcion_numero_aleatorio_puerto();




#endif /* _UDP_H */
