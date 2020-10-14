#include "udp.h"
#include "ipv4.h"
#include "eth.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <libgen.h>


int main ( int argc, char * argv[]){

    /* Mostrar mensaje de ayuda si el número de argumentos es incorrecto */
    char * myself = basename(argv[0]);
    if (argc != 5) {
        printf("Uso: %s <fichero.txt> <route_table> <puerto> <long>\n", myself);
        printf("        <fichero.txt>: Fichero de texto del servidor\n");
        printf("        <route_table>: La route table del servidor\n");
        printf("        <puerto>: El puerto del servidor\n");
        printf("        <long>: Longitud de los datos\n");
        exit(-1);
    }

    /* Procesar los argumentos de la línea de comandos */
    char* ipv4_config_server=argv[1];
    char* ipv4_route_table_server=argv[2];
    char* puerto_servidor = argv[3];
    char* longitud_datos_string = argv[4];

    int puerto_servidor_int = atoi(puerto_servidor); //De string a número
    int longitud_datos_int = atoi(longitud_datos_string); //De string a número


    /* Abrir la interfaz UDP */
    udp_layer_t * udp_layer = udp_open(ipv4_config_server, ipv4_route_table_server, puerto_servidor_int);
    if (udp_layer == NULL) {
        fprintf(stderr, "ERROR en udp_open en el servidor(\"%s\")\n", myself);
        exit(-1);
    }
    printf("Abriendo interfaz UDP en el servidor.\n");


    unsigned char buffer[longitud_datos_int];
    ipv4_addr_t direccion_origen;
    char direccion_origen_string[IPv4_STR_MAX_LENGTH];
       

    while(1) {
        /* Recibir trama UDP del Cliente */
        long int timeout = -1;

        printf("Escuchando tramas UDP (tipo=0x11) ...\n");

        uint16_t puerto_cliente=0;
    
        int payload_len = udp_recv(udp_layer, buffer, &puerto_cliente, direccion_origen, longitud_datos_int, timeout);
        if (payload_len == -1) {
            fprintf(stderr, "%s: ERROR en ipv4_recv()\n", myself);
            exit(-1);
        }

        ipv4_addr_str(direccion_origen, direccion_origen_string);
    
        printf("Recibidos %d bytes del puerto UDP del cliente %d\n", payload_len, puerto_cliente);
        print_pkt(buffer, payload_len, 0);

       
        printf("Enviando %d bytes al Cliente UDP a través del puerto %d\n", payload_len, puerto_cliente);
        print_pkt(buffer, payload_len, 0);

        int datos_enviados_vuelta = udp_send(udp_layer, direccion_origen, puerto_cliente, buffer, longitud_datos_int);
        if (datos_enviados_vuelta == -1) {
            fprintf(stderr, "%s: ERROR en ipv4_send()\n", myself);
        }
    }


    /* Cerrar interfaz IPv4 */
    printf("Cerrando udp en el servidor.\n");
    int error_cerrando_udp=udp_close(udp_layer);

    if(error_cerrando_udp){
        fprintf(stderr, "Error cerrando la capa de udp en el servidor\n");
        return -1;
    }

    return 0;




}
