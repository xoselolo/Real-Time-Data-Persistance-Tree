#include "types.h"
#include "passive.h"

Server server;

int main() {
    pthread_t t_passive, t_ping;
    int a = 0;
    server.my_direction.ip_address="127.0.0.1";
    server.my_direction.passive_port = 8840;
    printf("Hello World!\n");

    pthread_create(&t_passive, NULL, PASSIVE_server, &server.my_direction);

    return 0;
}
