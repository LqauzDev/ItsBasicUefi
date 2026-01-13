#include "network.h"
#include "../Uefi.Core/include/uefi_lib.h"
#include "../Uefi.StdLib/include/stdlib.h"
#include "../Uefi.StdLib/include/string.h"

int network_init(NetworkInterface* iface, EFI_HANDLE device) {
    if (!iface || !device) return 0;
    
    memset(iface, 0, sizeof(NetworkInterface));
    iface->device = device;
    
    EFI_GUID snp_guid = EFI_SIMPLE_NETWORK_PROTOCOL_GUID;
    EFI_STATUS status = gBS->HandleProtocol(device, &snp_guid, (void**)&iface->snp);
    
    if (EFI_ERROR(status)) return 0;
    
    if (iface->snp->Mode->State == EfiSimpleNetworkStopped) {
        status = iface->snp->Start(iface->snp);
        if (EFI_ERROR(status)) return 0;
    }
    
    if (iface->snp->Mode->State == EfiSimpleNetworkInitialized) {
        status = iface->snp->Shutdown(iface->snp);
        if (EFI_ERROR(status)) return 0;
    }
    
    status = iface->snp->Initialize(iface->snp, 0, 0);
    if (EFI_ERROR(status)) return 0;
    
    memcpy(&iface->mac_address, &iface->snp->Mode->CurrentAddress, sizeof(EFI_MAC_ADDRESS));
    iface->max_packet_size = iface->snp->Mode->MaxPacketSize;
    iface->media_state = iface->snp->Mode->MediaPresent;
    
    return 1;
}

int network_start(NetworkInterface* iface) {
    if (!iface || !iface->snp) return 0;
    
    EFI_STATUS status = iface->snp->Start(iface->snp);
    if (EFI_ERROR(status)) return 0;
    
    status = iface->snp->Initialize(iface->snp, 0, 0);
    return !EFI_ERROR(status);
}

int network_stop(NetworkInterface* iface) {
    if (!iface || !iface->snp) return 0;
    
    EFI_STATUS status = iface->snp->Shutdown(iface->snp);
    status = iface->snp->Stop(iface->snp);
    
    return !EFI_ERROR(status);
}

int network_get_mac_address(NetworkInterface* iface, uint8_t mac[6]) {
    if (!iface || !mac) return 0;
    
    memcpy(mac, &iface->mac_address, 6);
    return 1;
}

int network_send_packet(NetworkInterface* iface, const void* data, size_t size) {
    if (!iface || !iface->snp || !data || size == 0) return 0;
    
    if (size > iface->max_packet_size) return 0;
    
    void* buffer = NULL;
    EFI_STATUS status = gBS->AllocatePool(EfiLoaderData, size, &buffer);
    if (EFI_ERROR(status)) return 0;
    
    memcpy(buffer, data, size);
    
    UINTN header_size = 0;
    status = iface->snp->Transmit(iface->snp, 0, size, buffer, &header_size, NULL, NULL);
    
    gBS->FreePool(buffer);
    
    if (EFI_ERROR(status)) return 0;
    
    while (iface->snp->GetStatus(iface->snp, NULL, NULL) == EFI_NOT_READY) {
        uefi_delay(1000);
    }
    
    return 1;
}

int network_receive_packet(NetworkInterface* iface, NetworkPacket* packet) {
    if (!iface || !iface->snp || !packet) return 0;
    
    UINTN size = sizeof(packet->data);
    EFI_STATUS status = iface->snp->Receive(iface->snp, NULL, &size, packet->data, NULL, NULL, NULL);
    
    if (EFI_ERROR(status)) return 0;
    
    packet->length = size;
    return 1;
}

int network_set_ip_address(NetworkInterface* iface, uint32_t ip_address, uint32_t subnet_mask) {
    return 1;
}

int network_set_gateway(NetworkInterface* iface, uint32_t gateway) {
    return 1;
}

int network_set_dns(NetworkInterface* iface, uint32_t dns_server) {
    return 1;
}

uint16_t network_checksum(const void* data, size_t length) {
    const uint16_t* ptr = (const uint16_t*)data;
    uint32_t sum = 0;
    
    while (length > 1) {
        sum += *ptr++;
        length -= 2;
    }
    
    if (length > 0) {
        sum += *(uint8_t*)ptr;
    }
    
    while (sum >> 16) {
        sum = (sum & 0xFFFF) + (sum >> 16);
    }
    
    return (uint16_t)(~sum);
}

uint32_t network_ip_from_string(const char* ip_string) {
    if (!ip_string) return 0;
    
    uint32_t ip = 0;
    int octet = 0;
    int octet_count = 0;
    
    while (*ip_string) {
        if (*ip_string == '.') {
            ip = (ip << 8) | octet;
            octet = 0;
            octet_count++;
        } else if (*ip_string >= '0' && *ip_string <= '9') {
            octet = octet * 10 + (*ip_string - '0');
        }
        ip_string++;
    }
    
    ip = (ip << 8) | octet;
    return ip;
}

void network_ip_to_string(uint32_t ip_address, char* buffer) {
    if (!buffer) return;
    
    sprintf(buffer, "%d.%d.%d.%d", 
            (ip_address >> 24) & 0xFF,
            (ip_address >> 16) & 0xFF,
            (ip_address >> 8) & 0xFF,
            ip_address & 0xFF);
}
