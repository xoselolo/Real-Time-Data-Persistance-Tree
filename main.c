#include "config.h"
#include "passive.h"
#include "types.h"
#include "transaction.h"

extern int server_fd;
extern int client_fd;

static void sigint() {
    if (server_fd != -1) {
        close(server_fd);
        server_fd = -1;
    }
    if (client_fd != -1) {
        close(client_fd);
        client_fd = -1;
    }
    raise(SIGKILL);
}

int main(int argc, char** argv) {

    Server server;
    char *buffer;
    int size;

    signal(SIGINT, sigint);
    if (argc != 2){
        printf("---- ERROR: Config file name must be provided as argument!\n");
    }else{
        int active_fd;
        pthread_t t_passive;
        //pthread_t t_ping;

        server.data.value = 0;
        server.data.version = 0;

        server = readConfig(argv[1]);

        //printf("READ\n");
        // TODO: connection protocol
        if (TRANSACTION_sendConnect(&server) == EXIT_FAILURE) {
            size = asprintf(&buffer, BOLDRED "Unable to connect to the servers.\n" RESET);
            write(1, buffer, size);
            free(buffer);
            raise(SIGINT);
        }

        pthread_create(&t_passive, NULL, PASSIVE_server, &server);

        // TODO: Create ping thread


       /* while(1) {
            int option = TOOLS_displayMenu();
            switch (option) {
                case 1:
                
                    break;
            }
        }*/


        for(int i = 0; i < 10; i++){
            printf("\n\n\n\n");
            // I'm the first or the top server
            if(server.next_server_direction.id_server < 0 || server.is_first == 1){ // NO hem de connectar-nos amb ningú
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
            }else{
                // CONNECT to (passive) next server
                if (TOOLS_connect_server(&active_fd, server.next_server_direction.ip_address, server.next_server_direction.passive_port) == EXIT_SUCCESS){
                    printf("I'm not the first\n");
                    printf("--------- CONNECTED TO id_server %d\n", server.next_server_direction.id_server);
                    if(server.is_read_only == 'R'){

                        // create the transaction id and send the READ REQUEST
                        int id_transaction = TRANSACTION_generateId(server.transaction_trees[0]);
                        if(FRAME_sendReadRequest(active_fd, server.my_direction.id_server, id_transaction) == EXIT_SUCCESS){
                            TRANSACTION_BINARY_TREE_add(&(server.transaction_trees[0]), id_transaction, server.my_direction.id_server);

                            //printf("Server %d transaction %d added!\n", server.my_direction.id_server, id_transaction);

                            // active wait for response
                            if(FRAME_readReadResponse(active_fd, &(server.data.version), &(server.data.value)) == EXIT_SUCCESS){
                                printf("Value (GET) = %d\nv_%d\n\n", server.data.value, server.data.version);
                                FRAME_sendAck(active_fd);
                                //printf("I'm top!\n");
                                server.is_first = 1;
                                server.next_server_direction.id_server = -1;
                            }
                        }

                    }else{
                        // create the transaction id and send the READ REQUEST
                        int id_transaction = TRANSACTION_generateId(server.transaction_trees[0]);
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
                        }
                    }
                    close(active_fd); // Desconnectem
                }else{
                    break;
                }
            }

            //printf("---- Sleep %d\n", i);
            sleep(server.sleep_time);

        }

    }


    //int fd_client;
    //TOOLS_connect_server(&fd_client, "127.0.0.1", 8840);

    while (1) {
        pause();
    }
    
    return 0;
}
