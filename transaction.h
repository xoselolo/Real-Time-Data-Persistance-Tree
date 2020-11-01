
#ifndef _TRANSACTION_H
#define _TRANSACTION_H

#include "tools.h"
#include "types.h"
#include "frame.h"
#include "binary_tree.h"

int TRANSACTION_sendConnect(Server *server);
int TRANSACTION_connectPassive(int fd_client, Server *server);

int TRANSACTION_readPassive(Server *server, int fd_client, int id_server, int id_trans);

int TRANSACTION_generateId(Node* root);


#endif //_TRANSACTION_H
