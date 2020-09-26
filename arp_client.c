#include "arp.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <libgen.h>

int main ( int argc, char * argv[] ){

    /* Mostrar mensaje de ayuda si el número de argumentos es incorrecto */
    char * myself = basename(argv[0]);
    if (argc != 3) {
        printf("Uso: %s <iface> <ip destino> [<long>]\n", myself);
        printf("       <iface>: Nombre de la interfaz Ethernet\n");
        printf("         <ip>: Dirección IP del servidor Ethernet\n");
        exit(-1);
    }

}
