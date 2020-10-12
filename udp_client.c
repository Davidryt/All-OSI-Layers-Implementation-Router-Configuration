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
        printf("Uso: %s <direccion_ip> <fichero.txt> <route_table> <long>\n", myself);
        printf("        <direccion_ip>: La dirección IP destino\n");
        printf("        <fichero.txt>: Fichero de texto del cliente\n");
        printf("        <route_table>: La route table del cliente\n");
        printf("        <long>: Longitud de los datos\n");
        exit(-1);
    }

    /* Procesar los argumentos de la línea de comandos */
    char* direccion_ip_introducida_para_servidor = argv[1];
    char* ipv4_config_client=argv[2];
    char* ipv4_route_table_client=argv[3];
    char* longitud_datos_string = argv[4];

    int longitud_datos_int = atoi(longitud_datos_string); //De string a número

    ipv4_addr_t ip_destino;
    int comprobar_ip=ipv4_str_addr(direccion_ip_introducida_para_servidor, ip_destino);

    if(comprobar_ip==-1){
        fprintf(stderr, "ERROR en la dirección IP introducida\n");
        exit(-1); 
    }

    //Generar un puerto aleatorio
    uint16_t puerto_origen=funcion_numero_aleatorio_puerto();
    printf("El puerto origen en el cliente UDP es: %d\n", puerto_origen);

    /* Abrir la interfaz UDP */
    udp_layer_t * udp_layer=udp_open(ipv4_config_client, ipv4_route_table_client, puerto_origen);
    if (udp_layer == NULL) {
        fprintf(stderr, "%s: ERROR en udp_open en el cliente\n", myself);
        exit(-1);
    }

    /* Generar payload */
    unsigned char payload[longitud_datos_int];
    for (int i=0; i<longitud_datos_int; i++) {
        payload[i] = (unsigned char) i;
    }

    /* Enviar trama udp al Servidor */
    printf("Enviando %d bytes al Servidor UDP (%s):\n", longitud_datos_int, direccion_ip_introducida_para_servidor);
  
    int bytes_enviados = udp_send();
    if (bytes_enviados < 0) {
        fprintf(stderr, "ERROR en udp_send()\n");
        exit(-1);
    }
    else if(bytes_enviados>0){  //imprimirlo por pantalla
        printf("Se han enviado en udp_send(): %d\n", bytes_enviados);
    }   


  
    /* Recibir trama UDP del Servidor y procesar errores */
    long int timeout = 10000;
    unsigned char buffer[longitud_datos_int];
    ipv4_addr_t ip_origen_envio_paquete_ip;

    int longitud_datos_recibidos = udp_recv(); 
    if (longitud_datos_recibidos<0) {
        fprintf(stderr, "%s: ERROR en udp_recv()\n", myself);
    } else if (longitud_datos_recibidos == 0) {
        fprintf(stderr, "%s: El timeout ha finalizado en udp_recv\n", myself);
    }
    if (longitud_datos_recibidos > 0) { 
        char direccion_origen_string[IPv4_STR_MAX_LENGTH];
        ipv4_addr_str(ip_origen_envio_paquete_ip, direccion_origen_string);
        printf("Recibidos %d bytes del Servidor IPv4 (%s)\n", longitud_datos_recibidos, direccion_origen_string);
        print_pkt(buffer, longitud_datos_recibidos, 0); 
    }


    /* Cerrar interfaz UDP */
    printf("Cerrando udp en el cliente.\n");
    int error_cerrando_udp=udp_close(udp_layer);

    if(error_cerrando_udp==-1){
        fprintf(stderr, "Error cerrando la capa de udp en el cliente\n");
        return -1;
    }

    return 0;




}

