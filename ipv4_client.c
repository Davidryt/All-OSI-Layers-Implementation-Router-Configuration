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
    char * direccion_ip_introducida = argv[1];
    char* longitud_datos = argv[2];
    char* ipv4_config_client=argv[3];
    char* ipv4_route_table_client=argv[4];





    /* Cerrar interfaz IPv4 */
    printf("Cerrando ipv4 en el cliente.\n");
    int error_cerrando_ipv4=ipv4_close(ipv4_layer);

    if(error_cerrando_ipv4){
        fprintf(stderr, "Error cerrando la capa de ipv4 en el cliente\n");
        return -1;
    }

    return 0;

}
