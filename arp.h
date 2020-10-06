#include "eth.h"
#include "ipv4.h"

//Protocolo de IPv4
#define IPV4_PROTOCOL 0x800

//void ipv4_addr_str(ipv4_addr_t addr , char str[]);
//int ipv4_str_addr( char * str , ipv4_addr_t addr);
int arp_resolve (eth_iface_t * iface, ipv4_addr_t direccion_ip_origen, ipv4_addr_t dest, mac_addr_t mac);
