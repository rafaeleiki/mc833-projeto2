#include "udp_client_ui.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/socket.h>

#include "../shared/protocol.h"
#include "../shared/socket_info.h"

#define MIN(x, y) (((x) > (y)) ? (x) : (y))

/* ========================================================
 * Funções privadas auxiliares
 * ========================================================
 */
/* Limpa o buffer de entrada do teclado. */
void _udp_flush_input() {
    char c;
    while ((c = getchar()) != '\n' && c != EOF);
}

/* Desenha um separador. */
void _udp_write_separator() {
    printf("\n================ x ================\n\n");
}

/* Desenha um separador secundário na tela. */
void _udp_write_soft_separator() {
    printf("---\n");
}

/* Recebe um arquivo de imagem e o armazena. */
void _udp_receive_picture_file(SocketInfo *socket_info) {
    int message_size;
    recvfrom(socket_info->socket_fd, &message_size, sizeof(message_size), MSG_WAITALL, socket_info->socket_addr, &socket_info->addr_len);

    char file_name[message_size + 1];
    recvfrom(socket_info->socket_fd, file_name, message_size, MSG_WAITALL, socket_info->socket_addr, &socket_info->addr_len);
    file_name[message_size] = '\0';

    char file_path[200];
    sprintf(file_path, "pictures/%s", file_name);
    
    FILE *file = fopen(file_path, "wb");

    // Caso não consiga abrir o arquivo
    if (!file) {
        printf("Não foi possível abrir o arquivo '%s'\n", file_path);
        exit(1);
    }

    long int file_size;
    recvfrom(socket_info->socket_fd, &file_size, sizeof(file_size), MSG_WAITALL, socket_info->socket_addr, &socket_info->addr_len);

    printf("Pre file \"%s\", size: %ld\n", file_name, file_size);

    // Lê o arquivo enviado pelo servidor e escreve ele
    unsigned char buffer[BIG_MESSAGE];
    while (file_size > 0) {
        int max_bytes = file_size > BIG_MESSAGE ? BIG_MESSAGE : file_size;
        printf("Esperando %d\n", max_bytes);
        int bytes_read = recvfrom(socket_info->socket_fd, buffer, max_bytes, MSG_WAITALL, socket_info->socket_addr, &socket_info->addr_len);
        printf("Faltam: %ld\n", file_size - bytes_read);

        if (bytes_read > 0) {
            fwrite(buffer, 1, bytes_read, file);
            file_size -= bytes_read;
        }
    }

    fclose(file);
}

/* Exibe uma resposta recebida do servidor */
int _udp_show_text_response(SocketInfo *socket_info, struct timeval *after) {
    int size;
    recvfrom(socket_info->socket_fd, &size, sizeof(size), MSG_WAITALL, socket_info->socket_addr, &socket_info->addr_len);

    if (size > 0) {
        char buffer[size + 1];
        recvfrom(socket_info->socket_fd, buffer, size, MSG_WAITALL, socket_info->socket_addr, &socket_info->addr_len);
        buffer[size] = '\0';

        gettimeofday(after, NULL);

        _udp_write_soft_separator();
        printf("%s", buffer);
    }
    
    return size;
}

/* Exibe os dados de um perfil recebidos pelo servidor */
int _udp_show_profile(SocketInfo *socket_info, struct timeval *after) {
    int end = _udp_show_text_response(socket_info, after);
    
    // Menos que isso significa que não foi encontrado perfil
    if (end > 50) {
        _udp_receive_picture_file(socket_info);
    }

    return end > 0;
}

/* Exibe uma lista de perfis vindos do servidor. */
void _udp_show_profile_list(SocketInfo *socket_info, struct timeval *after) {
    int count;
    recvfrom(socket_info->socket_fd, &count, sizeof(count), MSG_WAITALL, socket_info->socket_addr, &socket_info->addr_len);

    printf("Encontrados %d resultado(s)\n", count);
    _udp_write_soft_separator();

    for (int i = 0; i < count; i++) {
        _udp_show_profile(socket_info, after);
    }
}

/* ========================================================
 * Funções privadas - Chamadsa para o servidor
 * ========================================================
 */
/* Exibe os dados de um perfil em específico. */
void _udp_server_show_single_profile(SocketInfo *socket_info, struct timeval *before, struct timeval *after) {
    char email[32];
    char buffer[128];
    printf("Digite o endereço de email do usuário: ");
    scanf("%s", email);

    sprintf(buffer, "%d %s", PROFILE_ALL_INFO_OPTION, email);

    gettimeofday(before, NULL);
    sendto(socket_info->socket_fd, buffer, strlen(buffer), 0, socket_info->socket_addr, socket_info->addr_len);

    _udp_show_profile(socket_info, after);
}

/* Exibe as experiências de um perfil específico. */
void _udp_server_show_experiences(SocketInfo *socket_info, struct timeval *before, struct timeval *after) {
    char email[32];
    char buffer[128];
    printf("Digite o endereço de email do usuário: ");
    scanf("%s", email);

    sprintf(buffer, "%d %s", PROFILE_EXPERIENCE_OPTION, email);

    gettimeofday(before, NULL);
    sendto(socket_info->socket_fd, buffer, strlen(buffer), MSG_CONFIRM, socket_info->socket_addr, socket_info->addr_len);

    _udp_show_text_response(socket_info, after);
}

