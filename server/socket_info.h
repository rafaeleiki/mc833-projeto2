#ifndef SOCKET_INFO_H
#define SOCKET_INFO_H

#include <sys/time.h>

#define MSG_CONFIRM 0

typedef struct SocketInfo {
    int socket_fd;
    const struct sockaddr* socket_addr;
    int addr_len;
    struct timeval time;
} SocketInfo;

#endif
