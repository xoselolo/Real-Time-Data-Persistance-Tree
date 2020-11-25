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
void TOOLS_printDirections(Server server);
void TOOLS_copyNextServerDirection(int id_server, Direction * next, Server server);
void TOOLS_copyDirection(Direction *direction, Direction values);
void TOOLS_operate(int* value, int* version, Operation operation);
void TOOLS_removeDirection(int id_server, Direction **directions, int *n_directions);
int TOOLS_replaceDirection(Direction * directions, int n_directions, Direction new_direction);
#endif /* _TOOLS_H_ */