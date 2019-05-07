#ifndef UDP_SERVER_UI_H
#define UDP_SERVER_UI_H

#include "../shared/profile.h"
#include "../shared/protocol.h"
#include "../shared/socket_info.h"

void udp_handle_option(SocketInfo *socket_info, int option, char info[BIG_MESSAGE]);

#endif