/* Adiciona uma experiência a um perfil. */
void _udp_server_add_experience(SocketInfo *socket_info, struct timeval *before, struct timeval *after) {
    char buffer[256];
    char email[32];
    printf("Digite o endereço de email do usuário: ");
    scanf("%s", email);
    sprintf(buffer, "%d %s", NEW_EXPERIENCE_OPTION, email);
    
    char experience[64];
    printf("Digite a nova experiência do usuário: ");
    _udp_flush_input();
    scanf("%[^\n]", experience);

    strcat(buffer, " ");
    strcat(buffer, experience);

    gettimeofday(before, NULL);
    sendto(socket_info->socket_fd, buffer, strlen(buffer), MSG_CONFIRM, socket_info->socket_addr, socket_info->addr_len);

    _udp_show_text_response(socket_info, after);
}

/* Requisita ao servidor todos os perfis com a mesma formação acadêmica e os exibe. */
void _udp_server_show_profiles_by_education(SocketInfo *socket_info, struct timeval *before, struct timeval *after) {
    char education[32];
    char buffer[128];
    printf("Digite a formação acadêmica procurada: ");
    scanf("%s", education);

    sprintf(buffer, "%d %s", GRADUATED_USERS_OPTION, education);

    gettimeofday(before, NULL);
    sendto(socket_info->socket_fd, buffer, strlen(buffer), MSG_CONFIRM, socket_info->socket_addr, socket_info->addr_len);

    _udp_show_profile_list(socket_info, after);
}

/* Mostra todas as habilidades dos perfis da mesma cidade. */
void _udp_server_show_profiles_by_address(SocketInfo *socket_info, struct timeval *before, struct timeval *after) {
    char address[32];
    char buffer[128];
    printf("Digite a cidade procurada: ");
    scanf("%s", address);

    sprintf(buffer, "%d %s", SKILLS_FROM_CITY_OPTION, address);

    gettimeofday(before, NULL);
    sendto(socket_info->socket_fd, buffer, strlen(buffer), MSG_CONFIRM, socket_info->socket_addr, socket_info->addr_len);

    int count;
    recvfrom(socket_info->socket_fd, &count, sizeof(count), MSG_WAITALL, socket_info->socket_addr, &socket_info->addr_len);

    for (int i = 0; i < count; i++) {
        _udp_show_text_response(socket_info, after);
    }
}

/* ========================================================
 * Funções públicas
 * ========================================================
 */

/* Desenha o menu principal */
void udp_write_client_menu() {
    printf("Olá! Bem-vindo ao sistema de recrutamento. O que você deseja fazer?\n");
    printf("(%d) Ver pessoas formadas em um curso\n", GRADUATED_USERS_OPTION);
    printf("(%d) Habilidades de perfis de uma cidade\n", SKILLS_FROM_CITY_OPTION);
    printf("(%d) Acrescentar experiência no perfil\n", NEW_EXPERIENCE_OPTION);
    printf("(%d) Experiências de um perfil específico\n", PROFILE_EXPERIENCE_OPTION);
    printf("(%d) Todas as informações de um perfil\n", PROFILE_ALL_INFO_OPTION);
    printf("(%d) Sair\n\n", EXIT_OPTION);
}

/* Realiza a leitura de uma opção. Caso ela seja inválida, uma nova é pedida até
 * se obter uma opção condizente com os serviços disponíveis.
 */
int udp_read_option() {
    int option;
    int valid;

    // Enquanto não teve uma opção válida
    do {
        scanf("%d", &option);
        _udp_flush_input();

        valid = option >= MIN_OPTION && option <= MAX_OPTION;

        if (!valid) {
            printf("Opção inválida\n");
        }
    } while (!valid);

    return option;
}

/* Maneja a requisição para o servidor e exibe sua resposta. */
void udp_handle_request(SocketInfo *socket_info, int option) {

    struct timeval before, after;

    switch (option) {
        case GRADUATED_USERS_OPTION:
            _udp_server_show_profiles_by_education(socket_info, &before, &after);
            break;
        case SKILLS_FROM_CITY_OPTION:
            _udp_server_show_profiles_by_address(socket_info, &before, &after);
            break;
        case NEW_EXPERIENCE_OPTION:
            _udp_server_add_experience(socket_info, &before, &after);
            break;
        case PROFILE_EXPERIENCE_OPTION:
            _udp_server_show_experiences(socket_info, &before, &after);
            break;
        case PROFILE_ALL_INFO_OPTION:
            _udp_server_show_single_profile(socket_info, &before, &after);
            break;
        case EXIT_OPTION:
            break;
    }
    
    long request_time = (after.tv_sec - before.tv_sec) * 1000000 + after.tv_usec - before.tv_usec;
    printf("Duração da requisição: %ldms\n\n", request_time);

    _udp_write_separator();
}
