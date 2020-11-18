
#ifndef _TRANSACTION_H
#define _TRANSACTION_H

#include "tools.h"
#include "types.h"
#include "frame.h"
#include "binary_tree.h"

#include "semaphore_v2.h"

int TRANSACTION_sendConnect(Server *server);
int TRANSACTION_connectPassive(int fd_client, Server *server);

int TRANSACTION_readPassive(Server *server, int fd_client, int id_server, int id_trans);

int TRANSACTION_generateId(Node* root);

int TRANSACTION_readActive(Server server);
int TRANSACTION_readResponsePassive(int fd_client, Server *server);
int TRANSACTION_replyReadLastUpdated(int client_fd, int id_server, Server *server);
int TRANSACTION_replyReadCommon(int client_fd, int id_server, int id_trans, Server *server);


int TRANSACTION_updateActive(Server server);
int TRANSACTION_readUpdatePassive(int fd_client, Server *server);
int TRANSACTION_replyUpdateLastUpdated(int client_fd, int id_server, Server *server, Operation operation);
int TRANSACTION_replyUpdateCommon(int client_fd, int id_server, int id_trans, Server *server, Operation operation);

#endif //_TRANSACTION_H
