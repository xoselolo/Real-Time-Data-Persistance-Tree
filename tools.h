#ifndef _TOOLS_H_
#define _TOOLS_H_

//Llibreries del sistema

#include "types.h"

#define BACKLOG 64
#define MENU "Selecciona una opció:\n\t1. Llegir el valor\n\t2. Actualitzar el valor\n\0"
#define INVALID_OPTION "Opció no vàlida\n"

void TOOLS_open_psocket(int* socket_fd, char* ip, int port);
char* TOOLS_read_until(int fd, char end);
int TOOLS_connect_server(int* socket_fd, char* ip, int port);
Direction TOOLS_findDirection(Direction * directions, int n_directions, int id_server);
void TOOLS_printServerDirections(Server server);
#endif /* _TOOLS_H_ */