#include "ipv4.h"

#include <stdio.h>
#include <stdlib.h>

/* Dirección IPv4 a cero: "0.0.0.0" */
ipv4_addr_t IPv4_ZERO_ADDR = { 0, 0, 0, 0 };


//Esto es el manejador de ip
typedef struct ipv4_layer {
    eth_iface_t * iface; 
    ipv4_addr_t addr; 
    ipv4_addr_t netmask; 
    ipv4_route_table_t * routing_table;
} ipv4_ layer_t ; 


//Este es el frame de IP
struct ip_frame{

    int8_t version_cabecera;
    uint8_t tipo_ip;  
    uint16_t longitud_total_ip;
    uint16_t identificador_ip;
    uint16_t flag_mas_offset;
    uint8_t ttl;
    uint8_t protocolo_ip;
    uint16_t checksum_ip;
    ipv4_addr_t direccion_ip_origen;
    ipv4_addr_t direccion_ip_destino;
    unsigned char payload_ip[IPV4_MTU];

}






/* void ipv4_addr_str ( ipv4_addr_t addr, char* str );
 *
 * DESCRIPCIÓN:
 *   Esta función genera una cadena de texto que representa la dirección IPv4
 *   indicada.
 *
 * PARÁMETROS:
 *   'addr': La dirección IP que se quiere representar textualente.
 *    'str': Memoria donde se desea almacenar la cadena de texto generada.
 *           Deben reservarse al menos 'IPv4_STR_MAX_LENGTH' bytes.
 */
void ipv4_addr_str ( ipv4_addr_t addr, char* str )
{
  if (str != NULL) {
    sprintf(str, "%d.%d.%d.%d",
            addr[0], addr[1], addr[2], addr[3]);
  }
}


/* int ipv4_str_addr ( char* str, ipv4_addr_t addr );
 *
 * DESCRIPCIÓN:
 *   Esta función analiza una cadena de texto en busca de una dirección IPv4.
 *
 * PARÁMETROS:
 *    'str': La cadena de texto que se desea procesar.
 *   'addr': Memoria donde se almacena la dirección IPv4 encontrada.
 *
 * VALOR DEVUELTO:
 *   Se devuelve 0 si la cadena de texto representaba una dirección IPv4.
 *
 * ERRORES:
 *   La función devuelve -1 si la cadena de texto no representaba una
 *   dirección IPv4.
 */
int ipv4_str_addr ( char* str, ipv4_addr_t addr )
{
  int err = -1;

  if (str != NULL) {
    unsigned int addr_int[IPv4_ADDR_SIZE];
    int len = sscanf(str, "%d.%d.%d.%d", 
                     &addr_int[0], &addr_int[1], 
                     &addr_int[2], &addr_int[3]);

    if (len == IPv4_ADDR_SIZE) {
      int i;
      for (i=0; i<IPv4_ADDR_SIZE; i++) {
        addr[i] = (unsigned char) addr_int[i];
      }
      
      err = 0;
    }
  }
  
  return err;
}


/*
 * uint16_t ipv4_checksum ( unsigned char * data, int len )
 *
 * DESCRIPCIÓN:
 *   Esta función calcula el checksum IP de los datos especificados.
 *
 * PARÁMETROS:
 *   'data': Puntero a los datos sobre los que se calcula el checksum.
 *    'len': Longitud en bytes de los datos.
 *
 * VALOR DEVUELTO:
 *   El valor del checksum calculado.
 */
uint16_t ipv4_checksum ( unsigned char * data, int len )
{
  int i;
  uint16_t word16;
  unsigned int sum = 0;
    
  /* Make 16 bit words out of every two adjacent 8 bit words in the packet
   * and add them up */
  for (i=0; i<len; i=i+2) {
    word16 = ((data[i] << 8) & 0xFF00) + (data[i+1] & 0x00FF);
    sum = sum + (unsigned int) word16;	
  }

  /* Take only 16 bits out of the 32 bit sum and add up the carries */
  while (sum >> 16) {
    sum = (sum & 0xFFFF) + (sum >> 16);
  }

  /* One's complement the result */
  sum = ~sum;

  return (uint16_t) sum;
}



ipv4_layer_t* ipv4_open(char * file_conf, char * file_conf_route) {

    ipv4_layer_t * layer = malloc(sizeof(ipv4_layer_t));
    /* 1. Crear layer -> routing_table */
    /* 2. Leer direcciones y subred de file_conf */
    /* 3. Leer tabla de reenvío IP de file_conf_route */
    /* 4. Inicializar capa Ethernet con eth_open() */

    if (layer == NULL) {
        fprintf(stderr, "ipv4_open(): ERROR en malloc()\n");
        return NULL;
    } 



    
    //Esto puede ser el punto 4
    /* Abrir el interfaz "en crudo" subyacente */
    eth_iface * eth_iface = eth_open(ipv4_layer->iface);
    if (eth_iface == NULL) {
        fprintf(stderr, "ipv4_open(): ERROR en eth_open()\n");
        return NULL;
    }  
    layer->iface = eth_iface;




    return layer;
}


int ipv4_close (ipv4_layer_t * layer) {
    /* 1. Liberar table de rutas (layer -> routing_table) */
    



    /* 2. Cerrar capa Ethernet con eth_close() */
    int err = -1;

    if (layer != NULL) {
        err = eth_close(layer->iface);
        free(layer);
    }

    return err;

}



int ipv4_send (ipv4_layer_t * layer, ipv4_addr_t dst, uint8_t protocol, unsigned char * payload, int payload_len) {


}


int ipv4_recv(ipv4_layer_t * layer, uint8_t protocol, unsigned char buffer [], ipv4_addr_t sender, int buf_len, long int timeout) {


}











