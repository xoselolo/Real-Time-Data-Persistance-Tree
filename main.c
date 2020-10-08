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

    return 0;
}
