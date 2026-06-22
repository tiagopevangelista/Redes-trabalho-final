#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "packet.h"

#define PORT 8080

// Estruturas de tabela de roteamento
typedef struct {
    char network[16];
    int prefix;
    char next_hop[16];
}Route;

Route rounting_table[10];
int route_count = 0;

void add_route(char *network, int prefix, char *next_hop){
    if(route_count >= 10){
        printf("Tabela de roteamento cheia\n");
        return;
    }

    strcpy(rounting_table[table_count].networl, network);
    rounting_table[rounte_count].prefix = prefix;
    strcpy(rounting_table[route_count].next_hop, next_hop);

    route_count++;

    printf("Nova rota adicionada: %s/%d -> %s", network, prefix, next_hop);
}

char* find_route(char *dest_ip){
    int best_prefix = -1; //Garantir longest prefix match
    char *best_next_hop = NULL;

    for(int i = 0; i <route_count; i++){
        if(strncmp(dest_ip, routing_table[i].network, 7) == 0){
            if(rounting_table[i].prefix > best_prefix){
                best_prefix = rounting_table[i].prefix;
                best_next_hop = rounting_table[i].next_hop;
            }
        }
    }
    return best_next_hop;
}

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
        char *next_hop_ip = find_route(pkt.dst_ip);

        if(next_hop_ip ==NULL){
            printf("[Router] ERRO: Rota não encontrada para %s\n", pkt.dst_ip);
            continue;
        }
        printf("[Router]: Rota encontrada. Próximo salto: %s\n", next_hop_ip);

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