#include "server_ui.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include "profile_manager.h"
#include "time_utils.h"
#include "../shared/profile.h"
#include "../shared/protocol.h"

/* Envia uma mensagem para o cliente, primeiro o tamanho da mensagem e depois o conteúdo. */
void _send_message(int socket_fd, char *content) {
    int size = strlen(content) + 1;
    write(socket_fd, &size, sizeof(int));
    write(socket_fd, content, size);

    printf("%s\n", content);
}

/* Retorna uma string com as experiências de um perfil. */
char* get_profile_experiences(Profile *profile) {
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
void show_profile(int socket_fd, Profile* profile, long int *delta) {
    char buffer[BIG_MESSAGE];
    memset(buffer, 0, BIG_MESSAGE);
    char* experiences = get_profile_experiences(profile);
    sprintf(buffer, "Email: %s\nNome: %s    | Sobrenome: %s\nFoto: %s\nResidência: %s\nFormação Acadêmcia: %s\nHabilidades: %s\nExperiências: %s\n", 
            profile->email, profile->name, profile->surename, profile->picture,
            profile->address, profile->education, profile->skills, experiences);
    printf("%s\n", buffer);

    _send_message(socket_fd, buffer);

    send_profile_picture(socket_fd, profile, delta);

    free(experiences);
}

/* Envia as experiências de um perfil. */
void show_experiences(int socket_fd, Profile* profile) {
    char *experiences = get_profile_experiences(profile);
    _send_message(socket_fd, experiences);
    free(experiences);
}

/* Envia uma mensagem de não encontrado. */
void send_not_found(int socket_fd) {
    _send_message(socket_fd, "Não foram encontrados resultados para a busca\n");
}

/* Envia uma lista de perfis. */
void show_profiles_list(int socket_fd, Profile* profiles, int profilesCount, long int *delta) {
    write(socket_fd, &profilesCount, sizeof(profilesCount));
    
    for (int i = 0; i < profilesCount; i++) {
        show_profile(socket_fd, &profiles[i], delta);
    }
    
    free(profiles);
}

/* Envia a lista dos perfis encontrados. */
void show_all_from_course(int socket_fd, char education[32], struct timeval *time, long int *delta) {
    int count;
    Profile* profiles = get_profiles_by_education(education, &count);
    gettimeofday(time, NULL);
    show_profiles_list(socket_fd, profiles, count, delta);
}

/* Envia as experiências dos perfis de dada cidade. */
void show_all_from_address(int socket_fd, char address[32], struct timeval *time, long int *delta) {
    int count;
    Profile* profiles = get_profiles_by_address(address, &count);
    gettimeofday(time, NULL);

    write(socket_fd, &count, sizeof(count));

    for (int i = 0; i < count; i++) {
        char buffer[BIG_MESSAGE];
        sprintf(buffer, "Email: %s\nHabilidades: %s\n", profiles[i].email, profiles[i].skills);
        _send_message(socket_fd, buffer);
    }

    free(profiles);
}

/* Envia todos os perfis para o cliente. */
void show_all_profiles(int socket_fd, struct timeval *time, long int *delta) {
    int count;
    Profile* profiles = get_all_profiles(&count);
    gettimeofday(time, NULL);
    show_profiles_list(socket_fd, profiles, count, delta);
}

/* Envia todas as experiências para o cliente */
void show_all_experiences(int socket_fd, char email[32], struct timeval *time) {
    int found;
    Profile *profile = get_profile(email, &found);
    gettimeofday(time, NULL);

    if (found) {
        show_experiences(socket_fd, profile);
    } else {
        send_not_found(socket_fd);
    }

    free(profile);
}

/* Envia os dados de um perfil. */
void show_single_profile(int socket_fd, char email[32], struct timeval *time, long int *delta) {
    int found;
    Profile *profile = get_profile(email, &found);
    gettimeofday(time, NULL);

    if (found) {
        show_profile(socket_fd, profile, delta);
    } else {
        send_not_found(socket_fd);
    }
    
    free(profile);
}

/* Adiciona uma experiência a um perfil e envia o estado da operação para o cliente. */
void add_experience(int socket_fd, char *email, struct timeval *time, long int *delta) {
    struct timeval before, after;
    gettimeofday(&before, NULL);

    char experience[128];
    read(socket_fd, experience, 128);
;
    gettimeofday(&after, NULL);
    *delta = *delta - time_passed(&before, &after);

    int result = add_experience_to_profile(email, experience);
    gettimeofday(time, NULL);

    switch (result) {
        case EXPERIENCE_PROFILE_NOT_FOUND:
            send_not_found(socket_fd);
            break;
        case EXPERIENCE_FULL:
            _send_message(socket_fd, "Não foi possível adicionar a habilidade - limite atingido.\n");
            break;
        case EXPERIENCE_ADDED:
            _send_message(socket_fd, "Experiência adicionada com sucesso!\n");
            break;
    }
}

/* Realiza a operação recebida pelo código numérico. */
void handle_option(int socket_fd, int option, char info[32], struct timeval *time, long int *delta) {

    switch (option) {
        case GRADUATED_USERS_OPTION:
            show_all_from_course(socket_fd, info, time, delta);
            break;
        case SKILLS_FROM_CITY_OPTION:
            show_all_from_address(socket_fd, info, time, delta);
            break;
        case NEW_EXPERIENCE_OPTION:
            add_experience(socket_fd, info, time, delta);
            break;
        case PROFILE_EXPERIENCE_OPTION:
            show_all_experiences(socket_fd, info, time);
            break;
        case ALL_PROFILE_OPTION:
            show_all_profiles(socket_fd, time, delta);
            break;
        case PROFILE_ALL_INFO_OPTION:
            show_single_profile(socket_fd, info, time, delta);
            break;
        case EXIT_OPTION: 
            break;
    }
}
