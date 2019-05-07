#ifndef UDP_CLIENT_UI_H
#define UDP_CLIENT_UI_H

#include "../shared/socket_info.h"

void udp_write_client_menu();
int udp_read_option();
void udp_handle_request(SocketInfo *socket_fd, int option);

#endif