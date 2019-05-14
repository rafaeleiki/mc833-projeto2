#include "profile_manager.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "time_utils.h"
#include "../shared/protocol.h"

typedef int (*FindFunction)(Profile*, char[32]);

/* ========================================================
 * Database management
 * ========================================================
 */
/* Abre um arquivo para adicionar ao final. */
FILE* _append_db_file() {
    return fopen(DB_FILE_NAME, "ab+");
}

/* Abre um arquivo para leitura e escrita binária a partir do começo dele. */
FILE* _open_db_file() {
    return fopen(DB_FILE_NAME, "rb+");
}

/* Fecha o arquivo */
void _close_db_file(FILE* file) {
    fclose(file);
}

/* ========================================================
 * Funções privadas
 * ========================================================
 */
/* Retorna uma lista com todos os perfis que passam pelo critério dado
 * pela função recebida como parâmetro.
 */
Profile* _get_profiles_list(int *profilesCount, char info[32], FindFunction func) {
    FILE *file = _open_db_file();
    Profile *profiles = malloc(sizeof(Profile) * 1000);
    Profile profile;
    *profilesCount = 0;

    // Lê cada perfil do arquivo e filtra com a função
    while (fread(&profile, sizeof(Profile), 1, file) != 0) {
        if (func(&profile, info)) {
            profiles[*profilesCount] = profile;
            (*profilesCount)++;
        }
    }

    _close_db_file(file);

    return profiles;
}

/* ========================================================
 * Functions to filter the profiles list
 * ========================================================
 */
int _find_by_email(Profile *profile, char *email) {
    return strcmp(profile->email, email) == 0;
}

int _find_all(Profile *profile, char info[32]) {
    return 1;
}

int _find_by_address(Profile *profile, char address[32]) {
    return strcmp(profile->address, address) == 0;
}

int _find_by_education(Profile *profile, char education[32]) {
    return strcmp(profile->education, education) == 0;
}

/* ========================================================
 * Funções públicas
 * ========================================================
 */

/* Cria o arquivo de banco caso ele não exista ainda. */
void init_profile_db() {
    FILE *file = _append_db_file();
    _close_db_file(file);
}

/* Adiciona um perfil ao banco. */
int add_profile(Profile *profile) {
    int found;
    get_profile(profile->email, &found);

    // Caso não tenha encontrado o perfil com aquele email, adiciona-o ao banco
    if (!found) {
        FILE *file = _append_db_file();
        fwrite(profile, sizeof(Profile), 1, file);
        _close_db_file(file);
    }

    return !found;
}

/* Adiciona uma experiência a um perfil dado pelo email. Pode
 * retorna EXPERIENCE_PROFILE_NOT_FOUND caso o perfil não exista;
 * EXPERIENCE_ADDED caso tenha adiciona a experiência com sucesso;
 * ou EXPERIENCE_FULL caso já tenha atingido o limite de experiências permitido.
 */
int add_experience_to_profile(char *email, char *experience) {
    int found;
    Profile *profile = get_profile(email, &found);

    int added_experience = 0;

    // Caso tenha encontrado o perfil, tenta atualizá-lo
    if (found) {
        for (int i = 0; !added_experience && i < 10; i++) {
            if (profile->experiences[i][0] == '\0') {
                strcpy(profile->experiences[i], experience);
                added_experience = 1;
            }
        }
    }

    int result;

    if (!found) {
        result = EXPERIENCE_PROFILE_NOT_FOUND;
    } else if (added_experience) {
        result = EXPERIENCE_ADDED;
        update_profile(profile);        // Atualiza o arquivo
    } else {
        result = EXPERIENCE_FULL;
    }

    return result;
}

/* Atualiza um perfil no arquivo de banco */
int update_profile(Profile *profile) {
    Profile current;
    int updated = 0;
    
    FILE *file = _open_db_file();

    // Procura a posição do registro
    while (!updated && fread(&current, sizeof(Profile), 1, file) != 0) {

        // Ao encontrá-la, atualiza ela
        if (strcmp(current.email, profile->email) == 0) {
            fseek(file, -sizeof(Profile), SEEK_CUR);
            fwrite(profile, sizeof(Profile), 1, file);
            updated = 1;
        }
    }

    _close_db_file(file);
    return updated;
}

