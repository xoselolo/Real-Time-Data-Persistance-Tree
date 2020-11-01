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

    int id_server, id_trans;

    TOOLS_open_psocket(&server_fd, server->my_direction.ip_address, server->my_direction.passive_port);
    server->fd_passive = server_fd;

    size = asprintf(&buffer, BOLDGREEN "Passive server started at %s:%d\n" RESET, server->my_direction.ip_address, server->my_direction.passive_port);
    write(1, buffer, size);
    free(buffer);

    while (1) {

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
                FRAME_sendReadResponse(client_fd, server->data.version, server->data.value); // TESTING: m'esta enviant 0,0 ?¿?¿?¿?¿
                // todo --> TRANSACTION_readPassive(server, client_fd, id_server, id_trans);
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