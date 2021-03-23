#include "udp.h"
#include "ipv4.h"
#include "eth.h"
#include "ripv2.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <libgen.h>


int main ( int argc, char * argv[]){

    /* Mostrar mensaje de ayuda si el número de argumentos es incorrecto */
    char * myself = basename(argv[0]);
    if (argc != 5) {
        printf("Uso: %s <fichero.txt> <fichero.txt>\n", myself);
        printf("        <fichero.txt>: Fichero de texto del servidor\n");
        printf("        <fichero.txt>: Fichero de texto del servidor\n");
        exit(-1);
    }

    /* Procesar los argumentos de la línea de comandos */
    char* ripv2_config_server=argv[1];
    char* ripv2_route_table_server=argv[2];



    /* Abrir la interfaz UDP */
    udp_layer_t * udp_layer = udp_open(ripv2_config_server, ripv2_route_table_server, PUERTO_RIP);
    if (udp_layer == NULL) {
        fprintf(stderr, "ERROR en udp_open en el servidor ripv2(\"%s\")\n", myself);
        exit(-1);
    }
    printf("Abriendo interfaz UDP en el servidor ripv2.\n");


    /* 1. Crear la tabla de rutas RIP
       2. Leer fichero tabla de rutas y rellenar
    */

    ripv2_route_t route_table_rip=ripv2_route_table_create();

    if(route_table_rip == NULL){
        fprintf(stderr, "Error en el servidor ripv2: ERROR en ripv2_route_create()\n");
    }

    ripv2_route_table_read (ripv2_route_table_server, route_table_rip);

    struct rip_frame rip_frame;
    ripv2_distance_vector_t vector_distancia;

    struct rip_frame * rip_frame_ptr = NULL;



    /*Hacemos un while, hacemos un recibir con udp. Si todo sale bien creamos paquete de respuesta
    si el paquete es request creo el paquete response y lo enviamos. Luego cerramos udp_close()
    Si es un response paso, se descartan, solo interesan los response
    */

    unsigned char buffer[PAQUETE_TOTAL_RIP_MAS_CABECERA];

    /* Inicializar temporizador para mantener timeout si se reciben tramas con tipo incorrecto. */
    timerms_t timer;
    timerms_reset(&timer, TEMPORIZADOR);
    long int time_left;

    ripv2_route_t ruta;
    long int tiempo_restante_ruta;
    uint16_t puerto_cliente=0;
    ipv4_addr_t direccion_origen;
       
    while(1) {

        printf("Escuchando tramas RIP ...\n");
        time_left = timerms_left(&timer);
       
        for(int i=0; i<LONGITUD_MAXIMA_ENTRADAS, i++){
            ruta=ripv2_route_table_get(route_table_rip, i);

            if(ruta==NULL)
            else{
                tiempo_restante_ruta=timerms_left(&ruta->temporizador);

                if(tiempo_restante_ruta<=0){ //DUDASSSSSS
                    
                }             
            }
        }


    
        int payload_len = udp_recv(udp_layer, buffer, &puerto_cliente, direccion_origen, PAQUETE_TOTAL_RIP_MAS_CABECERA, TEMPORIZADOR);
        if (payload_len == -1) {
            fprintf(stderr, "%s: ERROR en udp_recv() en el servidor ripv2\n", myself);
            exit(-1);
        }

        ipv4_addr_str(direccion_origen, direccion_origen_string);
    
        //printf("Recibidos %d bytes del puerto UDP del cliente %d\n", payload_len, puerto_cliente);
        print_pkt(buffer, PAQUETE_TOTAL_RIP_MAS_CABECERA, 0);


        /* Comprobar si es la trama que estamos buscando */
        rip_frame_ptr = (struct rip_frame *) buffer;



        //Comprobación de si es request o response

        //Si es un REQUEST
        if(((rip_frame_ptr->tipo_request_response)==REQUEST) && (ntohs(rip_frame_ptr->familia_de_direcciones)==0) && (ntohl(rip_frame_ptr->metrica)==16)){ 
            printf("Modo REQUEST \n");

            struct rip_frame rip_frame;

            rip_frame.tipo_request_response=RESPONSE;
            rip_frame.version=VERSION;
            rip_frame.dominio_de_encaminamiento=0x0000;

            ripv2_distance_vector_t vector_distancia; 

            ripv2_route_t *auxiliar_ruta;

            for(int i=0; i<25; i++){  //PREGUNTAR SI ES 24 O 25

                auxiliar_ruta=ripv2_route_table_get(route_table_rip, i);
                
                if(auxiliar_ruta != NULL){
                    vector_distancia.familia_de_direcciones=htons(2);
                    vector_distancia.etiqueta_ruta=htons(0x0000);
                    memcpy(vector_distancia.direcccion_ipv4, auxiliar_ruta->subnet_addr, IPv4_ADDR_SIZE);
                    memcpy(vector_distancia.netmask, auxiliar_ruta->subnet_mask, IPv4_ADDR_SIZE);
                    memcpy(vector_distancia.siguiente_salto, auxiliar_ruta->gateway_addr, IPv4_ADDR_SIZE);
                    vector_distancia.metrica=htonl(auxiliar_ruta->metrica);

                    rip_frame.entradas[i]=vector_distancia;
                }
                else{
                    break;
                }

            }

            int longitud=funcion_ripv2_entradas_a_longitud(i);
  
            int bytes_enviados = udp_send(udp_layer, ip_destino, (uint16_t) PUERTO_RIP, (unsigned char*) rip_frame, longitud); //PREGUNTAR el puerto

            if (bytes_enviados < 0) {
                fprintf(stderr, "ERROR en udp_send() en ripv2 servidor\n");
                exit(-1);
            }  
            else{
                System.out.print("Los bytes_enviados en ripv2 en el servidor por udp_send() son mayores que 0\n");
            }
            

        }


        //Si es un RESPONSE
        if(((rip_frame_ptr->tipo_request_response)==RESPONSE)){ 
            printf("Modo RESPONSE \n");

            struct rip_frame rip_frame;

            rip_frame.tipo_request_response=RESPONSE;
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
            

        }



       
        printf("Enviando %d bytes al Cliente UDP a través del puerto %d\n", payload_len, puerto_cliente);
        print_pkt(buffer, payload_len, 0);

        int datos_enviados_vuelta = udp_send(udp_layer, direccion_origen, puerto_cliente, buffer, longitud_datos_int);
        if (datos_enviados_vuelta == -1) {
            fprintf(stderr, "%s: ERROR en ipv4_send()\n", myself);
        }
    }


    /* Cerrar interfaz UDP */
    printf("Cerrando udp en el servidor ripv2.\n");
    int error_cerrando_udp=udp_close(udp_layer);

    if(error_cerrando_udp){
        fprintf(stderr, "Error cerrando la capa de udp en el servidor\n");
        return -1;
    }

    return 0;

}
