#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <stdlib.h>
#include <sys/time.h>
#include <arpa/inet.h>

#include "tcp_server_ui.h"
#include "data_seed.h"
#include "../shared/protocol.h"

/* Retorna um file descriptor do socket criado para o servidor. */
int prepare_server_socket(struct addrinfo *res) {
    
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);

    setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &(int){ 1 }, sizeof(int));

    struct sockaddr_in address;

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(sock_fd, (struct sockaddr*) &address, sizeof(address)) != 0) {
        perror("Falha no servidor");
        exit(1);
    }

    listen(sock_fd, 10);

    return sock_fd;
}

/* Processa uma requisição do cliente. */
int process_request(int client_socket) {
    int option;
    char buffer[128];
    char info[32];

    read(client_socket , buffer, 128);
    struct timeval before, after;
    gettimeofday(&before, NULL);

    sscanf(buffer, "%d %s", &option, info);
    printf("Executando operação %d\n\n", option);

    long int delta = 0;
    tcp_handle_option(client_socket, option, info, &after, &delta);

    long processing_time = delta + (after.tv_sec - before.tv_sec) * 1000000 + after.tv_usec - before.tv_usec;
    printf("Processamento da requisição: %ldms\n\n", processing_time);

    return option;
}

void show_server_info() {
    char host[256]; 
    char *ip; 
    struct hostent *host_data; 
    int hostname; 
  
    hostname = gethostname(host, sizeof(host));
    host_data = gethostbyname(host); 
  
    ip = inet_ntoa(*((struct in_addr*) host_data->h_addr_list[0])); 
  
    printf("Host: %s\n", host); 
    printf("IP: %s\n", ip);
}

/* Cria um servidor que realiza operações com os perfis. */
int main() {

    // Preenche com os dados iniciais caso eles não existam
    seed_database();

    struct addrinfo *res = NULL;
    int server_socket = prepare_server_socket(res);
    int client_socket;
    pid_t pid;

    show_server_info();
    printf("Servidor no ar!\n");

    // Loop infinito para receber requisições
    for (;;) {

        struct sockaddr addr;
        socklen_t addrlen;

        client_socket = accept(server_socket, &addr, &addrlen);

        // Cada vez que recebe uma nova requisição, cria um processo para cuidar dela
        if ((pid = fork()) == 0) {
            close(server_socket);

            int option;
            do {
                option = process_request(client_socket);
            } while (option != EXIT_OPTION);

            close(client_socket);
            printf("Conexão fechada\n");
            exit(0);
        }

        close(client_socket);
    }

    return 0;
}