#ifndef _TOOLS_H_
#define _TOOLS_H_

//Llibreries del sistema
#include <arpa/inet.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define BACKLOG 64

void TOOLS_open_psocket(int* socket_fd, char* ip, int port);
char* TOOLS_read_until(int fd, char end);
int TOOLS_connect_server(int* socket_fd, char* ip, int port);

/**
 * TRAMA = [ R id_server # id_trans # ]
 * @param fd
 * @param id_server
 * @param id_trans
 * @return
 */
int TOOLS_sendReadPetitionTrama(int fd, int id_server, int id_trans);

/**
 * TRAMA = [ R id_server # id_trans # version # value # ]
 * @param fd
 * @param id_server
 * @param id_trans
 * @param version
 * @param value
 * @return
 */
int TOOLS_sendReadResponseTrama(int fd, int id_server, int id_trans, int version, int value);

#endif /* _TOOLS_H_ */