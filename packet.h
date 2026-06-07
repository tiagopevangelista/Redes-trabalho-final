#ifndef PACKET_H
#define PACKET_H

#include <stdint.h>

typedef struct {
    char src_ip[16];
    char dst_ip[16];
    int ttl;
    char data[256];
} Packet;

#endif