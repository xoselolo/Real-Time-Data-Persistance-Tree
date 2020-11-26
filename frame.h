#ifndef FRAME_H
#define FRAME_H

#include "types.h"
#include "tools.h"

#define CONNECT 'C'
#define READ 'R'
#define READ_RESPONSE 'S'
#define UPDATE_RESPONSE 'K'
#define ACK 'A'
#define UPDATE 'U'
#define PING 'P'

#define READ_STR "R"
#define READ_RESPONSE_STR "S"
#define UPDATE_RESPONSE_STR "K"
#define ACK_STR "A"
#define PING_STR "P"

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

int FRAME_readReadRequest(int fd, int * id_server);
int FRAME_sendReadRequest(int fd, int id_server);
int FRAME_readReadResponse(int fd, int * version, int * value);
int FRAME_readOriginReadResponse(int fd, int * version, int * value);
int FRAME_sendReadResponse(int fd, int version, int value);
int FRAME_sendOriginReadResponse(int fd, int version, int value);
int FRAME_readAck(int fd);
int FRAME_sendAck(int fd);

int FRAME_sendUpdateRequest(int active_fd, int id_server, Operation operation);
int FRAME_readUpdateRequest(int fd, int * id_server, Operation* operation);
int FRAME_sendUpdateResponse(int fd, int version, int value);
int FRAME_readUpdateResponse(int fd, int * version, int * value);

int FRAME_readOriginUpdateResponse(int fd, int * version, int * value);
int FRAME_sendOriginUpdateResponse(int fd, int version, int value);

int FRAME_sendPingRequest(int fd);
int FRAME_readPingRequest(int fd);
int FRAME_sendPingResponse(int fd, int version, int value, int isFirst);
int FRAME_readPingResponse(int fd, int *version, int *value, int *isFirst);

#endif //FRAME_H