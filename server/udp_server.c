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

#include "udp_server_ui.h"
#include "data_seed.h"
#include "../shared/protocol.h"
#include "./socket_info.h"

/* Retorna um file descriptor do socket criado para o servidor. */
int udp_prepare_server_socket(struct addrinfo *res) {
    
    int sock_fd = socket(AF_INET, SOCK_DGRAM, 0);

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
int udp_process_request(int server_socket) {

    struct sockaddr addr;
    char buffer[BIG_MESSAGE];
    SocketInfo socket_info;

    int readed_chars = recvfrom(server_socket, buffer, BIG_MESSAGE,
                    MSG_WAITALL, &addr, &socket_info.addr_len);

    int option;
    char info[BIG_MESSAGE];

    struct timeval before;
    gettimeofday(&before, NULL);

    sscanf(buffer, "%d %s", &option, info);
    printf("Executando operação %d\n\n", option);

    long int delta = 0;
    socket_info.socket_addr = &addr;
    socket_info.socket_fd = server_socket;
    udp_handle_option(&socket_info, option, info);

    long processing_time = delta + (socket_info.time.tv_sec - before.tv_sec) * 1000000 
                           + socket_info.time.tv_usec - before.tv_usec;
    printf("Processamento da requisição: %ldms\n\n", processing_time);

    return option;
}

void udp_show_server_info() {
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
    int server_socket = udp_prepare_server_socket(res);
    pid_t pid;

    udp_show_server_info();
    printf("Servidor no ar!\n");

    // Loop infinito para receber requisições
    for (;;) {
        udp_process_request(server_socket);
    }

    return 0;
}
