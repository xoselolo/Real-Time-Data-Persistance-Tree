#include "ping.h"

int ping_fd = -1;
int ping_client_fd = -1;
extern semaphore sem_ping;

void * PING_server(void * arg) {

    Server *server = (Server *)(arg);
    char *buffer;
    int size;
    
    struct sockaddr_in s_addr;
    socklen_t len = sizeof(s_addr);

    TOOLS_open_psocket(&ping_fd, server->my_direction.ip_address, server->my_direction.ping_port);

    size = asprintf(&buffer, BOLDMAGENTA "Ping server started at %s:%d\n" RESET, server->my_direction.ip_address, server->my_direction.ping_port);
    write(1, buffer, size);
    free(buffer);
    SEM_signal(&sem_ping);

    while (1) {
        ping_client_fd = accept(ping_fd, (void *) &s_addr, &len); 

        if (FRAME_readPingRequest(ping_client_fd) == EXIT_SUCCESS) {
            FRAME_sendPingResponse(ping_client_fd, server->data.version, server->data.value, server->next_server_direction.id_server == -1);
        }

        close(ping_client_fd);
        ping_client_fd = -1;
    }
    write(1, "out ping\n", strlen("out ping\n"));
    return NULL;
}