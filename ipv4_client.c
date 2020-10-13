#include "ipv4.h"
#include "eth.h"

#include "ipv4_route_table.h"
#include "ipv4_config.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <libgen.h>



int main ( int argc, char * argv[]){

    /* Mostrar mensaje de ayuda si el número de argumentos es incorrecto */
    char * myself = basename(argv[0]);
    if (argc != 6) {
        printf("Uso: %s <direccion_ip> <fichero.txt> <route_table> <protocolo> <long>\n", myself);
        printf("        <direccion_ip>: La dirección IP destino\n");
        printf("        <fichero.txt>: Fichero de texto del cliente\n");
        printf("        <route_table>: La route table del cliente\n");
        printf("        <protocolo>: El protocolo del cliente\n");
        printf("        <long>: Longitud de los datos\n");
        exit(-1);
    }

    /* Procesar los argumentos de la línea de comandos */
    char* direccion_ip_introducida_para_servidor = argv[1];
    char* ipv4_config_client=argv[2];
    char* ipv4_route_table_client=argv[3];
    char* protocolo_cliente=argv[4];
    char* longitud_datos_string = argv[5];

    int protocolo_cliente_int = atoi(protocolo_cliente);
    int longitud_datos_int = atoi(longitud_datos_string); //De string a número

    ipv4_addr_t ip_destino;
    int comprobar_ip=ipv4_str_addr(direccion_ip_introducida_para_servidor, ip_destino);

    if(comprobar_ip==-1){
        fprintf(stderr, "ERROR en la dirección IP introducida\n");
        exit(-1); 
    }


    /* Abrir la interfaz IPv4 */
    ipv4_layer_t * ipv4_layer=ipv4_open(ipv4_config_client, ipv4_route_table_client);
    if (ipv4_layer == NULL) {
        fprintf(stderr, "%s: ERROR en ipv4_open en el cliente\n", myself);
        exit(-1);
    }



/*char direccion_nuestra_string[IPv4_STR_MAX_LENGTH];
ipv4_addr_str(ipv4_layer->addr, direccion_nuestra_string);
printf("Enviando al Cliente Ethernet (%s):\n", direccion_nuestra_string);*/



    /* Generar payload */
    unsigned char payload[longitud_datos_int];
    for (int i=0; i<longitud_datos_int; i++) {
        payload[i] = (unsigned char) i;
    }


    /* Enviar trama ipv4 al Servidor */
    printf("Enviando %d bytes al Servidor IPv4 (%s):\n", longitud_datos_int, direccion_ip_introducida_para_servidor);
  
    int bytes_enviados = ipv4_send(ipv4_layer, ip_destino, (uint8_t)protocolo_cliente_int, payload, longitud_datos_int);
    if (bytes_enviados < 0) {
        fprintf(stderr, "ERROR en ipv4_send()\n");
        exit(-1);
    }
    else if(bytes_enviados>0){  //imprimirlo por pantalla
        printf("Se han enviado en ipv4_send(): %d\n", bytes_enviados);
        
    }   


  
    /* Recibir trama IPv4 del Servidor y procesar errores */
    long int timeout = 10000;
    unsigned char buffer[longitud_datos_int];
    ipv4_addr_t ip_origen_envio_paquete_ip;

    int longitud_datos_recibidos = ipv4_recv(ipv4_layer, (uint8_t)protocolo_cliente_int, buffer, ip_origen_envio_paquete_ip, longitud_datos_int, timeout); 
    if (longitud_datos_recibidos<0) {
        fprintf(stderr, "%s: ERROR en ipv4_recv()\n", myself);
    } else if (longitud_datos_recibidos == 0) {
        fprintf(stderr, "%s: El timeout ha finalizado en ipv4_recv\n", myself);
    }
    if (longitud_datos_recibidos > 0) { 
        char direccion_origen_string[IPv4_STR_MAX_LENGTH];
        ipv4_addr_str(ip_origen_envio_paquete_ip, direccion_origen_string);
        printf("Recibidos %d bytes del Servidor IPv4 (%s)\n", longitud_datos_recibidos, direccion_origen_string);
        print_pkt(buffer, longitud_datos_recibidos, 0); 
        
    }


    /* Cerrar interfaz IPv4 */
    printf("Cerrando ipv4 en el cliente.\n");
    int error_cerrando_ipv4=ipv4_close(ipv4_layer);

    if(error_cerrando_ipv4==-1){
        fprintf(stderr, "Error cerrando la capa de ipv4 en el cliente\n");
        return -1;
    }

    return 0;

}
