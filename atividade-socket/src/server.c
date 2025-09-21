#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/wait.h>

#define BACKLOG 10
#define BUFFER_SIZE 1024
#define VERSION "1.0"

int server_fd;

void sigint_handler(int sig) {
    printf("\nServidor encerrando...\n");
    if(server_fd > 0) close(server_fd);
    exit(0);
}

void sigchld_handler(int sig) {
    while(waitpid(-1, NULL, WNOHANG) > 0);
}

int parse_request(char *line, double *a, double *b, char *op_type) {
    char op_str[4];  // ADD, SUB, MUL, DIV
    char extra[64];

    // Tentar primeiro forma obrigatória: OP A B
    int n = sscanf(line, "%3s %lf %lf %63s", op_str, a, b, extra);
    if(n == 3) {
        if(strcmp(op_str, "ADD") == 0) { *op_type='+'; return 1; }
        if(strcmp(op_str, "SUB") == 0) { *op_type='-'; return 1; }
        if(strcmp(op_str, "MUL") == 0) { *op_type='*'; return 1; }
        if(strcmp(op_str, "DIV") == 0) { *op_type='/'; return 1; }
        // Se não for OP válido
        return 0;
    }

    // Bônus: forma infixa A <op> B
    char infix_op;
    n = sscanf(line, "%lf %c %lf %63s", a, &infix_op, b, extra);
    if(n == 3) {
        if(infix_op=='+' || infix_op=='-' || infix_op=='*' || infix_op=='/')
        {
            *op_type = infix_op;
            return 1;
        }
    }

    return 0;
}

int handle_operation(double a, double b, char op, double *result) {
    switch(op) {
        case '+': *result = a + b; return 1;
        case '-': *result = a - b; return 1;
        case '*': *result = a * b; return 1;
        case '/':
            if(b == 0) return 0;
            *result = a / b; return 1;
        default: return 0;
    }
}

void handle_client(int client_fd) {
    char buffer[BUFFER_SIZE];
    int bytes;

    while((bytes = recv(client_fd, buffer, sizeof(buffer)-1, 0)) > 0) {
        buffer[bytes] = '\0';

        // Remover \r\n
        char *p = buffer;
        while(*p) { if(*p=='\r'||*p=='\n') *p='\0'; p++; }

        if(strlen(buffer)==0) continue;

        double a, b, result;
        char op;

        if(parse_request(buffer, &a, &b, &op)) {
            if(op=='/' && b==0) {
                send(client_fd, "ERR EZDV divisao_por_zero\n", 27, 0);
                continue;
            }
            if(handle_operation(a, b, op, &result)) {
                char res_str[64];
                snprintf(res_str, sizeof(res_str), "OK %.6f\n", result);
                send(client_fd, res_str, strlen(res_str), 0);
            } else {
                send(client_fd, "ERR ESRV erro_interno\n", 23, 0);
            }
        } else {
            send(client_fd, "ERR EINV entrada_invalida\n", 27, 0);
        }
    }

    printf("Cliente desconectado\n");
    close(client_fd);
}

int main(int argc, char *argv[]) {
    if(argc != 2) {
        fprintf(stderr, "Uso: %s <porta>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int port = atoi(argv[1]);
    signal(SIGINT, sigint_handler);
    signal(SIGCHLD, sigchld_handler);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(server_fd < 0) { perror("socket"); exit(EXIT_FAILURE); }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    if(bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) { perror("bind"); exit(EXIT_FAILURE); }
    if(listen(server_fd, BACKLOG) < 0) { perror("listen"); exit(EXIT_FAILURE); }

    printf("Servidor ouvindo na porta %d\n", port);

    while(1) {
        struct sockaddr_in client_addr;
        socklen_t addrlen = sizeof(client_addr);
        int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &addrlen);
        if(client_fd < 0) {
            if(errno == EINTR) continue;
            perror("accept");
            continue;
        }

        printf("Conexao de %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        if(fork() == 0) {
            close(server_fd);
            handle_client(client_fd);
            exit(0);
        } else {
            close(client_fd);
        }
    }

    return 0;
}
