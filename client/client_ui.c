#include "client_ui.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include "../shared/protocol.h"

#define MIN(x, y) (((x) > (y)) ? (x) : (y))

/* ========================================================
 * Funções privadas auxiliares
 * ========================================================
 */
/* Limpa o buffer de entrada do teclado. */
void _flush_input() {
    char c;
    while ((c = getchar()) != '\n' && c != EOF);
}

/* Desenha um separador. */
void _write_separator() {
    printf("\n================ x ================\n\n");
}

/* Desenha um separador secundário na tela. */
void _write_soft_separator() {
    printf("---\n");
}

/* Recebe um arquivo de imagem e o armazena. */
void _receive_picture_file(int socket_fd) {
    int message_size;
    read(socket_fd, &message_size, sizeof(message_size));

    char file_name[message_size];
    read(socket_fd, file_name, message_size);

    char file_path[200];
    sprintf(file_path, "pictures/%s", file_name);
    
    FILE *file = fopen(file_path, "wb");

    // Caso não consiga abrir o arquivo
    if (!file) {
        printf("Não foi possível abrir o arquivo '%s'\n", file_path);
        exit(1);
    }

    long int file_size;
    read(socket_fd, &file_size, sizeof(file_size));

    // Lê o arquivo enviado pelo servidor e escreve ele
    unsigned char image[file_size];
    while (file_size > 0) {
        int bytes_read = read(socket_fd, image, file_size);

        if (bytes_read > 0) {
            fwrite(image, 1, bytes_read, file);
            file_size -= bytes_read;
        }
    }

    fclose(file);
}

/* Exibe uma resposta recebida do servidor */
int _show_text_response(int socket_fd, struct timeval *after) {
    int size;
    read(socket_fd, &size, sizeof(size));

    if (size > 0) {
        char buffer[size];
        read(socket_fd, buffer, size);

        gettimeofday(after, NULL);

        _write_soft_separator();
        printf("%s", buffer);
    }
    
    return size;
}

/* Exibe os dados de um perfil recebidos pelo servidor */
int _show_profile(int socket_fd, struct timeval *after) {
    int end = _show_text_response(socket_fd, after);

    //if (showing) {
    //    printf("%s", buffer);
        _receive_picture_file(socket_fd);
    //    _write_soft_separator();
    //}

    return end > 0;
}

/* Exibe uma lista de perfis vindos do servidor. */
void _show_profile_list(int socket_fd, struct timeval *after) {
    int count;
    read(socket_fd, &count, sizeof(count));

    printf("Encontrados %d resultado(s)\n", count);
    _write_soft_separator();

    for (int i = 0; i < count; i++) {
        _show_profile(socket_fd, after);
    }
}

/* ========================================================
 * Funções privadas - Chamadsa para o servidor
 * ========================================================
 */
/* Exibe os dados de um perfil em específico. */
void _server_show_single_profile(int socket_fd, struct timeval *before, struct timeval *after) {
    char email[32];
    char buffer[128];
    printf("Digite o endereço de email do usuário: ");
    scanf("%s", email);

    sprintf(buffer, "%d %s", PROFILE_ALL_INFO_OPTION, email);

    gettimeofday(before, NULL);
    write(socket_fd, buffer, strlen(buffer) + 1);

    _show_profile(socket_fd, after);
}

/* Exibe as experiências de um perfil específico. */
void _server_show_experiences(int socket_fd, struct timeval *before, struct timeval *after) {
    char email[32];
    char buffer[128];
    printf("Digite o endereço de email do usuário: ");
    scanf("%s", email);

    sprintf(buffer, "%d %s", PROFILE_EXPERIENCE_OPTION, email);

    gettimeofday(before, NULL);
    write(socket_fd, buffer, strlen(buffer) + 1);

    _show_text_response(socket_fd, after);
}

/* Adiciona uma experiência a um perfil. */
void _server_add_experience(int socket_fd, struct timeval *before, struct timeval *after) {
    char buffer[128];
    char email[32];
    printf("Digite o endereço de email do usuário: ");
    scanf("%s", email);
    sprintf(buffer, "%d %s", NEW_EXPERIENCE_OPTION, email);
    write(socket_fd, buffer, strlen(buffer) + 1);

    char experience[64];
    printf("Digite a nova experiência do usuário: ");
    _flush_input();
    scanf("%[^\n]", experience);

    gettimeofday(before, NULL);
    write(socket_fd, experience, strlen(experience) + 1);

    _show_text_response(socket_fd, after);
}