/* Retorna os bytes da imagem de um perfil e retorna a quantidade de parâmetros
 * no file_size. Usado para envio TCP.
 */
void send_profile_picture_tcp(int socket_fd, Profile *profile, long int *delta) {
    struct timeval before, after;
    gettimeofday(&before, NULL);

    char path[200];
    memset(path, 0, 200);
    sprintf(path, "%s/%s", PICTURES_DIR_NAME, profile->picture);
    FILE *file = fopen(path, "rb");

    if (!file) {
        printf("Arquivo de imagem não encontrado: %s\n", profile->picture);
    }

    // Descobre o tamanho do arquivo
    fseek(file, 0, SEEK_END);
    long int file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    int size = sizeof(profile->picture) + 1;
    write(socket_fd, &size, sizeof(size));
    write(socket_fd, profile->picture, size);
    write(socket_fd, &file_size, sizeof(file_size));

    unsigned char picture_bytes[file_size];
    
    fread(picture_bytes, 1, file_size, file);

    gettimeofday(&after, NULL);
    *delta = *delta + time_passed(&before, &after);

    write(socket_fd, picture_bytes, file_size);

    printf("Arquivo de imagem %s enviado\n\n\n", profile->picture);
    
    fclose(file);
}

/* Retorna os bytes da imagem de um perfil e retorna a quantidade de parâmetros
 * no file_size. Usado para envio UDP.
 */
void send_profile_picture_udp(SocketInfo *socket, Profile *profile) {
    char path[200];
    memset(path, 0, 200);
    sprintf(path, "%s/%s", PICTURES_DIR_NAME, profile->picture);
    FILE *file = fopen(path, "rb");

    if (!file) {
        printf("Arquivo de imagem não encontrado: %s\n", profile->picture);
    }

    // Descobre o tamanho do arquivo
    fseek(file, 0, SEEK_END);
    long int file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    int size = sizeof(profile->picture) + 1;
    sendto(socket->socket_fd, &size, sizeof(size), MSG_CONFIRM, socket->socket_addr, socket->addr_len);
    sendto(socket->socket_fd, profile->picture, size, MSG_CONFIRM, socket->socket_addr, socket->addr_len);
    sendto(socket->socket_fd, &file_size, sizeof(file_size), MSG_CONFIRM, socket->socket_addr, socket->addr_len);

    gettimeofday(&socket->time, NULL);
    unsigned char picture_bytes[BIG_MESSAGE];

    while (file_size > 0) {
        int max_bytes = file_size < BIG_MESSAGE ? file_size : BIG_MESSAGE;
        int read_chars = fread(picture_bytes, 1, max_bytes, file);
        sendto(socket->socket_fd, picture_bytes, read_chars, MSG_CONFIRM, socket->socket_addr, socket->addr_len);

        printf("%ld\n", file_size);
        file_size -= read_chars;
    }

    printf("Arquivo de imagem %s enviado\n\n\n", profile->picture);
    
    fclose(file);
}

/* ========================================================
 * Gets de conjuntos de perfis
 * ========================================================
 */
/* Retorna um único perfil */
Profile* get_profile(char* email, int *found) {
    return _get_profiles_list(found, email, _find_by_email);
}

/* Retorna todos os perfis */
Profile* get_all_profiles(int *profiles_count) {
    return _get_profiles_list(profiles_count, NULL, _find_all);
}

/* Retorna todos os perfis de um mesmo endereço */
Profile* get_profiles_by_address(char* address, int *profiles_count) {
    return _get_profiles_list(profiles_count, address, _find_by_address);
}

/* Retorna todos os perfis com a mesma formação acadêmica */
Profile* get_profiles_by_education(char* education, int *profiles_count) {
    return _get_profiles_list(profiles_count, education, _find_by_education);
}
