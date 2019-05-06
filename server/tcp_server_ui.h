#ifndef TCP_SERVER_UI_H
#define TCP_SERVER_UI_H

#include "../shared/profile.h"
#include <sys/time.h>

void tcp_handle_option(int socket_fd, int option, char info[32], struct timeval *time, long int *delta);

#endif