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

#define BACKLOG 64

void TOOLS_open_psocket(int* socket_fd, char* ip, in_port_t port);
char* TOOLS_read_until(int fd, char end);

#endif /* _TOOLS_H_ */