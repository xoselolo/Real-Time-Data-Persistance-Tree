
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

int TRANSACTION_readPassive(Server *server, int fd_client, int id_server, int id_trans) {
    int fd;
    int value, version;
    Direction aux;

    // TODO: AFEGIR EL IDSERVER DEL QUE TRANSMET EL MISSATGE: per a que de tornada no li torni a arribar al que la creat

    printf("\n\n STEP 1\n");
    if (server->next_server_direction.id_server != -1) {

        printf("STEP 2\n");
        if (TOOLS_connect_server(&fd, server->next_server_direction.ip_address, server->next_server_direction.passive_port) != EXIT_SUCCESS)
            return EXIT_FAILURE;

        printf("STEP 3\n");
        if (FRAME_sendReadRequest(fd, id_server, id_trans) != EXIT_SUCCESS)
            return EXIT_FAILURE;

        printf("STEP 4\n");
        if (FRAME_readReadResponse(fd, &version, &value) != EXIT_SUCCESS)
            return EXIT_FAILURE;

        printf("STEP 5\n");
        server->next_server_direction = TOOLS_findDirection(server->servers_directions, server->total_servers, id_server);
        server->data.version = version;
        server->data.value = value;

        if (FRAME_sendReadResponse(fd_client, version, value) != EXIT_SUCCESS)
            return EXIT_FAILURE;

    } else {
        printf("STEP 6\n");
        aux = TOOLS_findDirection(server->servers_directions, server->total_servers, id_server);
        if (aux.id_server != -1) {
            printf("STEP 7A\n");
            server->next_server_direction = aux;
        } else {
            printf("STEP 7B\n");
            return ERR_SRVR_NOT_FOUND;
        }

        printf("STEP 8\n");
        if (TOOLS_connect_server(&fd, server->next_server_direction.ip_address, server->next_server_direction.passive_port) != EXIT_SUCCESS)
            return EXIT_FAILURE;

        printf("STEP 9\n");
        if (FRAME_sendReadResponse(fd, server->data.value, server->data.version) != EXIT_SUCCESS)
            return EXIT_FAILURE;

        printf("STEP 10\n");
        if (FRAME_readAck(fd) != EXIT_SUCCESS)
            return ERR_ACK;

        printf("STEP 11\n");
        if (FRAME_sendReadResponse(fd_client, server->data.value, server->data.version) != EXIT_SUCCESS)
            return EXIT_FAILURE;

    }

    return EXIT_SUCCESS;
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
        TOOLS_printServerDirections(*server);
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

    TOOLS_printServerDirections(*server);

    if (server->is_first) {
        if (FRAME_sendFirstConnectionResponse(fd_client, *server) == EXIT_FAILURE) return EXIT_FAILURE;
    } else {
        if (FRAME_sendConnectionResponse(fd_client, *server) == EXIT_FAILURE) return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int TRANSACTION_readActive(Server server) {
    int active_fd, version, value, size;
    char *buffer;

    if (TOOLS_connect_server(&active_fd, server.next_server_direction.ip_address, server.next_server_direction.passive_port) == EXIT_FAILURE) {
        close(active_fd);
        return EXIT_FAILURE;
    }

    int id_transaction = TRANSACTION_generateId(server.transaction_trees[0]);
    if(FRAME_sendReadRequest(active_fd, server.my_direction.id_server, id_transaction) == EXIT_FAILURE) {
        close(active_fd);
        return EXIT_FAILURE;
    }

    size = asprintf(&buffer, BOLDYELLOW "Read request sent to server %d, %s:%d\n" RESET, server.next_server_direction.id_server, server.next_server_direction.ip_address, server.next_server_direction.passive_port);
    write(1, buffer, size);
    free(buffer);

    TRANSACTION_BINARY_TREE_add(&(server.transaction_trees[0]), id_transaction, server.my_direction.id_server);

    // wait for response
    SEM_init(&sem_read_response, 0);
    SEM_wait(&sem_read_response);
    
    if(FRAME_readReadResponse(active_fd, &version, &value) == EXIT_FAILURE) {
        close(active_fd);
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int TRANSACTION_readResponsePassive(int fd_client, Server *server) {
    int size;
    char *buffer;

    if(FRAME_readOriginReadResponse(fd_client, &(server->data.version), &(server->data.value)) == EXIT_FAILURE) return EXIT_FAILURE;
    FRAME_sendAck(fd_client);
    server->next_server_direction.id_server = -1;
    size = asprintf(&buffer, BOLDGREEN "Response received %d v_%d\n\n" RESET, server->data.version, server->data.value);
    write(1, buffer, size);
    free(buffer);
    SEM_signal(&sem_read_response);
    return EXIT_SUCCESS;
}

int TRANSACTION_replyReadLastUpdated(int client_fd, int id_server, Server *server) {
    Direction origin = TOOLS_findDirection(server->servers_directions, server->total_servers, id_server);
    int origin_fd, size;
    char *buffer;

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
                             server->next_server_direction.passive_port) == EXIT_FAILURE) return EXIT_FAILURE; 

    if (FRAME_sendReadRequest(next_fd, id_server, id_trans) == EXIT_FAILURE) return EXIT_FAILURE;
    if (FRAME_readReadResponse(next_fd, &(server->data.version), &(server->data.value)) == EXIT_FAILURE) return EXIT_FAILURE;

    size = asprintf(&buffer, BOLDGREEN "Updated value recieved: %d v_%d\n\n" RESET, server->data.value, server->data.version);
    write(1, buffer, size);
    free(buffer);

    TOOLS_copyNextServerDirection(id_server, &(server->next_server_direction), *server);
    
    // Enviamos la respuesta al que nos ha preguntado a nosotros
    if (FRAME_sendReadResponse(client_fd, server->data.version, server->data.value) == EXIT_FAILURE) return EXIT_FAILURE;
    return EXIT_SUCCESS;
}