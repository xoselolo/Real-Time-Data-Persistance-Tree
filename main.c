#include "config.h"
#include "passive.h"
#include "ping.h"
#include "types.h"
#include "transaction.h"

extern int server_fd;
extern int client_fd;
extern int ping_fd;
extern int ping_client_fd;
semaphore sem_read_response;

static void sigint() {
    if (server_fd != -1) {
        close(server_fd);
        server_fd = -1;
    }
    if (client_fd != -1) {
        close(client_fd);
        client_fd = -1;
    }
    if (ping_fd != -1) {
        close(ping_fd);
        ping_fd = -1;
    }
    if (ping_client_fd != -1) {
        close(ping_client_fd);
        ping_client_fd = -1;
    }
    raise(SIGKILL);
}

int main(int argc, char** argv) {

    Server server;
    char *buffer;
    int size, return_val;
    signal(SIGINT, sigint);

    if (argc != 2){
        printf("---- ERROR: Config file name must be provided as argument!\n");
    }else{
        SEM_constructor(&sem_read_response);
	    SEM_init(&sem_read_response, 0);

        pthread_t t_passive;
        pthread_t t_ping;

        server.data.value = -1;
        server.data.version = -1;
        server = readConfig(argv[1]);

        if (TRANSACTION_sendConnect(&server) == EXIT_FAILURE) {
            size = asprintf(&buffer, BOLDRED "Unable to connect to servers.\n" RESET);
            write(1, buffer, size);
            free(buffer);
            raise(SIGINT);
        }

        pthread_create(&t_passive, NULL, PASSIVE_server, &server);
        pthread_create(&t_ping, NULL, PING_server, &server);

        // TODO: Create ping thread

        for(int i = 0; i < 10; i++){
            // I'm the first or the top server
            if(server.next_server_direction.id_server == -1){ // NO hem de connectar-nos amb ningú
                printf("I'm first\n");
                if(server.is_read_only == 'R'){
                    printf("Value (GET) v_%d  == %d\n", server.data.version, server.data.value);
                }else{
                    switch (server.operation.operator) {
                        case '+':
                            server.data.value += server.operation.operand;
                            break;
                        case '-':
                            server.data.value -= server.operation.operand;
                            break;
                        case '*':
                            server.data.value *= server.operation.operand;
                            break;
                        case '/':
                            server.data.value /= server.operation.operand;
                            break;
                    }
                    server.data.version++;
                    printf("Value (UPDATE) v_%d  == %d\n", server.data.version, server.data.value);

                }
            }
            else{
                // CONNECT to (passive) next server
                if(server.is_read_only == 'R'){
                    return_val = TRANSACTION_readActive(server);

                } else{
                    return_val = TRANSACTION_updateActive(server);
                    // create the transaction id and send the READ REQUEST
                    /*int id_transaction = TRANSACTION_generateId(server.transaction_trees[0]);
                    if(FRAME_sendUpdateRequest(active_fd, server.my_direction.id_server, id_transaction, server.operation) == EXIT_SUCCESS){
                        TRANSACTION_BINARY_TREE_add(&(server.transaction_trees[0]), id_transaction, server.my_direction.id_server);

                        //printf("Server %d transaction %d added!\n", server.my_direction.id_server, id_transaction);

                        // active wait for response
                        if(FRAME_readUpdateResponse(active_fd, &(server.data.version), &(server.data.value)) == EXIT_SUCCESS){
                            printf("Value (UPDATE) = %d\nv_%d\n\n", server.data.value, server.data.version);

                            FRAME_sendAck(active_fd);
                            server.is_first = 1;
                            server.next_server_direction.id_server = -1;
                        }
                    }*/
                }

                switch(return_val) {
                    case EXIT_NEXT_DOWN:
                        size = asprintf(&buffer, BOLDRED "Next server is down, trying to reconnect...\n" RESET);
                        write(1, buffer, size);
                        free(buffer);
                        TRANSACTION_reconnect(&server);
                        break;
                }


            }

            sleep(server.sleep_time);

        }

    }

    while (1) {
        pause();
    }
    
    return 0;
}
