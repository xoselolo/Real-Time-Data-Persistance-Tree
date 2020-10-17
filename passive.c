#include "passive.h"

void * PASSIVE_server(void * arg) {

    Server *server = (Server *)(arg);

    printf("thread started  %s %d\n", server->my_direction.ip_address, server->my_direction.passive_port);
    int server_fd, client_fd, size;
    struct sockaddr_in s_addr;
    socklen_t len = sizeof(s_addr);
    Frame frame;
    Operation operation;

    int id_server, id_trans;

    TOOLS_open_psocket(&server_fd, server->my_direction.ip_address, server->my_direction.passive_port);
    write(1, "socket opened\n", strlen("socket opened\n"));

    while (1) {

        client_fd = accept(server_fd, (void *) &s_addr, &len);
        write(1, "client accepted\n", strlen("client accepted\n"));
        memset(&frame, 0, sizeof(Frame));
        
        size = read(client_fd, &frame.type, 1);

        if (size != 1) {
            perror(ERR_CONN);
        }

        switch (frame.type) {
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

        free(frame.data);
    }
}