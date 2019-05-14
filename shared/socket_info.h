#ifndef SOCKET_INFO_H
#define SOCKET_INFO_H

#include <sys/time.h>
#include <sys/socket.h>

/*
#ifndef MSG_CONFIRM
#define MSG_CONFIRM 0
#endif

#ifndef MSG_WAITALL
#define MSG_WAITALL 0
#endif
*/

typedef struct SocketInfo {
    int socket_fd;
    struct sockaddr* socket_addr;
    struct sockaddr receiver;
    unsigned int addr_len;
    struct timeval time;
} SocketInfo;

#endif
