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
    int fd_passive_to_next;

    int id_server, id_trans;

    TOOLS_open_psocket(&server_fd, server->my_direction.ip_address, server->my_direction.passive_port);
    server->fd_passive = server_fd;

    size = asprintf(&buffer, BOLDGREEN "Passive server started at %s:%d\n" RESET, server->my_direction.ip_address, server->my_direction.passive_port);
    write(1, buffer, size);
    free(buffer);

    while (1) {
        fd_passive_to_next = -1;
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
                // TODO: NEEDS TO BE TESTED
                FRAME_readReadRequest(client_fd, &id_server, &id_trans);
                //Put the transaction on the tree
                int index_tree = TRANSACTION_BINARY_TREE_findRoot(server->transaction_trees, id_server, server->total_servers);
                if (TRANSACTION_BINARY_TREE_exists(server->transaction_trees[index_tree], id_trans) == 0){
                    TRANSACTION_BINARY_TREE_add(&(server->transaction_trees[index_tree]), id_trans, id_server);
                }else{
                    perror(ERR_TRANSACTION_EXISTS);
                }
                // check if i'm top or not
                if(server->is_first == 1){
                    FRAME_sendReadResponse(client_fd, server->data.version, server->data.value);
                }else{
                    // connect to next and send him the read request
                    if (TOOLS_connect_server(&fd_passive_to_next, server->next_server_direction.ip_address, server->next_server_direction.passive_port) == EXIT_SUCCESS){
                        // le enviamos al next la request
                        FRAME_sendReadRequest(fd_passive_to_next, id_server, id_trans);
                        // Esperamos a que responda
                        FRAME_readReadResponse(fd_passive_to_next, &(server->data.version), &(server->data.value));
                        // Nos reapuntamos al mas actualizado
                        server->next_server_direction.id_server = id_server;
                        TOOLS_copyNextServerDirection(id_server, &(server->next_server_direction), *server);
                        // Enviamos la respuesta al que nos ha preguntado a nosotros
                        FRAME_sendReadResponse(client_fd, server->data.version, server->data.value);
                    }
                }

                break;

            case UPDATE:
                printf("---- update received ----\n");
                FRAME_readUpdateRequest(client_fd, &id_server, &id_trans, &operation);
                //printf("Pre (%c): %d \n", operation.operator, server->data.value);
                switch (operation.operator) {
                    case '+':
                        printf("suma\n");
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
                //printf("Post: %d \n", server->data.value);
                FRAME_sendUpdateResponse(client_fd, server->data.version, server->data.value);
                // todo --> TRANSACTION_updatePassive

                break;

            case ACK:

                break;
        }

        switch (return_val) {
        case EXIT_FAILURE:
            /* code */
            break;
        
        default:
            break;
        }

        close(client_fd);
        client_fd = -1;
    }
}