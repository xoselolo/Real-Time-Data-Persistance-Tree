
#include "transaction.h"

extern semaphore sem_read_response;
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

int TRANSACTION_sendConnect(Server *server) {
    int server_fd, i, size;
    char * buffer;
    int id_server, version, value;

    if (!server->is_first) {
        size = asprintf(&buffer, BOLDGREEN "\nInicialitzant connexions\n" RESET);
        write(1, buffer, size);
        free(buffer);

        if (TOOLS_connect_server(&server_fd, 
                            server->servers_directions[0].ip_address, 
                            server->servers_directions[0].passive_port) == EXIT_FAILURE) return EXIT_FAILURE;

        if (FRAME_sendFirstConnectionRequest(server_fd,
                                        server->my_direction.id_server,
                                        server->my_direction.ip_address, 
                                        server->my_direction.passive_port, 
                                        server->my_direction.ping_port) == EXIT_FAILURE) return EXIT_FAILURE;

        if (FRAME_readFirstConnectionResponse(server_fd, server) == EXIT_FAILURE) return EXIT_FAILURE;

        for (i=1; i < server->total_servers; i++) {
            size = asprintf(&buffer, BOLD "\nInicialitzant connexió amb el servidor %d\n" RESET, server->servers_directions[i].id_server);
            write(1, buffer, size);
            free(buffer);

            if (TOOLS_connect_server(&server_fd, 
                                server->servers_directions[i].ip_address, 
                                server->servers_directions[i].passive_port) == EXIT_FAILURE) return EXIT_FAILURE;
            
            if (FRAME_sendConnectionRequest(server_fd, 
                                        server->my_direction.id_server,
                                        server->my_direction.ip_address, 
                                        server->my_direction.passive_port, 
                                        server->my_direction.ping_port) == EXIT_FAILURE) return EXIT_FAILURE;
            
            if (FRAME_readConnectionResponse(server_fd, &id_server, &version, &value) == EXIT_FAILURE) return EXIT_FAILURE;

            if (version > server->data.version) {
                server->next_server_direction = TOOLS_findDirection(server->servers_directions, server->total_servers, id_server);
                server->data.value = value;
                server->data.version = version;
            }
        }

        size = asprintf(&buffer, BOLD "Connexions inicialitzades\n" RESET);
        write(1, buffer, size);
        free(buffer);
        TOOLS_printDirections(server->servers_directions, server->total_servers);
    }

    return EXIT_SUCCESS;
}

