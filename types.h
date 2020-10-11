
#ifndef TRANSACTIONS_BINARY_TREE_TYPES_H
#define TRANSACTIONS_BINARY_TREE_TYPES_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <time.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <netdb.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define BOLD    "\033[1m"
#define BOLDGREEN "\033[1m\033[32m"
#define BOLDRED   "\033[1m\033[31m"
#define RESET     "\033[0m"

#define ERR_ARGS    BOLDRED "Error, missing arguments." RESET " Usage: ./ex0 ./config/server_config.txt\n"
#define ERR_CONN    BOLDRED "Connection to client failed" RESET

typedef struct{
    char operator; // + - * /
    int operand; // 1, 2, 3, 4, 5 ...
}Operation;

typedef struct{
    int version; // Version of the data
    int value; // Value of the data
} Data;

typedef struct{
    char type;
    char *data;
} Frame;

typedef struct {
    int id_server; // Config
    char* ip_address; // Config
    int active_port; // Config
    int passive_port; // Config
    int ping_port; // Config
} Direction;

typedef struct Node{
    int id_server;
    int id_transaction;
    struct Node* smaller;
    struct Node* bigger;
}Node;

typedef struct{
    Direction my_direction; // Config

    Direction next_server_direction; // to make a petition or resend it

    char is_read_only;
    int sleep_time;

    Data data; // Initially 0,0
    Node** transaction_trees; // Config (empty trees)
    Operation operation; // Config
    int total_servers; // Config + increment when added new servers
    Direction* servers_directions; // Config + new added servers {[0]->[ip,port] | [1]->[ip,port] ... [N]->[ip,port]}

    // --------- SOCKET 1 - Client (active) ---------
    int fd_active;

    // --------- SOCKET 2 - Server (passive) ---------
    int fd_passive;

    // --------- SOCKET 3 - Server (ping) ---------
    int fd_ping;

}Server;



#endif //TRANSACTIONS_BINARY_TREE_TYPES_H
