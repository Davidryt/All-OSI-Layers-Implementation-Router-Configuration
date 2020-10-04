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
        printf("       <direccion_ip>: La dirección IP destino\n");
        printf("        <long>: Longitud de los datos\n");
        printf("         <fichero.txt>: Fichero de texto del cliente\n");
        printf("        <route_table>: La route table del cliente\n");
        exit(-1);
    }

    /* Procesar los argumentos de la línea de comandos */
    char * direccion_ip_introducida_para_servidor = argv[1];
    char* longitud_datos = argv[2];
    char* ipv4_config_client=argv[3];
    char* ipv4_route_table_client=argv[4];







    /* Enviar trama ipv4 al Servidor */
    printf("Enviando %d bytes al Servidor IPv4 (%s):\n", longitud_datos, direccion_ip_introducida_para_servidor);
  
    err = ipv4_send(eth_iface, server_addr, eth_type, payload, payload_len);
    if (err == -1) {
        fprintf(stderr, "%s: ERROR en eth_send()\n", myself);
        exit(-1);
    }   
  
    /* Recibir trama Ethernet del Servidor y procesar errores */
    long int timeout = 2000;
    len = eth_recv(eth_iface, src_addr, eth_type, buffer, ETH_MTU, timeout);
    if (len == -1) {
        fprintf(stderr, "%s: ERROR en eth_recv()\n", myself);
    } else if (len == 0) {
        fprintf(stderr, "%s: ERROR: No hay respuesta del Servidor Ethernet\n", myself);
    }

    if (len > 0) {
        char src_addr_str[MAC_STR_LENGTH];
        mac_addr_str(src_addr, src_addr_str);    

        printf("Recibidos %d bytes del Servidor Ethernet (%s)\n", len, src_addr_str);
        print_pkt(buffer, len, 0);
    }







    /* Cerrar interfaz IPv4 */
    printf("Cerrando ipv4 en el cliente.\n");
    int error_cerrando_ipv4=ipv4_close(ipv4_layer);

    if(error_cerrando_ipv4){
        fprintf(stderr, "Error cerrando la capa de ipv4 en el cliente\n");
        return -1;
    }

    return 0;

}
