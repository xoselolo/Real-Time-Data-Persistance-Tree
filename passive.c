#include "passive.h"

int server_fd = -1;
int client_fd = -1;
extern semaphore sem_passive;

void * PASSIVE_server(void * arg) {

    Server *server = (Server *)(arg);
    char *buffer;
    int size;
    int return_val = EXIT_SUCCESS;
    struct sockaddr_in s_addr;
    socklen_t len = sizeof(s_addr);
    int type;
    Operation operation;

    int id_server;

    TOOLS_open_psocket(&server_fd, server->my_direction.ip_address, server->my_direction.passive_port);

    size = asprintf(&buffer, BOLDGREEN "Passive server started at %s:%d\n" RESET, server->my_direction.ip_address, server->my_direction.passive_port);
    write(1, buffer, size);
    free(buffer);
    SEM_signal(&sem_passive);

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
                return_val = FRAME_readReadRequest(client_fd, &id_server);
                if (return_val == EXIT_FAILURE) break;
                // check if i'm top or not

                do {
                    if(server->next_server_direction.id_server == -1){
                        return_val = TRANSACTION_replyReadLastUpdated(client_fd, id_server, server);

                    } else {
                        return_val = TRANSACTION_replyReadCommon(client_fd, id_server, server);
                        if (return_val == EXIT_NEXT_DOWN) {
                            size = asprintf(&buffer, BOLDRED "Next server is down, trying to reconnect...\n" RESET);
                            write(1, buffer, size);
                            free(buffer);
                            TRANSACTION_reconnect(server);
                        }
                    } 
                } while (return_val == EXIT_NEXT_DOWN);
                break;

            case UPDATE:
                return_val = FRAME_readUpdateRequest(client_fd, &id_server, &operation);
                if (return_val == EXIT_FAILURE) break;

                do {
                    if(server->next_server_direction.id_server == -1){
                        return_val = TRANSACTION_replyUpdateLastUpdated(client_fd, id_server, server, operation);

                    } else {
                        return_val = TRANSACTION_replyUpdateCommon(client_fd, id_server, server, operation);
                        if (return_val == EXIT_NEXT_DOWN) {
                            size = asprintf(&buffer, BOLDRED "Next server is down, trying to reconnect...\n" RESET);
                            write(1, buffer, size);
                            free(buffer);
                            TRANSACTION_reconnect(server);
                        }
                    } 
                } while (return_val == EXIT_NEXT_DOWN);
                break;

            case READ_RESPONSE:
                return_val = TRANSACTION_readResponsePassive(client_fd, server);
                break;

            case UPDATE_RESPONSE:
                return_val = TRANSACTION_updateResponsePassive(client_fd, server);
                break;
        }

        switch (return_val) {
            case EXIT_FAILURE:
                size = asprintf(&buffer, BOLDRED "Error on transaction of type %c\n" RESET, type);
                write(1, buffer, size);
                free(buffer);
                break;
        }

        close(client_fd);
        client_fd = -1;
    }
}