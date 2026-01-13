#ifndef NETWORK_H
#define NETWORK_H

#include "../Uefi.Core/include/uefi.h"

typedef struct {
    EFI_HANDLE device;
    EFI_SIMPLE_NETWORK_PROTOCOL* snp;
    EFI_MAC_ADDRESS mac_address;
    UINT32 max_packet_size;
    UINTN media_state;
} NetworkInterface;

typedef struct {
    uint8_t data[1500];
    size_t length;
} NetworkPacket;

typedef struct {
    uint8_t src_mac[6];
    uint8_t dst_mac[6];
    uint16_t ethertype;
    uint8_t payload[];
} EthernetFrame;

typedef struct {
    uint8_t version_ihl;
    uint8_t tos;
    uint16_t total_length;
    uint16_t identification;
    uint16_t flags_fragment;
    uint8_t ttl;
    uint8_t protocol;
    uint16_t header_checksum;
    uint32_t src_ip;
    uint32_t dst_ip;
    uint8_t payload[];
} IPv4Header;

typedef struct {
    uint16_t src_port;
    uint16_t dst_port;
    uint16_t length;
    uint16_t checksum;
    uint8_t payload[];
} UDPHeader;

typedef struct {
    uint32_t sequence_num;
    uint32_t ack_num;
    uint16_t flags;
    uint16_t window_size;
    uint16_t checksum;
    uint16_t urgent_ptr;
    uint8_t payload[];
} TCPHeader;

int network_init(NetworkInterface* iface, EFI_HANDLE device);
int network_start(NetworkInterface* iface);
int network_stop(NetworkInterface* iface);
int network_get_mac_address(NetworkInterface* iface, uint8_t mac[6]);
int network_send_packet(NetworkInterface* iface, const void* data, size_t size);
int network_receive_packet(NetworkInterface* iface, NetworkPacket* packet);
int network_set_ip_address(NetworkInterface* iface, uint32_t ip_address, uint32_t subnet_mask);
int network_set_gateway(NetworkInterface* iface, uint32_t gateway);
int network_set_dns(NetworkInterface* iface, uint32_t dns_server);

uint16_t network_checksum(const void* data, size_t length);
uint32_t network_ip_from_string(const char* ip_string);
void network_ip_to_string(uint32_t ip_address, char* buffer);

#define ETH_TYPE_IP    0x0800
#define ETH_TYPE_ARP   0x0806
#define ETH_TYPE_IPV6  0x86DD
#define IP_PROTOCOL_TCP 6
#define IP_PROTOCOL_UDP 17

#endif
