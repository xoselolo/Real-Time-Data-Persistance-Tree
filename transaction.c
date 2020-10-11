
#include "transaction.h"
#include <time.h>
#include <stdlib.h>

char initialized = 0;

/**
 * Generates a random id between 0 and 32,767
 * Limit of our system, every server can only make 32,767 transactions
 */
int TRANSACTION_generateId(Node* root){
    if(initialized == 0){
        srand(time(NULL));
    }
    int transaction_id = rand();
    while (TRANSACTION_BINARY_TREE_exists(root, transaction_id)){
        transaction_id = rand();
    }
    return transaction_id;
}

int TRANSACTION_readPassive(Server *server, int fd_client, int id_server, int id_trans) {
    int fd;
    int value, version;
    Direction aux;

    if (server->next_server_direction.id_server != -1) {
        if (TOOLS_connect_server(&fd, server->next_server_direction.ip_address, server->next_server_direction.passive_port) != EXIT_SUCCESS)
            return EXIT_FAILURE;

        if (FRAME_sendReadRequest(fd, id_server, id_trans) != EXIT_SUCCESS)
            return EXIT_FAILURE;

        if (FRAME_readReadResponse(fd, &version, &value) != EXIT_SUCCESS)
            return EXIT_FAILURE;

        server->next_server_direction = TOOLS_findDirection(server->servers_directions, server->total_servers, id_server);
        server->data.version = version;
        server->data.value = value;

        if (FRAME_sendReadResponse(fd_client, version, value) != EXIT_SUCCESS)
            return EXIT_FAILURE;

    } else {
        aux = TOOLS_findDirection(server->servers_directions, server->total_servers, id_server);
        if (aux.id_server != -1) {
            server->next_server_direction = aux;
        } else {
            return ERR_SRVR_NOT_FOUND;
        }

        if (TOOLS_connect_server(&fd, server->next_server_direction.ip_address, server->next_server_direction.passive_port) != EXIT_SUCCESS)
            return EXIT_FAILURE;

        if (FRAME_sendReadResponse(fd, server->data.value, server->data.version) != EXIT_SUCCESS)
            return EXIT_FAILURE;

        if (FRAME_readReadAck(fd) != EXIT_SUCCESS)
            return ERR_ACK;

        if (FRAME_sendReadResponse(fd_client, server->data.value, server->data.version) != EXIT_SUCCESS)
            return EXIT_FAILURE;

    }

    return EXIT_SUCCESS;
}