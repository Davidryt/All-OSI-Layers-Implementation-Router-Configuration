#include "arp.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <libgen.h>

int main (int argc, char * argv[] ){

    ipv4_addr_t ip_destino; //La ip destino que luego pasaremos en la función arp_resolve
    ipv4_addr_t ip_origen;

    /* Mostrar mensaje de ayuda si el número de argumentos es incorrecto */
    char * myself = basename(argv[0]);
    if (argc != 4) {
        printf("Uso: %s <iface> <ip_destino> <ip_origen>\n", myself);
        printf("       <iface>: Nombre de la interfaz Ethernet\n");
        printf("         <ip_destino>: Dirección IP del servidor\n");
        printf("         <ip_origen>: Dirección IP del origen\n");
        exit(-1);
    }

    /* Procesar los argumentos de la línea de comandos */
    char* iface_name = argv[1];
    char* direccion_ip = argv[2];
    char* direccion_ip_origen = argv[3];

    int comprobar_ip=ipv4_str_addr(direccion_ip, ip_destino);

    if(comprobar_ip==-1){
        fprintf(stderr, "ERROR en la dirección IP destino introducida\n");
        exit(-1); 
    }


    int comprobar2_ip=ipv4_str_addr(direccion_ip_origen, ip_origen);

    if(comprobar2_ip==-1){
        fprintf(stderr, "ERROR en la dirección IP origen introducida\n");
        exit(-1); 
    }


    /* Abrir la interfaz Ethernet */
    eth_iface_t * eth_iface = eth_open(iface_name);
    if (eth_iface == NULL) {
        fprintf(stderr, "ERROR en eth_open\n");
        exit(-1);
    }

    mac_addr_t mac_empty_rellena_arp_c;

    int resultado_funcion_resolve=arp_resolve(eth_iface, ip_origen, ip_destino, mac_empty_rellena_arp_c);

    char string_MAC_empty_rellena[MAC_ADDR_SIZE];

    if(resultado_funcion_resolve < 0){
        fprintf(stderr, "ERROR en la función ARP resolve\n");
        exit(-1);
    }
    else if(resultado_funcion_resolve == 0){
        fprintf(stderr, "El timeout ha finalizado\n");
    }
    else{
        mac_addr_str(mac_empty_rellena_arp_c, string_MAC_empty_rellena);
        printf("La dirección MAC recibida es: %s\n", string_MAC_empty_rellena);
    }
   

    /* Cerrar interfaz Ethernet */
    printf("Cerrando interfaz Ethernet %s.\n", iface_name);
    int error_cerrar=eth_close(eth_iface);

    if(error_cerrar==-1){
        fprintf(stderr, "Error con eth_close en arp\n");
    }

    return 0;

}
