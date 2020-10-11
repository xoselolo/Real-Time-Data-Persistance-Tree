#ifndef FRAME_H
#define FRAME_H

#include "types.h"
#include "tools.h"

#define READ 'R'
#define READ_STR "R"
#define ACK "ACK\0"

#define ERR_ACK 2
#define ERR_SRVR_NOT_FOUND 3
#define ERR_ACK_MSG "Error receiving acknowledge from the server"

int FRAME_readReadRequest(int fd, int * id_server, int * id_trans);
int FRAME_sendReadRequest(int fd, int id_server, int id_trans);
int FRAME_readReadResponse(int fd, int * version, int * value);
int FRAME_sendReadResponse(int fd, int version, int value);
int FRAME_readReadAck(int fd);
int FRAME_sendReadAck(int fd);

#endif //FRAME_H