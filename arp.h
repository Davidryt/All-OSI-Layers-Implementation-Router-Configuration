#include "eth.h"
#include "ipv4.h"

/* Tamaño de la cabecera ARP (sin incluir el campo FCS) */
#define ARP_FULL_LENGTH 28
/* Tamaño máximo de una trama Ethernet (sin incluir el campo FCS) */
#define ETH_FRAME_MAX_LENGTH (ETH_HEADER_SIZE + ETH_MTU)

//El hardware type que siempre es 1 en ethernet
#define HARDWARE_TYPE 1

//Protocolo de IPv4
#define IPV4_PROTOCOL 0x800

//Protocolo de ARP
#define ARP_PROTOCOL 0x806

//El operation type de ARP -> 1 va a ser para request
#define OPERATION_CODE_REQUEST_ARP 1

//El operation type de ARP -> 2 va a ser para reply
#define OPERATION_CODE_REPLY_ARP 2

//void ipv4_addr_str(ipv4_addr_t addr , char str[]);
//int ipv4_str_addr( char * str , ipv4_addr_t addr);
int arp_resolve (eth_iface_t * iface, ipv4_addr_t dest, mac_addr_t mac);
