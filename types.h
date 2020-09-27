
#ifndef TRANSACTIONS_BINARY_TREE_TYPES_H
#define TRANSACTIONS_BINARY_TREE_TYPES_H

#include "binary_tree.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>

typedef struct{
    char operator; // + - * /
    int operand; // 1, 2, 3, 4, 5 ...
}Operation;

typedef struct{
    int version; // Version of the data
    int value; // Value of the data
}Data;

typedef struct {
    char* ip_address; // Config
    int active_port; // Config
    int passive_port; // Config
    int ping_port; // Config
}Direction;

typedef struct{
    int id_server; // Config
    Direction my_direction; // Config
    int id_server_next; // to make a petition or resend it

    // --------- SOCKET 1 - Client (active) ---------
    int fd_active;
    struct sockaddr_in socket_1_Config;

    // --------- SOCKET 2 - Server (passive) ---------
    int fd_passive;
    struct sockaddr_in socket_2_Config;

    // --------- SOCKET 3 - Server (ping) ---------
    int fd_ping;
    struct sockaddr_in socket_3_Config;

    int next_server;
    Data data; // Initially 0,0
    Node** transaction_trees; // Config (empty trees)
    Operation operation; // Config
    int total_servers; // Config + increment when added new servers
    Direction* servers_directions; // Config + new added servers {[0]->[ip,port] | [1]->[ip,port] ... [N]->[ip,port]}
}Server;



#endif //TRANSACTIONS_BINARY_TREE_TYPES_H
