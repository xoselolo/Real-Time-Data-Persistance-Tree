#include "types.h"
#include "passive.h"
#include "config.h"

Server server;

int main(int argc, char** argv) {

    if (argc != 2){
        printf("---- ERROR: Config file name must be provided as argument!\n");
    }else{
        int active_fd, value = 0;
        pthread_t t_passive;
        //pthread_t t_ping;

        server = readConfig(argv[1]);

        server.my_direction.ip_address="127.0.0.1";
        server.my_direction.passive_port = 8840;

        // TODO: connection protocol

        pthread_create(&t_passive, NULL, PASSIVE_server, &server.my_direction);

        // TODO: Create ping thread


        for(int i = 0; i < 10; i++){
            // TODO: Connect to next server passive port
            if(server.next_server_direction.id_server < 0){ // NO hem de connectar-nos amb ningú
                // I'm the first or the top server
                if(server.is_read_only == 'R'){
                    printf("Value (GET) = %d\n", value);
                }else{
                    switch (server.operation.operator) {
                        case '+':
                            value += server.operation.operand;
                            break;
                        case '-':
                            value -= server.operation.operand;
                            break;
                        case '*':
                            value *= server.operation.operand;
                            break;
                        case '/':
                            value /= server.operation.operand;
                            break;
                    }
                    printf("Value (UPDATE) = %d\n", value);
                }
            }else{
                // CONNECT
                if (TOOLS_connect_server(&active_fd, server.next_server_direction.ip_address, server.next_server_direction.passive_port) == EXIT_SUCCESS){
                    if(server.is_read_only == 'R'){
                        // TODO: GET value (trama)
                        printf("Value (GET) = %d\n", value);
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


    return 0;
}
