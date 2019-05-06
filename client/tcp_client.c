#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

#include "client_ui.h"
#include "../shared/protocol.h"

/* Cliente do serviço de perfil. */
int main(int argc, char *argv[]) {
    
    char address[100];
    if (argc > 1) {
        strcpy(address, argv[1]);
    } else {
        strcpy(address, "127.0.0.1");
    }

    printf("Conectando com o IP %s\n", address);

    int option;

    // Cria a conexão com o servidor
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr(address);
    server_address.sin_port = htons(PORT);

    if (connect(socket_fd, (struct sockaddr*) &server_address, sizeof(server_address)) < 0) {
        perror("Erro de conexão");
        exit(1);
    }

    // Enquanto não pedir para sair, segue fazendo o mesmo fluxo
    do {
        writeClientMenu();
        option = readOption();
        handle_request(socket_fd, option);
    } while (option != EXIT_OPTION);
    
    return 0;
}
