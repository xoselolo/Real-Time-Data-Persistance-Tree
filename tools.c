#include "tools.h"

void TOOLS_open_psocket(int* socket_fd, char* ip, int port) {
    struct sockaddr_in s_addr;

    //Crear el socket
    *socket_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (*socket_fd < 0) {
        perror("socket creation");
        *socket_fd = -1;
        raise(SIGINT);
    }

    //Omplir l'adreça del servidor data amb el port i ip indicats
    memset(&s_addr, 0, sizeof(s_addr));

    s_addr.sin_family = AF_INET;
    if (port < 1 || port > 65535) {
        perror("invalid port");
        raise(SIGINT);
    }
    s_addr.sin_port = htons(port);

    s_addr.sin_addr.s_addr = inet_addr(ip);

    //Associar el socket amb l'adreça i el port indicats
    if (bind(*socket_fd, (void*)&s_addr, sizeof(s_addr)) < 0) {
        perror("bind");
        raise(SIGINT);

    } else {
        listen(*socket_fd, BACKLOG);
    }
}

int TOOLS_connect_server(int* socket_fd, char* ip, int port) {
    struct sockaddr_in s_addr;
    
    //Crear el socket
    *socket_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (*socket_fd < 0) {
        perror("socket creation");
        *socket_fd = -1;
        raise(SIGINT);
    }

    //Omplir l'adreça del servidor amb el port i ip indicats
    memset(&s_addr, 0, sizeof(s_addr));
    s_addr.sin_family = AF_INET;
    if (port < 1 || port > 65535) {
        perror("invalid port");
        raise(SIGINT);
    }
    s_addr.sin_port = htons(port);
    s_addr.sin_addr.s_addr = inet_addr(ip);


    //Connectar amb el servidor
    if (connect(*socket_fd, (void*)&s_addr, sizeof(s_addr)) < 0) {
        perror("socket connection");
        close(*socket_fd);
        *socket_fd = -1;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

char* TOOLS_read_until(int fd, char end) {
    int i = 0, size;
    char c = '\0';
    char* string = (char*)malloc(sizeof(char));

    while (1) {
        size = read(fd, &c, sizeof(char));

        if (c != end && size > 0) {
            string = (char*)realloc(string, sizeof(char) * (i + 2));
            string[i++] = c;
        } else {
            i++;  //Pel \0
            break;
        }
    }

    string[i - 1] = '\0';
    return string;
}

Direction TOOLS_findDirection(Direction * directions, int n_directions, int id_server) {
    int i = 0, size;
    char *buffer;
    Direction dir;
    dir.id_server = -1;

    for (i = 0; i < n_directions; i++) {
        if (directions[i].id_server == id_server)
            return directions[i];
    }

    size = asprintf(&buffer, BOLDRED "Unable to find this direction.\n" RESET);
    write(1, buffer, size);
    free(buffer);
    return dir;
}

int TOOLS_displayMenu() {
    int option;
    char *buffer;

    write(1, MENU, strlen(MENU));
    buffer = TOOLS_read_until(1, '\n');
    option = atoi(buffer);
    free(buffer);

    if (option < 1 || option > 2) {
        perror(INVALID_OPTION);
        return -1;
    }

    return option;
}

void TOOLS_printServerDirections(Server server) {
    char *buffer;
    int size, i;

    size = asprintf(&buffer, BOLDBLUE "\nTotal servers: %d\n" RESET, server.total_servers);
    write(1, buffer, size);
    free(buffer);

    for (i=0; i < server.total_servers; i++) {

        size = asprintf(&buffer, BOLDBLUE "Server %d, Ip: %s, Passive port: %d, Ping port: %d\n" RESET, 
                            server.servers_directions[i].id_server, 
                            server.servers_directions[i].ip_address, 
                            server.servers_directions[i].passive_port, 
                            server.servers_directions[i].ping_port);
        write(1, buffer, size);
        free(buffer);
    }

}

void TOOLS_copyNextServerDirection(int id_server, Direction * next, Server server){
    for (int i = 0; i < server.total_servers; i++) {
        if (server.servers_directions[i].id_server == id_server){
            next->id_server = id_server;
            next->passive_port = server.servers_directions[i].passive_port;
            next->ping_port = server.servers_directions[i].ping_port;
            next->active_port = server.servers_directions[i].active_port;
            next->ip_address = server.servers_directions[i].ip_address;
            break;
        }
    }
}

void TOOLS_operate(int* value, int* version, Operation operation){
    switch (operation.operator) {
        case '+':
            *value += operation.operand;
            break;
        case '-':
            *value -= operation.operand;
            break;
        case '*':
            *value *= operation.operand;
            break;
        case '/':
            if(operation.operand == 0)
                *value = 0;
            else
                *value /= operation.operand;
            break;
    }
    *version ++;
}