int TRANSACTION_connectPassive(int fd_client, Server *server) {
    char first_notFirst;
    char *ip_addr;
    int passive_port, ping_port, id_server;

    if (read(fd_client, &first_notFirst, sizeof(char)) != sizeof(char)) return EXIT_FAILURE;
    if ((server->is_first && first_notFirst != 'F') || (!server->is_first && first_notFirst != 'N')) return EXIT_FAILURE;

    if (FRAME_readConnectionRequest(fd_client, &id_server, &ip_addr, &passive_port, &ping_port) == EXIT_FAILURE) return EXIT_FAILURE;

    server->servers_directions[server->total_servers].id_server = id_server;
    server->servers_directions[server->total_servers].ip_address = ip_addr;
    server->servers_directions[server->total_servers].passive_port = passive_port;
    server->servers_directions[server->total_servers].ping_port = ping_port;
    (server->total_servers)++;

    server->servers_directions = (Direction *)realloc(server->servers_directions, sizeof(Direction)*(server->total_servers + 1));

    server->transaction_trees = (Node **)realloc(server->transaction_trees, sizeof(Node*) * server->total_servers + 1);
    server->transaction_trees[server->total_servers] = (Node*) malloc(sizeof(Node));
    server->transaction_trees[server->total_servers]->id_server = id_server;
    server->transaction_trees[server->total_servers]->id_transaction = -1;
    server->transaction_trees[server->total_servers]->smaller = NULL;
    server->transaction_trees[server->total_servers]->bigger = NULL;

    TOOLS_printDirections(server->servers_directions, server->total_servers);

    if (server->is_first) {
        if (FRAME_sendFirstConnectionResponse(fd_client, *server) == EXIT_FAILURE) return EXIT_FAILURE;
    } else {
        if (FRAME_sendConnectionResponse(fd_client, *server) == EXIT_FAILURE) return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int TRANSACTION_readActive(Server server) {
    int next_fd, version, value, size;
    char *buffer;

    if (TOOLS_connect_server(&next_fd, server.next_server_direction.ip_address, server.next_server_direction.passive_port) == EXIT_FAILURE) {
        close(next_fd);
        return EXIT_NEXT_DOWN;
    }

    int id_transaction = TRANSACTION_generateId(server.transaction_trees[0]);
    TRANSACTION_BINARY_TREE_add(&(server.transaction_trees[0]), id_transaction, server.my_direction.id_server);

    if(FRAME_sendReadRequest(next_fd, server.my_direction.id_server, id_transaction) == EXIT_FAILURE) {
        close(next_fd);
        return EXIT_NEXT_DOWN;
    }

    size = asprintf(&buffer, BOLDYELLOW "Read request sent to server %d, %s:%d\n" RESET, server.next_server_direction.id_server, server.next_server_direction.ip_address, server.next_server_direction.passive_port);
    write(1, buffer, size);
    free(buffer);

    // wait for response
    SEM_init(&sem_read_response, 0);
    SEM_wait(&sem_read_response);
    
    if(FRAME_readReadResponse(next_fd, &version, &value) == EXIT_FAILURE) {
        close(next_fd);
        return EXIT_NEXT_DOWN;
    }
    return EXIT_SUCCESS;
}

int TRANSACTION_readResponsePassive(int fd_client, Server *server) {
    int size;
    char *buffer;

    if(FRAME_readOriginReadResponse(fd_client, &(server->data.version), &(server->data.value)) == EXIT_FAILURE) return EXIT_FAILURE;
    FRAME_sendAck(fd_client);
    server->next_server_direction.id_server = -1;

    size = asprintf(&buffer, BOLDGREEN "Response received %d v_%d\n\n" RESET, server->data.value, server->data.version);
    write(1, buffer, size);
    free(buffer);
    
    SEM_signal(&sem_read_response);
    return EXIT_SUCCESS;
}

int TRANSACTION_replyReadLastUpdated(int client_fd, int id_server, Server *server) {
    int origin_fd, size;
    char *buffer;

    Direction origin = TOOLS_findDirection(server->servers_directions, server->total_servers, id_server);

    if (TOOLS_connect_server(&origin_fd, 
                            origin.ip_address, 
                            origin.passive_port) == EXIT_FAILURE) return EXIT_FAILURE;
    if (FRAME_sendOriginReadResponse(origin_fd, server->data.version, server->data.value) == EXIT_FAILURE) return EXIT_FAILURE;
    if (FRAME_readAck(origin_fd) == EXIT_FAILURE) return EXIT_FAILURE;

    size = asprintf(&buffer, BOLDGREEN "Successfully sent the last updated value: %d v_%d\n\n" RESET, server->data.value, server->data.version);
    write(1, buffer, size);
    free(buffer);

    TOOLS_copyNextServerDirection(id_server, &(server->next_server_direction), *server);

    if (FRAME_sendReadResponse(client_fd, server->data.version, server->data.value) == EXIT_FAILURE) return EXIT_FAILURE;
    return EXIT_SUCCESS;
}

int TRANSACTION_replyReadCommon(int client_fd, int id_server, int id_trans, Server *server) {
    int next_fd, size;
    char *buffer;
    if (TOOLS_connect_server(&next_fd,
                             server->next_server_direction.ip_address, 
                             server->next_server_direction.passive_port) == EXIT_FAILURE) return EXIT_NEXT_DOWN; 

    if (FRAME_sendReadRequest(next_fd, id_server, id_trans) == EXIT_FAILURE) return EXIT_NEXT_DOWN;
    if (FRAME_readReadResponse(next_fd, &(server->data.version), &(server->data.value)) == EXIT_FAILURE) return EXIT_NEXT_DOWN;

    size = asprintf(&buffer, BOLDGREEN "Updated value recieved: %d v_%d\n\n" RESET, server->data.value, server->data.version);
    write(1, buffer, size);
    free(buffer);

    TOOLS_copyNextServerDirection(id_server, &(server->next_server_direction), *server);
    
    // Enviamos la respuesta al que nos ha preguntado a nosotros
    if (FRAME_sendReadResponse(client_fd, server->data.version, server->data.value) == EXIT_FAILURE) return EXIT_FAILURE;
    return EXIT_SUCCESS;
}



int TRANSACTION_updateActive(Server server) {
    int next_fd, version, value, size;
    char *buffer;

    if (TOOLS_connect_server(&next_fd, server.next_server_direction.ip_address, server.next_server_direction.passive_port) == EXIT_FAILURE) {
        close(next_fd);
        return EXIT_NEXT_DOWN;
    }

    int id_transaction = TRANSACTION_generateId(server.transaction_trees[0]);
    if(FRAME_sendUpdateRequest(next_fd, server.my_direction.id_server, id_transaction, server.operation) == EXIT_FAILURE) {
        close(next_fd);
        return EXIT_NEXT_DOWN;
    }

    size = asprintf(&buffer, BOLDYELLOW "Update request sent to server %d, %s:%d\n" RESET, server.next_server_direction.id_server, server.next_server_direction.ip_address, server.next_server_direction.passive_port);
    write(1, buffer, size);
    free(buffer);

    TRANSACTION_BINARY_TREE_add(&(server.transaction_trees[0]), id_transaction, server.my_direction.id_server);

    // wait for response
    SEM_init(&sem_read_response, 0);
    SEM_wait(&sem_read_response);

    if(FRAME_readUpdateResponse(next_fd, &version, &value) == EXIT_FAILURE) {
        close(next_fd);
        return EXIT_NEXT_DOWN;
    }
    return EXIT_SUCCESS;
}

int TRANSACTION_updateResponsePassive(int fd_client, Server *server) {
    int size;
    char *buffer;

    if(FRAME_readOriginUpdateResponse(fd_client, &(server->data.version), &(server->data.value)) == EXIT_FAILURE) return EXIT_FAILURE;
    FRAME_sendAck(fd_client);
    server->next_server_direction.id_server = -1;
    size = asprintf(&buffer, BOLDGREEN "Response received %d v_%d\n\n" RESET, server->data.value, server->data.version);
    write(1, buffer, size);
    free(buffer);
    SEM_signal(&sem_read_response);
    return EXIT_SUCCESS;
}

int TRANSACTION_replyUpdateLastUpdated(int client_fd, int id_server, Server *server, Operation operation) {
    TOOLS_operate(&server->data.value, &server->data.version, operation);

    Direction origin = TOOLS_findDirection(server->servers_directions, server->total_servers, id_server);
    int origin_fd, size;
    char *buffer;

    if (TOOLS_connect_server(&origin_fd,
                             origin.ip_address,
                             origin.passive_port) == EXIT_FAILURE) return EXIT_FAILURE;
    if (FRAME_sendOriginUpdateResponse(origin_fd, server->data.version, server->data.value) == EXIT_FAILURE) return EXIT_FAILURE;
    if (FRAME_readAck(origin_fd) == EXIT_FAILURE) return EXIT_FAILURE;

    size = asprintf(&buffer, BOLDGREEN "Successfully sent the last updated value: %d v_%d\n\n" RESET, server->data.value, server->data.version);
    write(1, buffer, size);
    free(buffer);

    TOOLS_copyNextServerDirection(id_server, &(server->next_server_direction), *server);

    if (FRAME_sendUpdateResponse(client_fd, server->data.version, server->data.value) == EXIT_FAILURE) return EXIT_FAILURE;
    return EXIT_SUCCESS;
}

int TRANSACTION_replyUpdateCommon(int client_fd, int id_server, int id_trans, Server *server, Operation operation) {
    int next_fd, size;
    char *buffer;
    if (TOOLS_connect_server(&next_fd,
                             server->next_server_direction.ip_address,
                             server->next_server_direction.passive_port) == EXIT_FAILURE) return EXIT_NEXT_DOWN;

    if (FRAME_sendUpdateRequest(next_fd, id_server, id_trans, operation) == EXIT_FAILURE) return EXIT_NEXT_DOWN;
    if (FRAME_readUpdateResponse(next_fd, &(server->data.version), &(server->data.value)) == EXIT_FAILURE) return EXIT_NEXT_DOWN;

    size = asprintf(&buffer, BOLDGREEN "Updated value recieved: %d v_%d\n\n" RESET, server->data.value, server->data.version);
    write(1, buffer, size);
    free(buffer);

    TOOLS_copyNextServerDirection(id_server, &(server->next_server_direction), *server);

    // Enviamos la respuesta al que nos ha preguntado a nosotros
    if (FRAME_sendUpdateResponse(client_fd, server->data.version, server->data.value) == EXIT_FAILURE) return EXIT_FAILURE;
    return EXIT_SUCCESS;
}

void TRANSACTION_reconnect(Server * server) {
    int i, return_val, ping_fd, version, value, isFirst, firstFound, size, new_server = 0;
    char *buffer;

    for (i=0; i < server->total_servers; i++) {
        return_val = TOOLS_connect_server(&ping_fd,
                                            server->servers_directions[i].ip_address,
                                            server->servers_directions[i].ping_port); 
        
        if (return_val == EXIT_SUCCESS) return_val = FRAME_sendPingRequest(ping_fd);
        if (return_val == EXIT_SUCCESS) return_val = FRAME_readPingResponse(ping_fd, &version, &value, &isFirst);

        switch (return_val) {
            case EXIT_FAILURE:
                size = asprintf(&buffer, BOLDRED "Server %d is down. Removing it...\n" RESET, server->servers_directions[i].id_server);
                write(1, buffer, size);
                free(buffer);

                TOOLS_removeDirection(server->servers_directions[i].id_server, &(server->servers_directions), &(server->total_servers));
                i--;
                break;

            case EXIT_SUCCESS:
                size = asprintf(&buffer, BOLDGREEN "Server %d is active.\n" RESET, server->servers_directions[i].id_server);
                write(1, buffer, size);
                free(buffer);
                
                if (isFirst || 
                    (!firstFound && ((version > server->data.version) || 
                                    (version == server->data.version && server->servers_directions[i].id_server > server->my_direction.id_server)))) {
                    server->data.version = version;
                    server->data.value = value;
                    TOOLS_copyDirection(&(server->next_server_direction), server->servers_directions[i]);
                    new_server = 1;
                    if (isFirst) firstFound = 1;
                } 
                
                if (version == server->data.version && value != server->data.value) {
                    size = asprintf(&buffer, BOLDRED "Server %d has the same version with different value\n" RESET, server->servers_directions[i].id_server);
                    write(1, buffer, size);
                    free(buffer);
                }
                break;
        }
        close(ping_fd);
        ping_fd = -1;
    }

    if (!new_server) {
        server->next_server_direction.id_server = -1;
    }
    
    TOOLS_printDirections(server->servers_directions, server->total_servers);
    size = asprintf(&buffer, BOLDBLUE "Next server: %d" RESET, server->next_server_direction.id_server);
    write(1, buffer, size);
    free(buffer);
}