/* Requisita ao servidor todos os perfis com a mesma formação acadêmica e os exibe. */
void _server_show_profiles_by_education(int socket_fd, struct timeval *before, struct timeval *after) {
    char education[32];
    char buffer[128];
    printf("Digite a formação acadêmica procurada: ");
    scanf("%s", education);

    sprintf(buffer, "%d %s", GRADUATED_USERS_OPTION, education);

    gettimeofday(before, NULL);
    write(socket_fd, buffer, strlen(buffer) + 1);

    _show_profile_list(socket_fd, after);
}

/* Mostra todas as habilidades dos perfis da mesma cidade. */
void _server_show_profiles_by_address(int socket_fd, struct timeval *before, struct timeval *after) {
    char address[32];
    char buffer[128];
    printf("Digite a cidade procurada: ");
    scanf("%s", address);

    sprintf(buffer, "%d %s", SKILLS_FROM_CITY_OPTION, address);

    gettimeofday(before, NULL);
    write(socket_fd, buffer, strlen(buffer) + 1);

    int count;
    read(socket_fd, &count, sizeof(count));

    for (int i = 0; i < count; i++) {
        _show_text_response(socket_fd, after);
    }
}

/* ========================================================
 * Funções públicas
 * ========================================================
 */

/* Desenha o menu principal */
void writeClientMenu() {
    printf("Olá! Bem-vindo ao sistema de recrutamento. O que você deseja fazer?\n");
    printf("(%d) Ver pessoas formadas em um curso\n", GRADUATED_USERS_OPTION);
    printf("(%d) Habilidades de perfis de uma cidade\n", SKILLS_FROM_CITY_OPTION);
    printf("(%d) Acrescentar experiência no perfil\n", NEW_EXPERIENCE_OPTION);
    printf("(%d) Experiências de um perfil específico\n", PROFILE_EXPERIENCE_OPTION);
    printf("(%d) Todas as informações de todos os perfis\n", ALL_PROFILE_OPTION);
    printf("(%d) Todas as informações de um perfil\n", PROFILE_ALL_INFO_OPTION);
    printf("(%d) Sair\n\n", EXIT_OPTION);
}

/* Realiza a leitura de uma opção. Caso ela seja inválida, uma nova é pedida até
 * se obter uma opção condizente com os serviços disponíveis.
 */
int readOption() {
    int option;
    int valid;

    // Enquanto não teve uma opção válida
    do {
        scanf("%d", &option);
        _flush_input();

        valid = option >= MIN_OPTION && option <= MAX_OPTION;

        if (!valid) {
            printf("Opção inválida\n");
        }
    } while (!valid);

    return option;
}

/* Maneja a requisição para o servidor e exibe sua resposta. */
void handle_request(int socket_fd, int option) {

    struct timeval before, after;

    switch (option) {
        case GRADUATED_USERS_OPTION:
            _server_show_profiles_by_education(socket_fd, &before, &after);
            break;
        case SKILLS_FROM_CITY_OPTION:
            _server_show_profiles_by_address(socket_fd, &before, &after);
            break;
        case NEW_EXPERIENCE_OPTION:
            _server_add_experience(socket_fd, &before, &after);
            break;
        case PROFILE_EXPERIENCE_OPTION:
            _server_show_experiences(socket_fd, &before, &after);
            break;
        case ALL_PROFILE_OPTION:
            gettimeofday(&before, NULL);
            write(socket_fd, "5", 2);
            _show_profile_list(socket_fd, &after);
            break;
        case PROFILE_ALL_INFO_OPTION:
            _server_show_single_profile(socket_fd, &before, &after);
            break;
        case EXIT_OPTION:
            write(socket_fd, "7", 2);
            break;
    }
    
    long request_time = (after.tv_sec - before.tv_sec) * 1000000 + after.tv_usec - before.tv_usec;
    printf("Duração da requisição: %ldms\n\n", request_time);

    _write_separator();
}
