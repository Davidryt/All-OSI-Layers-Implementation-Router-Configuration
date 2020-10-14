#include "ipv4.h"
#include "eth.h"
#include "ipv4_route_table.h"
#include "ipv4_config.h"
#include "arp.h"

#include <timerms.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <unistd.h>
#include <libgen.h>



#define IP_HEADER_SIZE 20

#define CAMPO_VERSION_MAS_CABECERA 0x45

#define TTL 64


/* Dirección IPv4 a cero: "0.0.0.0" */
ipv4_addr_t IPv4_ZERO_ADDR = { 0, 0, 0, 0 };


//Esto es el manejador de ip
typedef struct ipv4_layer {
    eth_iface_t * iface; 
    ipv4_addr_t addr; 
    ipv4_addr_t netmask; 
    ipv4_route_table_t * routing_table;
} ipv4_layer_t; 


//Este es el frame de IP
struct ip_frame{

    uint8_t version_mas_cabecera;
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

};






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
void ipv4_addr_str ( ipv4_addr_t addr, char* str)
{
  if (str != NULL) {
    sprintf(str, "%d.%d.%d.%d", addr[0], addr[1], addr[2], addr[3]);
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



//IPv4 devuelve el layer entero
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

    /* 2. Leer direcciones y subred de file_conf */
    //Ahora rellenamos la estructura manejadora -> la layer
    char interfaz_temporal[IFACE_NAME_MAX_LENGTH];
    int err_read_config=ipv4_config_read(file_conf, interfaz_temporal, layer->addr, layer->netmask);

    if(err_read_config==-1){
        fprintf(stderr, "ipv4_open(): ERROR en ipv4_config_read()\n");
    }



//Esto para ver errores en el servidor
/*char patata[IPv4_STR_MAX_LENGTH];
ipv4_addr_str(layer->addr, patata);
printf("HOLAAAAAAA %s\n", patata);
printf("HOLAAAAAAA2 %s\n", layer->iface);*/




    /* 1. Crear layer -> routing_table */
    layer->routing_table=ipv4_route_table_create();

    if(layer->routing_table == NULL){
        fprintf(stderr, "ipv4_open(): ERROR en ipv4_route_create()\n");
    }


    /* 3. Leer tabla de reenvío IP de file_conf_route */
    int err_read_route_table=ipv4_route_table_read(file_conf_route, layer->routing_table);

    if(err_read_route_table==-1){
        fprintf(stderr, "ipv4_open(): ERROR en ipv4_route_table_read()\n");
    }

    
    //Esto puede ser el punto 4
    /* Abrir el interfaz subyacente */
    eth_iface_t * eth_iface = eth_open(interfaz_temporal);
    if (eth_iface == NULL) {
        fprintf(stderr, "ipv4_open(): ERROR en eth_open()\n");
        return NULL;
    }  
    layer->iface = eth_iface;


    return layer;
}


//Devuelve si hemos tenido errores al cerrar eth_close: 0 si todo bien y -1 si ha habido error
int ipv4_close (ipv4_layer_t * layer) {
    /* 1. Liberar table de rutas (layer -> routing_table) */
    
    ipv4_route_table_free(layer -> routing_table);

    /* 2. Cerrar capa Ethernet con eth_close() */
    int err = -1;
    err = eth_close(layer->iface);
    free(layer);

    if(err == -1) {
        fprintf(stderr, "ipv4_close(): ERROR en eth_close()\n");
        return -1;   
    }

    return err;

}


/* 1. Llamar a route_table_lookup para que nos devuelva la ruta de salida
   2. Crear la cabecer IPv4 y rellenar los campos
   3. Hacer un arp_resolve: OJO puede haber dos casos (que la dirección destino esté en mi subred o no)
   4. Enviar con eth_send */

int ipv4_send (ipv4_layer_t * layer, ipv4_addr_t dst, uint8_t protocol, unsigned char * payload, int payload_len) {

    /* Comprobar parámetros */
    if (layer == NULL) {
        fprintf(stderr, "ipv4_send(): ERROR: layer == NULL\n");
        return -1;
    }

    //1. Llamar a route_table_lookup para que nos devuelva la ruta de salida 
    ipv4_route_t * ruta_salida=ipv4_route_table_lookup(layer->routing_table, dst);

    if(ruta_salida == NULL){
        fprintf(stderr, "ipv4_send(): ERROR: ipv4_route_table_lookup\n");
    }

    /* Crear la Trama IPv4 y rellenar todos los campos */
    struct ip_frame ip_frame;

    ip_frame.version_mas_cabecera=(CAMPO_VERSION_MAS_CABECERA);
    ip_frame.tipo_ip=0;
    ip_frame.longitud_total_ip=htons(IP_HEADER_SIZE+payload_len);
    ip_frame.identificador_ip=htons(2807);
    ip_frame.flag_mas_offset=0;
    ip_frame.ttl=TTL;
    ip_frame.protocolo_ip=protocol;
    ip_frame.checksum_ip=htons(0); 
    memcpy(ip_frame.direccion_ip_origen, layer->addr, IPv4_ADDR_SIZE);
    memcpy(ip_frame.direccion_ip_destino, dst, IPv4_ADDR_SIZE);
    memcpy(ip_frame.payload_ip, payload, payload_len);

    //Calcular el checksum y volver a hacer un htons
    /*ipv4_send() [hacer el checksum]
    1. header->checksum = htons(0);
    2. hacer htons correspondientes;
    3. header->checksum = ipv4_checksum();
    4. header->checksum = htons(header->checksum) */
    ip_frame.checksum_ip = htons(ipv4_checksum((unsigned char *)&ip_frame, IP_HEADER_SIZE));



    //3. Hacer un arp_resolve: OJO puede haber dos casos (que la dirección destino esté en mi subred o no)
    int resultado_arp_resolve=0; 
    mac_addr_t mac_del_que_responda;

    //Hacer un arp_resolve: OJO puede haber dos casos (que la dirección destino esté en mi subred o no)
    if(memcmp(ruta_salida->gateway_addr, IPv4_ZERO_ADDR, IPv4_ADDR_SIZE)==0){ 
    //Este el caso de mi subred->el arp_resolve lo hago a la dirección de destino
        resultado_arp_resolve = arp_resolve(layer->iface, layer->addr, dst, mac_del_que_responda);

        printf("ARP resolve en mi subred en la función send()\n");
    }
    else{ //Este el caso de que no esté en mi subred->el arp_resolve lo hago al gateway
        resultado_arp_resolve = arp_resolve(layer->iface, layer->addr, ruta_salida->gateway_addr, mac_del_que_responda);
        printf("ARP resolve en otra subred en la función send()\n");
    }

    //Comprobar que arp_resolve ha salido bien
    if(resultado_arp_resolve!=1){
        fprintf(stderr, "arp_resolve en ipv4_send(): ERROR en el resolve()\n");
        return -1;
    }


    //4. Enviar con eth_send 
    /* Enviar la trama IP creada con eth_send() y comprobar errores */
    int datos_enviados=0;
    //eth_send devuelve -1 si ha habido un error u otro número >0 que es el número de bytes que han podido ser enviados
    datos_enviados = eth_send(layer->iface, mac_del_que_responda, IPV4_PROTOCOL, (unsigned char *) &ip_frame, (IP_HEADER_SIZE+payload_len));
    if (datos_enviados != (IP_HEADER_SIZE+payload_len)) { //Si manda menos datos de los que hemos pedido está MAL
        fprintf(stderr, "ipv4_send(): ERROR en eth_send()\n");
        return -1;
    }

    /* Devolver el número de bytes de datos recibidos */
    printf("Los datos enviados en la función ipv4_send() son: %d\n", datos_enviados);
    return (datos_enviados - IP_HEADER_SIZE);

}


/* 1. Casting a la cabecera IP
   2. Tener cuidado con el ntohs y con el htons
   3. Comprobar que el protocolo es el nuestro 
*/

int ipv4_recv(ipv4_layer_t * layer, uint8_t protocol, unsigned char buffer [], ipv4_addr_t sender, int buf_len, long int timeout){

    /* Inicializar temporizador para mantener timeout si se reciben tramas con tipo incorrecto. */
    timerms_t timer;
    timerms_reset(&timer, timeout);

    int buffer_ip_length = IP_HEADER_SIZE+buf_len;
    unsigned char ip_buffer[buffer_ip_length];

    mac_addr_t mac_del_que_responda;

    struct ip_frame * ip_frame_ptr = NULL;

    int is_my_ip;
    int is_target_type;

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
            fprintf(stderr, "En eth_recv ha habido un timeout en ipv4_recv() \n");
            return 0;
        } else if (longitud_datos_recibidos < IP_HEADER_SIZE) {
            fprintf(stderr, "Trama de tamaño invalido: %d bytes\n", longitud_datos_recibidos);
            continue;
        }
        //printf("OJOOOO: La longitud recibida en ipv4_recv() es %d\n", buffer_ip_length);


        /* Comprobar si es la trama que estamos buscando */
        ip_frame_ptr = (struct ip_frame *) ip_buffer;

        //OJOOO hay que comprobar el checksum
        uint16_t checksum_temporal=ntohs(ip_frame_ptr->checksum_ip); //Este es el que hemos recibido del paquete
        ip_frame_ptr->checksum_ip=htons(0);
        //uint16_t checksum_calculado=ipv4_checksum((unsigned char *)ip_frame_ptr, IP_HEADER_SIZE);  
        ip_frame_ptr->checksum_ip = ipv4_checksum((unsigned char *)ip_frame_ptr, IP_HEADER_SIZE);

        if(checksum_temporal != ip_frame_ptr->checksum_ip){
            fprintf(stderr, "El checksum es incorrecto en ipv4_recv()\n");
            continue;
        }

        is_my_ip = (memcmp(ip_frame_ptr->direccion_ip_destino, layer->addr, IPv4_ADDR_SIZE) == 0);
        //is_target_type = (ntohs(ip_frame_ptr->tipo_ip) == protocol);
        is_target_type = ((ip_frame_ptr->protocolo_ip) == protocol);

    } while ( ! (is_my_ip && is_target_type) );


    /* Trama recibida con 'tipo' indicado. Copiar datos y dirección MAC origen */
    memcpy(sender, ip_frame_ptr->direccion_ip_origen, IPv4_ADDR_SIZE);
    int payload_len = ntohs((ip_frame_ptr->longitud_total_ip))-IP_HEADER_SIZE;
    if (buf_len > payload_len) {
        buf_len = payload_len; //Reduce el tamaño del buffer al tamaño de datos útiles recibidos
    }
    memcpy(buffer, ip_frame_ptr->payload_ip, buf_len);
    //Para convertir el payload eth a IP se puede hacer así (lo dijo en clase)
    //Esto es equivalente a:
    /*ipv4_frame_t * ip_header=NULL;
    ip_header=(ipv4_frame_t *)ip_frame_ptr->payload_ip;*/

    return payload_len;

}











