
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
                                        server->my_direction.ping_port) == EXIT_FAILURE) {
                                            close(server_fd);
                                            server_fd = -1;
                                            return EXIT_FAILURE;
                                        }

        if (FRAME_readFirstConnectionResponse(server_fd, server) == EXIT_FAILURE) {
            close(server_fd);
            server_fd = -1;
            return EXIT_FAILURE;
        }
        close(server_fd);
        server_fd = -1;

        for (i=1; i < server->total_servers; i++) {
            size = asprintf(&buffer, BOLD "\nInicialitzant connexió amb el servidor %d\n" RESET, server->servers_directions[i].id_server);
            write(1, buffer, size);
            free(buffer);

            if (TOOLS_connect_server(&server_fd, 
                                server->servers_directions[i].ip_address, 
                                server->servers_directions[i].passive_port) == EXIT_FAILURE) {
                                    size = asprintf(&buffer, BOLDRED "Server %d is down. Removing it...\n" RESET, server->servers_directions[i].id_server);
                                    write(1, buffer, size);
                                    free(buffer);

                                    TOOLS_removeDirection(server->servers_directions[i].id_server, &(server->servers_directions), &(server->total_servers));
                                    
                                    continue;
                                }
            
            if (FRAME_sendConnectionRequest(server_fd, 
                                        server->my_direction.id_server,
                                        server->my_direction.ip_address, 
                                        server->my_direction.passive_port, 
                                        server->my_direction.ping_port) == EXIT_FAILURE) {
                                            close(server_fd);
                                            server_fd = -1;
                                            return EXIT_FAILURE;
                                        }
            
            if (FRAME_readConnectionResponse(server_fd, &id_server, &version, &value) == EXIT_FAILURE) {
                close(server_fd);
                server_fd = -1;
                return EXIT_FAILURE;
            }

            if (version > server->data.version) {
                server->next_server_direction = TOOLS_findDirection(server->servers_directions, server->total_servers, id_server);
                server->data.value = value;
                server->data.version = version;
            }
            close(server_fd);
            server_fd = -1;
        }

        size = asprintf(&buffer, BOLD "Connexions inicialitzades\n" RESET);
        write(1, buffer, size);
        free(buffer);
        TOOLS_printDirections(*server);
    }

    return EXIT_SUCCESS;
}

