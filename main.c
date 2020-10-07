#include "config.h"
#include "passive.h"
#include "types.h"

Server server;

int main(int argc, char** argv) {
    pthread_t t_passive;  //, t_ping;

    if (argc != 2) {
        write(1, ERR_ARGS, strlen(ERR_ARGS));
        return EXIT_FAILURE;
    }

    //server = readConfig(argv[1]);

    server.my_direction.ip_address = "127.0.0.1";
    server.my_direction.passive_port = 8840;

    pthread_create(&t_passive, NULL, PASSIVE_server, &server.my_direction);

    int fd_client;
    //TOOLS_connect_server(&fd_client, "127.0.0.1", 8840);

    while (1) {
        pause();
    }
    
    return 0;
}
