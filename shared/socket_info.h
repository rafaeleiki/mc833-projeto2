#ifndef SOCKET_INFO_H
#define SOCKET_INFO_H

#include <sys/time.h>
#include <sys/socket.h>

#define MSG_CONFIRM 0
//#define MSG_WAITALL 0

typedef struct SocketInfo {
    int socket_fd;
    struct sockaddr* socket_addr;
    struct sockaddr receiver;
    unsigned int addr_len;
    struct timeval time;
} SocketInfo;

#endif
