
#ifndef TRANSACTIONS_BINARY_TREE_TYPES_H
#define TRANSACTIONS_BINARY_TREE_TYPES_H

#include "binary_tree.h"

typedef struct{
    char operator; // + - * /
    int operand; // 1, 2, 3, 4, 5 ...
}Operation;

typedef struct{
    int version; // Version of the data
    int value; // Value of the data
}Data;

typedef struct {
    char* ip_address;
    int port;
}Direction;

typedef struct{
    int id_server; // Config
    Direction my_direction; // Config
    int next_server;
    Data data; // Initially 0,0
    Node** transaction_trees; // Config (empty trees)
    Operation operation; // Config
    int total_servers; // Config + increment when added new servers
    Direction* servers_directions; // Config + new added servers {[0]->[ip,port] | [1]->[ip,port] ... [N]->[ip,port]}
}Server;

#endif //TRANSACTIONS_BINARY_TREE_TYPES_H
