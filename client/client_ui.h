#ifndef CLIENT_UI_H
#define CLIENT_UI_H

void writeClientMenu();
int readOption();
void handle_request(int socket_fd, int option);

#endif