#ifndef _RIPv2_H
#define _RIPv2_H

#include <stdint.h>
#include "ripv2_route_table.h"


#define PAQUETE_TOTAL_RIP_MAS_CABECERA 504

#define LONGITUD_ENTRADA 20

#define LONGITUD_MAXIMA_ENTRADAS 500  //Esta es la longitud máxima de RIP sin la cabecera (es de 25x20)

#define RIP_HEADER_SIZE 4

#define PUERTO_RIP 520

#define ENTRADAS_MAXIMAS 25

#define INFINITO 16

#define REQUEST 1

#define RESPONSE 2

#define TEMPORIZADOR 180000

#define VERSION 2


typedef struct rip_distance_vector ripv2_distance_vector_t;


int funcion_ripv2_entradas_a_longitud(int numero_entradas);

int funcion_ripv2_longitud_a_entradas(int longitud_total);

void imprimir_rip_frame(rip_frame *rip_frame, int longitud);



#endif /* _RIPv2_H */
