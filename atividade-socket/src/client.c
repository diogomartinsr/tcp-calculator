#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    if(argc != 3) {
        fprintf(stderr, "Uso: %s <IP> <porta>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char *ip = argv[1];
    int port = atoi(argv[2]);

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock < 0) { perror("socket"); exit(EXIT_FAILURE); }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    if(inet_pton(AF_INET, ip, &server_addr.sin_addr) <= 0) {
        perror("inet_pton"); exit(EXIT_FAILURE);
    }

    if(connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect"); exit(EXIT_FAILURE);
    }

    char buffer[BUFFER_SIZE];
    while(1) {
        printf("> ");
        if(!fgets(buffer, sizeof(buffer), stdin)) break;
        send(sock, buffer, strlen(buffer), 0);

        int bytes = recv(sock, buffer, sizeof(buffer)-1, 0);
        if(bytes <= 0) {
            printf("Servidor desconectado\n");
            break;
        }

        buffer[bytes] = '\0';
        printf("%s", buffer);
    }

    close(sock);
    return 0;
}
