#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h> 
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

#include "udp_client_ui.h"
#include "../shared/protocol.h"
#include "../shared/socket_info.h"

/* Cliente do serviço de perfil. */
int main(int argc, char *argv[]) {
    
    char address[100];
    if (argc > 1) {
        strcpy(address, argv[1]);
    } else {
        strcpy(address, "127.0.0.1");
    }

    printf("Servidor no IP %s\n", address);

    int option;

    // Cria a conexão com o servidor
    int socket_fd = socket(AF_INET, SOCK_DGRAM, 0);

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr(address);
    server_address.sin_port = htons(UDP_PORT);

    SocketInfo socket_info;
    socket_info.socket_addr = (struct sockaddr*) &server_address;
    socket_info.socket_fd = socket_fd;
    socket_info.addr_len = sizeof(server_address);

    // Enquanto não pedir para sair, segue fazendo o mesmo fluxo
    do {
        udp_write_client_menu();
        option = udp_read_option();
        udp_handle_request(&socket_info, option);
    } while (option != EXIT_OPTION);

    close(socket_fd);
    
    return 0;
}
