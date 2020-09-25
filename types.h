
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

typedef struct{
    int id_server; // Config
    char* ip_address; // Config
    int port; // Config
    int next_server;
    Data data; // Initially 0,0
    Node** binary_trees_root_nodes; // Config (empty trees)
    Operation operation; // Config
}Server;

#endif //TRANSACTIONS_BINARY_TREE_TYPES_H