int TRANSACTION_connectPassive(int fd_client, Server *server) {
    char first_notFirst;
    char *ip_addr;
    int passive_port, ping_port, id_server;
    Direction new_direction;

    if (read(fd_client, &first_notFirst, sizeof(char)) != sizeof(char)) return EXIT_FAILURE;
    if ((server->is_first && first_notFirst != 'F') || (!server->is_first && first_notFirst != 'N')) return EXIT_FAILURE;

    if (FRAME_readConnectionRequest(fd_client, &id_server, &ip_addr, &passive_port, &ping_port) == EXIT_FAILURE) return EXIT_FAILURE;

    new_direction.id_server = id_server;
    new_direction.ip_address = ip_addr;
    new_direction.passive_port = passive_port;
    new_direction.ping_port = ping_port;

    if (!TOOLS_replaceDirection(server->servers_directions, server->total_servers, new_direction)) {
        server->servers_directions[server->total_servers].id_server = id_server;
        server->servers_directions[server->total_servers].ip_address = ip_addr;
        server->servers_directions[server->total_servers].passive_port = passive_port;
        server->servers_directions[server->total_servers].ping_port = ping_port;
        
        (server->total_servers)++;
        server->servers_directions = (Direction *)realloc(server->servers_directions, sizeof(Direction)*(server->total_servers + 1));
    }

    TOOLS_printDirections(*server);

    if (server->is_first) {
        if (FRAME_sendFirstConnectionResponse(fd_client, *server) == EXIT_FAILURE) return EXIT_FAILURE;
    } else {
        if (FRAME_sendConnectionResponse(fd_client, *server) == EXIT_FAILURE) return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int TRANSACTION_readActive(Server server, int i) {
    int next_fd, version, value, size;
    char *buffer;

    if (TOOLS_connect_server(&next_fd, server.next_server_direction.ip_address, server.next_server_direction.passive_port) == EXIT_FAILURE) {
        close(next_fd);
        next_fd = -1;
        return EXIT_NEXT_DOWN;
    }

    SEM_init(&sem_read_response, 0);
    if(FRAME_sendReadRequest(next_fd, server.my_direction.id_server) == EXIT_FAILURE) {
        close(next_fd);
        next_fd = -1;
        return EXIT_NEXT_DOWN;
    }

    size = asprintf(&buffer, BOLDYELLOW "\t[%d] - Read request sent to server %d, %s:%d\n" RESET, i, server.next_server_direction.id_server, server.next_server_direction.ip_address, server.next_server_direction.passive_port);
    write(1, buffer, size);
    free(buffer);

    // wait for response
    SEM_wait(&sem_read_response);
    
    if(FRAME_readReadResponse(next_fd, &version, &value) == EXIT_FAILURE) {
        close(next_fd);
        next_fd = -1;
        return EXIT_NEXT_DOWN;
    }
    close(next_fd);
    next_fd = -1;
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
    int origin_fd;

    Direction origin = TOOLS_findDirection(server->servers_directions, server->total_servers, id_server);

    if (TOOLS_connect_server(&origin_fd, 
                            origin.ip_address, 
                            origin.passive_port) == EXIT_FAILURE) return EXIT_FAILURE;
    if (FRAME_sendOriginReadResponse(origin_fd, server->data.version, server->data.value) == EXIT_FAILURE) {
        close(origin_fd);
        origin_fd = -1;
        return EXIT_FAILURE;
    }
    if (FRAME_readAck(origin_fd) == EXIT_FAILURE) {
        close(origin_fd);
        origin_fd = -1;
        return EXIT_FAILURE;
    } 
    close(origin_fd);
    origin_fd = -1;
    TOOLS_copyNextServerDirection(id_server, &(server->next_server_direction), *server);

    if (FRAME_sendReadResponse(client_fd, server->data.version, server->data.value) == EXIT_FAILURE) return EXIT_FAILURE;
    
    return EXIT_SUCCESS;
}

int TRANSACTION_replyReadCommon(int client_fd, int id_server, Server *server) {
    int next_fd, size;
    char *buffer;
    if (TOOLS_connect_server(&next_fd,
                             server->next_server_direction.ip_address, 
                             server->next_server_direction.passive_port) == EXIT_FAILURE) return EXIT_NEXT_DOWN; 

    if (FRAME_sendReadRequest(next_fd, id_server) == EXIT_FAILURE) {
        close(next_fd);
        next_fd = -1;
        return EXIT_NEXT_DOWN;
    }
    if (FRAME_readReadResponse(next_fd, &(server->data.version), &(server->data.value)) == EXIT_FAILURE) {
        close(next_fd);
        next_fd = -1;
        return EXIT_NEXT_DOWN;
    }
    close(next_fd);
    next_fd = -1;
    
    size = asprintf(&buffer, BOLDGREEN "Updated value received: %d v_%d \n\n" RESET, server->data.value, server->data.version);
    write(1, buffer, size);
    free(buffer);

    TOOLS_copyNextServerDirection(id_server, &(server->next_server_direction), *server);
    
    // Enviamos la respuesta al que nos ha preguntado a nosotros
    if (FRAME_sendReadResponse(client_fd, server->data.version, server->data.value) == EXIT_FAILURE) return EXIT_FAILURE;
    return EXIT_SUCCESS;
}



int TRANSACTION_updateActive(Server server, int i) {
    int next_fd, version, value, size;
    char *buffer;

    if (TOOLS_connect_server(&next_fd, server.next_server_direction.ip_address, server.next_server_direction.passive_port) == EXIT_FAILURE) {
        close(next_fd);
        next_fd = -1;
        return EXIT_NEXT_DOWN;
    }

    SEM_init(&sem_read_response, 0);
    if(FRAME_sendUpdateRequest(next_fd, server.my_direction.id_server, server.operation) == EXIT_FAILURE) {
        close(next_fd);
        next_fd = -1;
        return EXIT_NEXT_DOWN;
    }

    size = asprintf(&buffer, BOLDYELLOW "\t[%d] - Update request sent to server %d, %s:%d\n" RESET, i, server.next_server_direction.id_server, server.next_server_direction.ip_address, server.next_server_direction.passive_port);
    write(1, buffer, size);
    free(buffer);

    // wait for response
    SEM_wait(&sem_read_response);

    if(FRAME_readUpdateResponse(next_fd, &version, &value) == EXIT_FAILURE) {
        close(next_fd);
        next_fd = -1;
        return EXIT_NEXT_DOWN;
    }
    close(next_fd);
    next_fd = -1;
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
    int origin_fd;

    if (TOOLS_connect_server(&origin_fd,
                             origin.ip_address,
                             origin.passive_port) == EXIT_FAILURE) return EXIT_FAILURE;
    if (FRAME_sendOriginUpdateResponse(origin_fd, server->data.version, server->data.value) == EXIT_FAILURE) {
        close(origin_fd);
        origin_fd = -1;
        return EXIT_FAILURE;
    }
    if (FRAME_readAck(origin_fd) == EXIT_FAILURE) {
        close(origin_fd);
        origin_fd = -1;
        return EXIT_FAILURE;
    }
    close(origin_fd);
    origin_fd = -1;

    TOOLS_copyNextServerDirection(id_server, &(server->next_server_direction), *server);

    if (FRAME_sendUpdateResponse(client_fd, server->data.version, server->data.value) == EXIT_FAILURE) return EXIT_FAILURE;
    return EXIT_SUCCESS;
}

int TRANSACTION_replyUpdateCommon(int client_fd, int id_server, Server *server, Operation operation) {
    int next_fd, size;
    char *buffer;
    if (TOOLS_connect_server(&next_fd,
                             server->next_server_direction.ip_address,
                             server->next_server_direction.passive_port) == EXIT_FAILURE) return EXIT_NEXT_DOWN;

    if (FRAME_sendUpdateRequest(next_fd, id_server, operation) == EXIT_FAILURE) {
        close(next_fd);
        next_fd = -1;
        return EXIT_NEXT_DOWN;
    }
    if (FRAME_readUpdateResponse(next_fd, &(server->data.version), &(server->data.value)) == EXIT_FAILURE) {
        close(next_fd);
        next_fd = -1;
        return EXIT_NEXT_DOWN;
    }
    close(next_fd);
        next_fd = -1;

    size = asprintf(&buffer, BOLDGREEN "Updated value received: %d v_%d \n\n" RESET, server->data.value, server->data.version);
    write(1, buffer, size);
    free(buffer);

    TOOLS_copyNextServerDirection(id_server, &(server->next_server_direction), *server);

    // Enviamos la respuesta al que nos ha preguntado a nosotros
    if (FRAME_sendUpdateResponse(client_fd, server->data.version, server->data.value) == EXIT_FAILURE) return EXIT_FAILURE;
    return EXIT_SUCCESS;
}

void TRANSACTION_reconnect(Server * server) {
    int i, return_val, ping_fd, version, value, isFirst, firstFound = 0, size, new_server = 0;
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

        if (ping_fd != -1) {
            close(ping_fd);
            ping_fd = -1;
        }
    }

    if (!new_server) {
        server->next_server_direction.id_server = -1;
    }
    
    TOOLS_printDirections(*server);
}
