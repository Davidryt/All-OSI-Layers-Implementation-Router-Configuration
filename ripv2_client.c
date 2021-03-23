#include "udp.h"
#include "ipv4.h"
#include "eth.h"
#include "ripv2.h"

#include "ripv2_route_table.h"
#include "ripv2_config.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <libgen.h>


int main ( int argc, char * argv[]){

    /* Mostrar mensaje de ayuda si el número de argumentos es incorrecto */
    char * myself = basename(argv[0]);
    if (argc != 6) {
        printf("Uso: %s <fichero.txt> <fichero.txt> <direccion_ip>\n", myself);
        printf("        <fichero.txt>: Fichero de texto del cliente\n");
        printf("        <fichero.txt>: Fichero de texto del cliente\n");
        printf("        <direccion_ip>: La dirección IP destino\n");
        exit(-1);
    }

    /* Procesar los argumentos de la línea de comandos */
    char* ripv2_config_client=argv[1];
    char* ripv2_route_table_client=argv[2];
    char* direccion_ip_introducida_para_servidor = argv[3];

    ipv4_addr_t ip_destino;
    int comprobar_ip=ipv4_str_addr(direccion_ip_introducida_para_servidor, ip_destino);

    if(comprobar_ip==-1){
        fprintf(stderr, "ERROR en la dirección IP introducida\n");
        exit(-1); 
    }



    /* Abrir la interfaz UDP */
    udp_layer_t * udp_layer=udp_open(ripv2_config_client, ripv2_route_table_client, PUERTO_RIP);
    if (udp_layer == NULL) {
        fprintf(stderr, "%s: ERROR en udp_open en el cliente\n", myself);
        exit(-1);
    }



    /* Crear la Trama UDP y rellenar todos los campos */
    struct rip_frame rip_frame;

    rip_frame.tipo_request_response=REQUEST;
    rip_frame.version=VERSION;
    rip_frame.dominio_de_encaminamiento=0x0000;

    ripv2_distance_vector_t vector_distancia; 

    vector_distancia.familia_de_direcciones=0;
    vector_distancia.etiqueta_ruta=0x0000;
    memcpy(vector_distancia.direcccion_ipv4, IPv4_ZERO_ADDR, IPv4_ADDR_SIZE);
    memcpy(vector_distancia.netmask, IPv4_ZERO_ADDR, IPv4_ADDR_SIZE);
    memcpy(vector_distancia.siguiente_salto, IPv4_ZERO_ADDR, IPv4_ADDR_SIZE);
    vector_distancia.metrica=INFINITO;
    
    rip_frame.entradas[0]=vector_distancia;

    int longitud=RIP_HEADER_SIZE+LONGITUD_ENTRADA;
  
    int bytes_enviados = udp_send(udp_layer, ip_destino, (uint16_t) PUERTO_RIP, (unsigned char*) rip_frame, longitud);  //Preguntar el puerto

    if (bytes_enviados < 0) {
        fprintf(stderr, "ERROR en udp_send() en ripv2 cliente\n");
        exit(-1);
    }  
    else{
        System.out.print("Los bytes_enviados en ripv2 por udp_send() son mayores que 0\n");
    }


    uint16_t puerto_cliente=0; 
  
    /* Recibir trama UDP del Servidor y procesar errores */
    unsigned char buffer[PAQUETE_TOTAL_RIP_MAS_CABECERA];


    while(1){

        int longitud_datos_recibidos = udp_recv(udp_layer, buffer, &puerto_cliente, ip_destino, PAQUETE_TOTAL_RIP_MAS_CABECERA, TEMPORIZADOR); 
        if (longitud_datos_recibidos<0) {
            fprintf(stderr, "%s: ERROR en udp_recv() en el cliente ripv2\n", myself);
        } else if (longitud_datos_recibidos == 0) {
        fprintf(stderr, "%s: El timeout ha finalizado en udp_recv en el cliente ripv2\n", myself);
        }
        if (longitud_datos_recibidos > 0) { 
            printf("Datos recibidos por udp_recv() en el cliente ripv2\n");
            print_pkt(buffer, longitud_datos_recibidos, 0); 
        }

        struct rip_frame *frame=(struct rip_frame *)&buffer;

        imprimir_rip_frame(frame, longitud_datos_recibidos);

    }


    /* Cerrar interfaz UDP */
    printf("Cerrando udp en el cliente ripv2.\n");
    int error_cerrando_udp=udp_close(udp_layer);

    if(error_cerrando_udp==-1){
        fprintf(stderr, "Error cerrando la capa de udp en el cliente\n");
        return -1;
    }

    return 0;

}
