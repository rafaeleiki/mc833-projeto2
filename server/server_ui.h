#ifndef SERVER_UI_H
#define SERVER_UI_H

#include "../shared/profile.h"
#include <sys/time.h>

void showProfile(int socket_fd, Profile* profile);
void show_experiences(int socket_fd, Profile* profile);
void send_not_found(int socket_fd);
void handle_option(int socket_fd, int option, char info[32], struct timeval *time, long int *delta);

#endif