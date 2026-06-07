#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "packet.h"

#define PORT 8080

// TODO: Implementar estruturas de tabela de roteamento
// TODO: Implementar estruturas de tabela ARP

int main() {
    int sockfd;
    struct sockaddr_in router_addr, next_hop_addr;
    Packet pkt;
    socklen_t addr_len = sizeof(struct sockaddr_in);

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    router_addr.sin_family = AF_INET;
    router_addr.sin_addr.s_addr = INADDR_ANY;
    router_addr.sin_port = htons(PORT);

    bind(sockfd, (const struct sockaddr *)&router_addr, sizeof(router_addr));
    printf("Router online e escutando na porta %d...\n", PORT);

    while(1) {
        struct sockaddr_in client_addr;
        recvfrom(sockfd, &pkt, sizeof(Packet), 0, (struct sockaddr *) &client_addr, &addr_len);
        
        printf("\n[Router] Pacote recebido destinado a %s. TTL atual: %d\n", pkt.dst_ip, pkt.ttl);

        // 1. Check de TTL 
        pkt.ttl--;
        if (pkt.ttl <= 0) {
            printf("[Router] Pacote descartado! TTL expirado.\n");
            continue;
        }

        // 2. Tabela de roteamento Lookup 
        // TODO: Fazer o lookup na tabela de roteamento
        char next_hop_ip[16] = "10.0.0.4"; // Assumindo .4 como nosso receiver no Docker
        printf("[Router] Rota encontrada. Próximo salto: %s\n", next_hop_ip);

        // 3. Resolução ARP 
        // TODO: Fazer o lookup na tabela ARP

        // 4. Forwarding
        next_hop_addr.sin_family = AF_INET;
        next_hop_addr.sin_port = htons(PORT);
        inet_pton(AF_INET, next_hop_ip, &next_hop_addr.sin_addr);

        printf("[Router] Forwarding pacote para %s... Novo TTL: %d\n", next_hop_ip, pkt.ttl);
        sendto(sockfd, &pkt, sizeof(Packet), 0, (const struct sockaddr *) &next_hop_addr, sizeof(next_hop_addr));
    }
    return 0;
}