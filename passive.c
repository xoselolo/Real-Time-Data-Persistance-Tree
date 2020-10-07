
#include "passive.h"

void * PASSIVE_server(void * arg) {

    Direction direction = *(Direction *)(arg);

    printf("thread started  %s %d\n", direction.ip_address, direction.passive_port);
    int server_fd, client_fd, size;
    struct sockaddr_in s_addr;
    socklen_t len = sizeof(s_addr);
    Frame frame;
    
    TOOLS_open_psocket(&server_fd, direction.ip_address, direction.passive_port);
    write(1, "socket opened\n", strlen("socket opened\n"));

    while (1) {
        client_fd = accept(server_fd, (void *) &s_addr, &len);
        write(1, "client accepted\n", strlen("client accepted\n"));
        memset(&frame, 0, sizeof(Frame));
        
        size = read(client_fd, &frame.type, 1);

        if (size != 1) {
            write(1,"errror connexio\n", strlen("errror connexio\n"));
        }

        if (frame.type == 'R') {
            frame.data = TOOLS_read_until(client_fd, '\0');
        }

        free(frame.data);
    }
}