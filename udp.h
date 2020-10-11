#ifndef _UDP_H
#define _UDP_H

#include <stdint.h>


typedef struct udp_layer udp_layer_t;

udp_layer_t* udp_open();

int udp_close ();

int udp_send ();

int udp_recv();

int funcion_numero_aleatorio_puerto();




#endif /* _UDP_H */
