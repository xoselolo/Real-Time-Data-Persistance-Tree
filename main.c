#include "types.h"
#include "passive.h"
#include "config.h"

Server server;

int main(int argc, char** argv) {
    pthread_t t_passive, t_ping;

    server = readConfig(argv[1]);

    server.my_direction.ip_address="127.0.0.1";
    server.my_direction.passive_port = 8840;

    pthread_create(&t_passive, NULL, PASSIVE_server, &server.my_direction);

    // TODO: Create ping thread

    // TODO: Connect to next server passive port
    if (server.is_read_only == 'R'){
        for(int i = 0; i < 10; i++){
            // TODO: GET value
            // TODO: UPDATE value
            sleep(server.sleep_time);
        }
    }else{
        for(int i = 0; i < 10; i++){
            // TODO: GET value
            sleep(server.sleep_time);
        }
    }

    return 0;
}
