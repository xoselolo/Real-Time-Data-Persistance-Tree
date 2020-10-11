
#ifndef TRANSACTIONS_BINARY_TREE_TRANSACTION_H
#define TRANSACTIONS_BINARY_TREE_TRANSACTION_H

#include "tools.h"
#include "types.h"
#include "frame.h"

int TRANSACTION_generateId();
int TRANSACTION_readPassive(Server *server, int fd_client, int id_server, int id_trans);

int TRANSACTION_generateId(Node* root);


#endif //TRANSACTIONS_BINARY_TREE_TRANSACTION_H
