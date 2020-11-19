#include "passive.h"

int server_fd = -1;
int client_fd = -1;

void * PASSIVE_server(void * arg) {

    Server *server = (Server *)(arg);
    char *buffer;
    int size;
    int return_val = EXIT_SUCCESS;
    struct sockaddr_in s_addr;
    socklen_t len = sizeof(s_addr);
    int type;
    Operation operation;
    //int fd_passive_to_next;

    int id_server, id_trans;

    TOOLS_open_psocket(&server_fd, server->my_direction.ip_address, server->my_direction.passive_port);
    server->fd_passive = server_fd;

    size = asprintf(&buffer, BOLDGREEN "Passive server started at %s:%d\n" RESET, server->my_direction.ip_address, server->my_direction.passive_port);
    write(1, buffer, size);
    free(buffer);

    while (1) {
        //fd_passive_to_next = -1;
        client_fd = accept(server_fd, (void *) &s_addr, &len);
        size = read(client_fd, &type, 1);

        if (size != 1) {
            perror(ERR_CONN);
        }
        
        switch (type) {
            case CONNECT:
                return_val = TRANSACTION_connectPassive(client_fd, server);
                break;

            case READ:
                FRAME_readReadRequest(client_fd, &id_server, &id_trans);
                //Put the transaction on the tree
                /* --> ToDo: Poner este cacho de código para comprobar si la transacción sigue en ciclo
                int index_tree = TRANSACTION_BINARY_TREE_findRoot(server->transaction_trees, id_server, server->total_servers);
                if (TRANSACTION_BINARY_TREE_exists(server->transaction_trees[index_tree], id_trans) == 0){
                    printf("--- ADD TRANSACTION\n");
                    TRANSACTION_BINARY_TREE_add(&(server->transaction_trees[index_tree]), id_trans, id_server);
                }else{
                    perror(ERR_TRANSACTION_EXISTS);
                }*/
                // check if i'm top or not
                if(server->next_server_direction.id_server == -1){
                    return_val = TRANSACTION_replyReadLastUpdated(client_fd, id_server, server);

                } else {
                    return_val = TRANSACTION_replyReadCommon(client_fd, id_server, id_trans, server);
                }

                break;

            case UPDATE:
                FRAME_readUpdateRequest(client_fd, &id_server, &id_trans, &operation);
                if(server->next_server_direction.id_server == -1){
                    return_val = TRANSACTION_replyUpdateLastUpdated(client_fd, id_server, server, operation);

                } else {
                    return_val = TRANSACTION_replyUpdateCommon(client_fd, id_server, id_trans, server, operation);
                }
                //Put the transaction on the tree
                /* --> ToDo: Poner este cacho de código para comprobar si la transacción sigue en ciclo
                int index_tree = TRANSACTION_BINARY_TREE_findRoot(server->transaction_trees, id_server, server->total_servers);
                if (TRANSACTION_BINARY_TREE_exists(server->transaction_trees[index_tree], id_trans) == 0){
                    printf("--- ADD TRANSACTION\n");
                    TRANSACTION_BINARY_TREE_add(&(server->transaction_trees[index_tree]), id_trans, id_server);
                }else{
                    perror(ERR_TRANSACTION_EXISTS);
                }
                // check if i'm top or not
                if(server->next_server_direction.id_server == -1){
                    switch (operation.operator) {
                        case '+':
                            //printf("suma\n");
                            server->data.value += operation.operand;
                            break;
                        case '-':
                            printf("resta\n");
                            server->data.value -= operation.operand;
                            break;
                        case '*':
                            printf("product\n");
                            server->data.value *= operation.operand;
                            break;
                        case '/':
                            printf("division\n");
                            server->data.value /= operation.operand;
                            break;
                    }
                    server->data.version++;
                    FRAME_sendUpdateResponse(client_fd, server->data.version, server->data.value);
                    FRAME_readAck(client_fd);
                    // Nos reapuntamos al mas actualizado
                    server->next_server_direction.id_server = id_server;
                    TOOLS_copyNextServerDirection(id_server, &(server->next_server_direction), *server);
                }
                else{
                    printf("--- 5 NOT FIRST\n");
                    // connect to next and send him the read request
                    if (TOOLS_connect_server(&fd_passive_to_next, server->next_server_direction.ip_address, server->next_server_direction.passive_port) == EXIT_SUCCESS){
                        // le enviamos al next la request
                        FRAME_sendUpdateRequest(fd_passive_to_next, id_server, id_trans, operation);
                        // Esperamos a que responda
                        FRAME_readUpdateResponse(fd_passive_to_next, &(server->data.version), &(server->data.value));
                        // Nos reapuntamos al mas actualizado
                        server->next_server_direction.id_server = id_server;
                        TOOLS_copyNextServerDirection(id_server, &(server->next_server_direction), *server);
                        // Enviamos la respuesta al que nos ha preguntado a nosotros
                        FRAME_sendUpdateResponse(client_fd, server->data.version, server->data.value);
                    }
                }*/
                break;

            case READ_RESPONSE:
                return_val = TRANSACTION_readResponsePassive(client_fd, server);
                break;

            case UPDATE_RESPONSE:
                return_val = TRANSACTION_updateResponsePassive(client_fd, server);

            case ACK:

                break;
        }

        switch (return_val) {
        case EXIT_FAILURE:
            size = asprintf(&buffer, BOLDRED "Error on transaction of type %c\n" RESET, type);
            write(1, buffer, size);
            free(buffer);
            break;
        
        default:
            break;
        }

        close(client_fd);
        client_fd = -1;
    }
}