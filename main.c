#include "config.h"
#include "passive.h"
#include "types.h"

int main(int argc, char** argv) {

    Server server;

    if (argc != 2){
        printf("---- ERROR: Config file name must be provided as argument!\n");
    }else{
        int active_fd;
        pthread_t t_passive;
        //pthread_t t_ping;

        server.data.value = 0;
        server.data.version = 0;

        server = readConfig(argv[1]);

        //server.my_direction.ip_address="127.0.0.1";
        //server.my_direction.passive_port = 8840;

        // TODO: connection protocol

        pthread_create(&t_passive, NULL, PASSIVE_server, &server);

        // TODO: Create ping thread


        for(int i = 0; i < 10; i++){
            // I'm the first or the top server
            if(server.next_server_direction.id_server < 0){ // NO hem de connectar-nos amb ningú
                if(server.is_read_only == 'R'){
                    printf("Value (GET) = %d\n", server.data.value);
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
                    printf("Value (UPDATE) = %d\n", server.data.value);
                }
            }else{
                // CONNECT to (passive) next server
                if (TOOLS_connect_server(&active_fd, server.next_server_direction.ip_address, server.next_server_direction.passive_port) == EXIT_SUCCESS){
                    if(server.is_read_only == 'R'){
                        // TODO: GET value (trama)
                    }else{
                        // TODO: UPDATE value (trama)
                    }
                }else{
                    break;
                }
            }

            printf("---- Sleep %d\n", i);
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
