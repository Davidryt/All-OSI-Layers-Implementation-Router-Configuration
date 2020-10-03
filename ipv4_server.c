#include "ipv4.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <libgen.h>



int main ( int argc, char * argv[]){

    /* Mostrar mensaje de ayuda si el número de argumentos es incorrecto */
    char * myself = basename(argv[0]);
    if (argc != 5) {
        printf("Uso: %s <iface> <tipo> <mac> [<long>]\n", myself);
        printf("       <direccion_ip>: La dirección IP\n");
        printf("        <long>: Longitud de los datos\n");
        printf("         <fichero.txt>: Fichero d etexto del cliente\n");
        printf("        <route_table>: La route table del cliente\n");
        exit(-1);
    }


    /* Procesar los argumentos de la línea de comandos */
    char* ipv4_config_server=argv[2];
    char* ipv4_route_table_server=argv[3];



    /* Abrir la interfaz IPv4 */
    ipv4_layer_t * ipv4_layer = ipv4_open(ipv4_config_server, ipv4_route_table_server);
    if (ipv4_layer == NULL) {
        fprintf(stderr, "ERROR en ipv4_open(\"%s\")\n", ipv4_layer);
        exit(-1);
    }
  
    printf("Abriendo interfaz IPv4 %s.\n");






    /* Cerrar interfaz IPv4 */
    printf("Cerrando ipv4 en el servidor.\n");
    int error_cerrando_ipv4=ipv4_close(ipv4_layer);

    if(error_cerrando_ipv4){
        fprintf(stderr, "Error cerrando la capa de ipv4 en el servidor\n");
        return -1;
    }

    return 0;
}






