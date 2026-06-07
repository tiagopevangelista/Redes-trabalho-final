#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "packet.h"

#define PORT 8080

void run_receiver() {
    int sockfd;
    struct sockaddr_in server_addr, client_addr;
    Packet pkt;
    socklen_t addr_len = sizeof(client_addr);

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    bind(sockfd, (const struct sockaddr *)&server_addr, sizeof(server_addr));
    printf("Receptor escutando na porta %d...\n", PORT);

    while(1) {
        recvfrom(sockfd, &pkt, sizeof(Packet), 0, (struct sockaddr *) &client_addr, &addr_len);
        printf("\n--- Pacote Recebido ---\n");
        printf("De: %s\nPara: %s\nTTL: %d\nData: %s\n", pkt.src_ip, pkt.dst_ip, pkt.ttl, pkt.data);
    }
}

void run_sender(const char* router_ip, const char* target_ip) {
    int sockfd;
    struct sockaddr_in router_addr;
    Packet pkt;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    router_addr.sin_family = AF_INET;
    router_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, router_ip, &router_addr.sin_addr);

    // Criar um pacote
    strcpy(pkt.src_ip, "10.0.0.2"); 
    strcpy(pkt.dst_ip, target_ip);
    pkt.ttl = 5;
    strcpy(pkt.data, "Olá do Emisor!");

    printf("Enviando pacote para o roteador (%s) com destino a %s...\n", router_ip, target_ip);
    sendto(sockfd, &pkt, sizeof(Packet), 0, (const struct sockaddr *) &router_addr, sizeof(router_addr));
    close(sockfd);
}

int main(int argc, char *argv[]) {
    if (argc > 1 && strcmp(argv[1], "receber") == 0) {
        run_receiver();
    } else if (argc == 4 && strcmp(argv[1], "enviar") == 0) {
        run_sender(argv[2], argv[3]);
    } else {
        printf("Modo de uso: %s receber | enviar <ip_roteador> <ip_destino>\n", argv[0]);
    }
    return 0;
}