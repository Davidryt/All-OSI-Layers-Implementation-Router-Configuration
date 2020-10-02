#include "ipv4.h"

#include <stdio.h>
#include <stdlib.h>


#define IP_FULL_LENGTH 

#define IP_HEADER_SIZE 20


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


/* 1. Llamar a route_table_lookup para que nos devuelva la ruta de salida
   2. Crear la cabecer IPv4 y rellenar los campos
   3. Hacer un arp_resolve: OJO puede haber dos casos (que la dirección destino esté en mi subred o no)
   4. Enviar con eth_send
*/

int ipv4_send (ipv4_layer_t * layer, ipv4_addr_t dst, uint8_t protocol, unsigned char * payload, int payload_len) {

    /* Comprobar parámetros */
    if (layer == NULL) {
        fprintf(stderr, "ipv4_send(): ERROR: layer == NULL\n");
        return -1;
    }

    //El punto 1 
    ipv4_route_t ruta_salida=ipv4_route_table_lookup(layer->routing_table, dst);

    /* Crear la Trama IPv4 y rellenar todos los campos */
    struct ip_frame ip_frame;
    ip_frame.version_cabecera=htons();
    ip_frame.tipo_ip=htons();
    ip_frame.longitud_total_ip=;
    ip_frame.identificador_ip=htons();
    ip_frame.flag_mas_offset=htons();
    ip_frame.ttl=htons();
    ip_frame.protocolo_ip=htons(IPV4_PROTOCOL);
    ip_frame.checksum_ip=htons();
    memcpy(ip_frame.direccion_ip_origen, IPv4_ZERO_ADDR, IPv4_ADDR_SIZE);
    memcpy(ip_frame.direccion_ip_destino, IPv4_ZERO_ADDR, IPv4_ADDR_SIZE);



    int resultado_arp_resolve=0; 
    mac_addr_t mac_del_que_responda;

    //Hacer un arp_resolve: OJO puede haber dos casos (que la dirección destino esté en mi subred o no)
    if(ruta_salida){ //Este el caso de mi subred->el arp_resolve lo hago a la dirección de destino
        resultado_arp_resolve = arp_resolve(layer->iface, dst, mac_del_que_responda);
    }
    else{ //Este el caso de que no esté en mi subred->el arp_resolve lo hago al gateway
        resultado_arp_resolve = arp_resolve(layer->iface, ruta_salida->gateway_addr, mac_del_que_responda);
    }

    //Comprobar que arp_resolve ha salido bien
    if(resultado_arp_resolve!=1){
        fprintf(stderr, "arp_resolve en ipv4_send(): ERROR en el resolve()\n");
        return -1;
    }


    /* Enviar la trama IP creada con eth_send() y comprobar errores */
    int err=0;
    //eth_send devuelve -1 si ha habido un error u otro número >0 que es el número de bytes que han podido ser enviados
    err = eth_send(layer->iface, (unsigned char *) &ip_frame, IP_FULL_LENGTH);
    if (err == -1) {
        fprintf(stderr, "ipv4_send(): ERROR en eth_send()\n");
        return -1;
    }


    /* Devolver el número de bytes de datos recibidos */
    return (err - IP_HEADER_SIZE);

}


/* 1. Casting a la cabecera IP
   2. Tener cuidado con el ntohs y con el htons
   3. Comprobar que el protocolo es el nuestro 
*/

int ipv4_recv(ipv4_layer_t * layer, uint8_t protocol, unsigned char buffer [], ipv4_addr_t sender, int buf_len, long int timeout) {

    //Para convertir el payload eth a IP se puede hacer así (lo dijo en clase)
    ipv4_frame_t * ip_header=NULL;
    ip_header=(ipv4_frame_t *)payload;

    /* Inicializar temporizador para mantener timeout si se reciben tramas con tipo incorrecto. */
    timerms_t timer;
    timerms_reset(&timer, timeout);

    int buffer_ip_length = IP_FULL_LENGTH;
    unsigned char ip_buffer[buffer_ip_length];

    do {
        timerms_elapsed(&timer); //Devuelve el tiempo cada iteración
        long int time_left = timerms_left(&timer);

        /* Recibir trama del interfaz Ethernet y procesar errores */
        int longitud_datos_recibidos = eth_recv (layer->iface, mac_del_que_responda, IPV4_PROTOCOL, ip_buffer, buffer_ip_length, time_left);
        if (longitud_datos_recibidos < 0) {
            fprintf(stderr, "Los datos recibidos de IP ERRORRRRRRR\n");
            return -1;
        } else if (longitud_datos_recibidos == 0) {
            /* Timeout! */
            return 0;
        } else if (longitud_datos_recibidos < IP_HEADER_SIZE) {
            fprintf(stderr, "Trama de tamaño invalido: %d bytes\n", longitud_datos_recibidos);
            continue;
        }

        /* Comprobar si es la trama que estamos buscando */
        eth_frame_ptr = (struct eth_frame *) eth_buffer;
        is_my_mac = (memcmp(eth_frame_ptr->dest_addr, iface->mac_address, MAC_ADDR_SIZE) == 0);
        is_target_type = (ntohs(eth_frame_ptr->type) == type);

    } while ( ! (is_my_mac && is_target_type) );


}











