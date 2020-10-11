#ifndef _TOOLS_H_
#define _TOOLS_H_

//Llibreries del sistema

#include "types.h"

#define BACKLOG 64

void TOOLS_open_psocket(int* socket_fd, char* ip, int port);
char* TOOLS_read_until(int fd, char end);
int TOOLS_connect_server(int* socket_fd, char* ip, int port);
Direction TOOLS_findDirection(Direction * directions, int n_directions, int id_server);

#endif /* _TOOLS_H_ */