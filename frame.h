#ifndef FRAME_H
#define FRAME_H

#include "types.h"
#include "tools.h"

#define CONNECT 'C'
#define READ 'R'
#define READ_RESPONSE 'S'
#define ACK 'A'
#define UPDATE 'U'
#define READ_STR "R"
#define ACK_STR "A"
#define READ_RESPONSE_STR "S"

#define ERR_ACK 2
#define ERR_SRVR_NOT_FOUND 3
#define ERR_ACK_MSG "Error receiving acknowledge from the server"

int FRAME_sendFirstConnectionRequest(int fd, int id_server, char * ip_addr, int passive_port, int ping_port);
int FRAME_sendConnectionRequest(int fd, int id_server, char * ip_addr, int passive_port, int ping_port);
int FRAME_readConnectionRequest(int fd, int *id_server, char **ip_addr, int *passive_port, int *ping_port);
int FRAME_sendFirstConnectionResponse(int fd, Server server);
int FRAME_readFirstConnectionResponse(int fd, Server *server);
int FRAME_sendConnectionResponse(int fd, Server server);
int FRAME_readConnectionResponse(int fd, int *id_server, int *version, int *value);

int FRAME_readReadRequest(int fd, int * id_server, int * id_trans);
int FRAME_sendReadRequest(int fd, int id_server, int id_trans);
int FRAME_readReadResponse(int fd, int * version, int * value);
int FRAME_readOriginReadResponse(int fd, int * version, int * value);
int FRAME_sendReadResponse(int fd, int version, int value);
int FRAME_sendOriginReadResponse(int fd, int version, int value);
int FRAME_readAck(int fd);
int FRAME_sendAck(int fd);

int FRAME_sendUpdateRequest(int active_fd, int id_server, int id_transaction, Operation operation);
int FRAME_readUpdateRequest(int fd, int * id_server, int * id_trans, Operation* operation);
int FRAME_sendUpdateResponse(int fd, int version, int value);
int FRAME_readUpdateResponse(int fd, int * version, int * value);

#endif //FRAME_H