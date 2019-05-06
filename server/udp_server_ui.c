#include "udp_server_ui.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "profile_manager.h"
#include "time_utils.h"
#include "../shared/profile.h"
#include "../shared/protocol.h"

/* Envia uma mensagem para o cliente, primeiro o tamanho da mensagem e depois o conteúdo. */
void _udp_send_message(SocketInfo *socket_info, char *content) {
    int size = strlen(content) + 1;
    sendto(socket_info->socket_fd, content, size, MSG_CONFIRM, socket_info->socket_addr, socket_info->addr_len);
    printf("%s\n", content);
}

/* Retorna uma string com as experiências de um perfil. */
char* _udp_get_profile_experiences(Profile *profile) {
    char* experiences = malloc(sizeof(char) * 640);
    int end = 0;
    char buffer[80];

    for (int i = 0; !end && i < 10; i++) {
        if (profile->experiences[i][0] == '\0') {
            end = 1;
        } else {
            sprintf(buffer, "(%d) %s\n", i + 1, profile->experiences[i]);
            strcat(experiences, buffer);
        }
    }

    return experiences;
}

/* Envia as informações de um perfil. */
void _udp_show_profile(SocketInfo *socket_info, Profile* profile) {
    char buffer[BIG_MESSAGE];
    memset(buffer, 0, BIG_MESSAGE);
    char* experiences = get_profile_experiences(profile);
    sprintf(buffer, "Email: %s\nNome: %s    | Sobrenome: %s\nFoto: %s\nResidência: %s\nFormação Acadêmcia: %s\nHabilidades: %s\nExperiências: %s\n", 
            profile->email, profile->name, profile->surename, profile->picture,
            profile->address, profile->education, profile->skills, experiences);
    printf("%s\n", buffer);

    _send_message(socket_info, buffer);

    send_profile_picture_udp(socket_info, profile);

    free(experiences);
}

/* Envia as experiências de um perfil. */
void _udp_show_experiences(SocketInfo *socket_info, Profile* profile) {
    char *experiences = get_profile_experiences(profile);
    _udp_send_message(socket_info, experiences);
    free(experiences);
}

/* Envia uma mensagem de não encontrado. */
void _udp_send_not_found(SocketInfo *socket_info) {
    _udp_send_message(socket_info, "Não foram encontrados resultados para a busca\n");
}

/* Envia uma lista de perfis. */
void _udp_show_profiles_list(SocketInfo *socket_info, Profile* profiles, int profilesCount) {
    write(socket_info, &profilesCount, sizeof(profilesCount));
    
    for (int i = 0; i < profilesCount; i++) {
        _udp_show_profile(socket_info, &profiles[i]);
    }
    
    free(profiles);
}

/* Envia a lista dos perfis encontrados. */
void _udp_show_all_from_course(SocketInfo *socket_info, char education[32]) {
    int count;
    Profile* profiles = get_profiles_by_education(education, &count);
    gettimeofday(&socket_info->time, NULL);
    _udp_show_profiles_list(socket_info, profiles, count);
}

/* Envia as experiências dos perfis de dada cidade. */
void _udp_show_all_from_address(SocketInfo *socket_info, char address[32]) {
    int count;
    Profile* profiles = get_profiles_by_address(address, &count);
    gettimeofday(&socket_info->time, NULL);

    sendto(socket_info->socket_fd, &count, sizeof(count), MSG_CONFIRM, socket_info->socket_addr, socket_info->addr_len);

    for (int i = 0; i < count; i++) {
        char buffer[BIG_MESSAGE];
        sprintf(buffer, "Email: %s\nHabilidades: %s\n", profiles[i].email, profiles[i].skills);
        _udp_send_message(socket_info, buffer);
    }

    free(profiles);
}

/* Envia todos os perfis para o cliente. */
void _udp_show_all_profiles(SocketInfo *socket_info) {
    int count;
    Profile* profiles = get_all_profiles(&count);
    gettimeofday(&socket_info->time, NULL);
    _udp_show_profiles_list(socket_info, profiles, count);
}

/* Envia todas as experiências para o cliente */
void _udp_show_all_experiences(SocketInfo *socket_info, char email[32]) {
    int found;
    Profile *profile = get_profile(email, &found);
    gettimeofday(&socket_info->time, NULL);

    if (found) {
        _udp_show_experiences(socket_info, profile);
    } else {
        _udp_send_not_found(socket_info);
    }

    free(profile);
}

/* Envia os dados de um perfil. */
void _udp_show_single_profile(SocketInfo *socket_info, char email[32]) {
    int found;
    Profile *profile = get_profile(email, &found);
    gettimeofday(&socket_info->time, NULL);

    if (found) {
        _udp_show_profile(socket_info, profile);
    } else {
        _udp_send_not_found(socket_info);
    }
    
    free(profile);
}

/* Adiciona uma experiência a um perfil e envia o estado da operação para o cliente. */
void _udp_add_experience(SocketInfo *socket_info, char *email, char *extra_info) {
    char experience[128];
    sscanf(extra_info, "%[^\n]s", experience);

    int result = add_experience_to_profile(email, experience);
    gettimeofday(&socket_info->time, NULL);

    switch (result) {
        case EXPERIENCE_PROFILE_NOT_FOUND:
            _udp_send_not_found(socket_info);
            break;
        case EXPERIENCE_FULL:
            _udp_send_message(socket_info, "Não foi possível adicionar a habilidade - limite atingido.\n");
            break;
        case EXPERIENCE_ADDED:
            _udp_send_message(socket_info, "Experiência adicionada com sucesso!\n");
            break;
    }
}

/* Realiza a operação recebida pelo código numérico. */
void udp_handle_option(SocketInfo *socket_info, int option, char info[BIG_MESSAGE]) {
    char parameter[32];
    char extra_info[BIG_MESSAGE];

    if (option == ALL_PROFILE_OPTION) {
        _udp_show_all_profiles(socket_info);
    } else {
        sscanf(info, "%s %[^\n]s", parameter, extra_info);

        switch (option) {
            case GRADUATED_USERS_OPTION:
                _udp_show_all_from_course(socket_info, parameter);
                break;
            case SKILLS_FROM_CITY_OPTION:
                _udp_show_all_from_address(socket_info, parameter);
                break;
            case NEW_EXPERIENCE_OPTION:
                _udp_add_experience(socket_info, parameter, extra_info);
                break;
            case PROFILE_EXPERIENCE_OPTION:
                _udp_show_all_experiences(socket_info, parameter);
                break;
            case PROFILE_ALL_INFO_OPTION:
                _udp_show_single_profile(socket_info, parameter);
                break;
        }
    }
}
