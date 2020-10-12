#include "ipv4.h"
#include "eth.h"

#include "ipv4_route_table.h"
#include "ipv4_config.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <libgen.h>



int main ( int argc, char * argv[]){  //preguntar si la longitud es optativa

    /* Mostrar mensaje de ayuda si el número de argumentos es incorrecto */
    char * myself = basename(argv[0]);
    if (argc != 4) {
        printf("Uso: %s <fichero.txt> <route_table> <long>\n", myself);
        printf("        <fichero.txt>: Fichero de texto del servidor\n");
        printf("        <route_table>: La route table del servidor\n");
        printf("        <long>: Longitud de los datos\n");
        exit(-1);
    }


    /* Procesar los argumentos de la línea de comandos */
    char* ipv4_config_server=argv[1];
    char* ipv4_route_table_server=argv[2];
    char* longitud_datos_string = argv[3];

    int longitud_datos_int = atoi(longitud_datos_string); //De string a número



    /* Abrir la interfaz IPv4 */
    ipv4_layer_t * ipv4_layer = ipv4_open(ipv4_config_server, ipv4_route_table_server);
    if (ipv4_layer == NULL) {
        fprintf(stderr, "ERROR en ipv4_open en el servidor(\"%s\")\n", myself);
        exit(-1);
    }
  
    printf("Abriendo interfaz IPv4 en el servidor.\n");


    unsigned char buffer[longitud_datos_int];
    ipv4_addr_t direccion_origen;
    char direccion_origen_string[IPv4_STR_MAX_LENGTH];
    //char direccion_nuestra_string[IPv4_STR_MAX_LENGTH];

    /*ipv4_addr_str(ipv4_layer->addr, direccion_nuestra_string);
    printf("Enviando al Cliente Ethernet (%s):\n", direccion_nuestra_string);*/
       

    while(1) {
        /* Recibir trama IPv4 del Cliente */
        long int timeout = -1;

        printf("Escuchando tramas IPv4 (tipo=0x800) ...\n");
    
        int payload_len = ipv4_recv(ipv4_layer, (uint8_t)IPV4_PROTOCOL, buffer, direccion_origen, longitud_datos_int, timeout);
        if (payload_len == -1) {
            fprintf(stderr, "%s: ERROR en ipv4_recv()\n", myself);
            exit(-1);
        }

        ipv4_addr_str(direccion_origen, direccion_origen_string);
    
        printf("Recibidos %d bytes del Cliente IPv4 (%s):\n", payload_len, direccion_origen_string);
        //print_pkt(buffer, payload_len, 0);

        /* Enviar la misma trama IPv4 de vuelta al Cliente */
        //ipv4_addr_str(ipv4_layer->addr, direccion_nuestra_string);
        //printf("Enviando %d bytes al Cliente Ethernet (%s):\n", payload_len, direccion_nuestra_string);
        printf("Enviando %d bytes al Cliente Ethernet\n", payload_len);
        //print_pkt(buffer, payload_len, 0);


        int datos_enviados_vuelta = ipv4_send(ipv4_layer, direccion_origen, (uint8_t)IPV4_PROTOCOL, buffer, longitud_datos_int);
        if (datos_enviados_vuelta == -1) {
            fprintf(stderr, "%s: ERROR en ipv4_send()\n", myself);
        }
  }


    /* Cerrar interfaz IPv4 */
    printf("Cerrando ipv4 en el servidor.\n");
    int error_cerrando_ipv4=ipv4_close(ipv4_layer);

    if(error_cerrando_ipv4){
        fprintf(stderr, "Error cerrando la capa de ipv4 en el servidor\n");
        return -1;
    }

    return 0;
}






