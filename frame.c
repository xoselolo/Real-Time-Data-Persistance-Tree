#include "frame.h"

int FRAME_sendFirstConnectionRequest(int fd, int id_server, char * ip_addr, int passive_port, int ping_port) {
    if (write(fd, "C", sizeof(char)) != sizeof(char)) return EXIT_FAILURE;
    if (write(fd, "F", sizeof(char)) != sizeof(char)) return EXIT_FAILURE;
    if (write(fd, &id_server, sizeof(int)) != sizeof(int)) return EXIT_FAILURE;
    if (write(fd, ip_addr, strlen(ip_addr)) != (int)strlen(ip_addr)) return EXIT_FAILURE;
    if (write(fd, "|", sizeof(char)) != sizeof(char)) return EXIT_FAILURE;
    if (write(fd, &passive_port, sizeof(int)) != sizeof(int)) return EXIT_FAILURE;
    if (write(fd, &ping_port, sizeof(int)) != sizeof(int)) return EXIT_FAILURE;
    return EXIT_SUCCESS;
}

int FRAME_readConnectionRequest(int fd, int *id_server, char **ip_addr, int *passive_port, int *ping_port) {
    
    if (read(fd, id_server, sizeof(int)) != sizeof(int)) return EXIT_FAILURE;
    *ip_addr = TOOLS_read_until(fd, '|');
    if ((*ip_addr)[0] == 0) return EXIT_FAILURE;
    if (read(fd, passive_port, sizeof(int)) != sizeof(int)) return EXIT_FAILURE;
    if (read(fd, ping_port, sizeof(int)) != sizeof(int)) return EXIT_FAILURE;

    return EXIT_SUCCESS;
}

int FRAME_sendConnectionRequest(int fd, int id_server, char * ip_addr, int passive_port, int ping_port) {
    if (write(fd, "C", sizeof(char)) != sizeof(char)) return EXIT_FAILURE;
    if (write(fd, "N", sizeof(char)) != sizeof(char)) return EXIT_FAILURE;
    if (write(fd, &id_server, sizeof(int)) != sizeof(int)) return EXIT_FAILURE;
    if (write(fd, ip_addr, strlen(ip_addr)) != (int)strlen(ip_addr)) return EXIT_FAILURE;
    if (write(fd, "|", sizeof(char)) != sizeof(char)) return EXIT_FAILURE;
    if (write(fd, &passive_port, sizeof(int)) != sizeof(int)) return EXIT_FAILURE;
    if (write(fd, &ping_port, sizeof(int)) != sizeof(int)) return EXIT_FAILURE;
    return EXIT_SUCCESS;
}

int FRAME_sendFirstConnectionResponse(int fd, Server server) {
    int i = 0;
    int total_servers = server.total_servers - 1; //The last one is the one that we are answering

    if (write(fd, "C", sizeof(char)) != sizeof(char)) return EXIT_FAILURE;
    if (write(fd, &(server.my_direction.id_server), sizeof(int)) != sizeof(int)) return EXIT_FAILURE;
    if (write(fd, &(server.data.version), sizeof(int)) != sizeof(int)) return EXIT_FAILURE;
    if (write(fd, &(server.data.value), sizeof(int)) != sizeof(int)) return EXIT_FAILURE;
    if (write(fd, &total_servers, sizeof(int)) != sizeof(int)) return EXIT_FAILURE;
    
    for (i=0; i<total_servers; i++) {
        if (write(fd, &server.servers_directions[i].id_server, sizeof(int)) != sizeof(int)) return EXIT_FAILURE;
        if (write(fd, server.servers_directions[i].ip_address, strlen(server.servers_directions[i].ip_address)) != (int) strlen(server.servers_directions[i].ip_address)) return EXIT_FAILURE;
        if (write(fd, "|", sizeof(char)) != EXIT_FAILURE) return EXIT_FAILURE;
        if (write(fd, &(server.servers_directions[i].passive_port), sizeof(int)) != sizeof(int)) return EXIT_FAILURE;
        if (write(fd, &(server.servers_directions[i].ping_port), sizeof(int)) != sizeof(int)) return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int FRAME_readFirstConnectionResponse(int fd, Server *server) {
    int i = 0, size;
    char *buffer;
    char type;

    if (read(fd, &type, sizeof(char)) != sizeof(char)) return EXIT_FAILURE;
    if (type != CONNECT) return EXIT_FAILURE;
    if (read(fd, &(server->next_server_direction.id_server), sizeof(int)) != sizeof(int)) return EXIT_FAILURE;
    if (read(fd, &(server->data.version), sizeof(int)) != sizeof(int)) return EXIT_FAILURE;
    if (read(fd, &(server->data.value), sizeof(int)) != sizeof(int)) return EXIT_FAILURE;
    if (read(fd, &(server->total_servers), sizeof(int)) != sizeof(int)) return EXIT_FAILURE;
    
    //For first server
    (server->total_servers)++; 
    server->servers_directions[0].id_server = server->next_server_direction.id_server;

    server->servers_directions = (Direction *)realloc(server->servers_directions, sizeof(Direction)*(server->total_servers + 1));

    for (i=1; i<server->total_servers; i++) {
        if (read(fd, &server->servers_directions[i].id_server, sizeof(int)) != sizeof(int)) return EXIT_FAILURE;
        server->servers_directions[i].ip_address = TOOLS_read_until(fd, '|');
        if (server->servers_directions[i].ip_address[0] == 0) return EXIT_FAILURE;
        if (read(fd, &server->servers_directions[i].passive_port, sizeof(int)) != sizeof(int)) return EXIT_FAILURE;
        if (read(fd, &server->servers_directions[i].ping_port, sizeof(int)) != sizeof(int)) return EXIT_FAILURE;
    }
    
    size = asprintf(&buffer, BOLD "%d servidors rebuts del node inicial %d\n" RESET, server->total_servers - 1, server->next_server_direction.id_server);
    write(1, buffer, size);
    free(buffer);

    return EXIT_SUCCESS;
}

int FRAME_sendConnectionResponse(int fd, Server server) {
    if (write(fd, "C", sizeof(char)) != sizeof(char)) return EXIT_FAILURE;
    if (write(fd, &(server.next_server_direction.id_server), sizeof(int)) != sizeof(int)) return EXIT_FAILURE;
    if (write(fd, &(server.data.version), sizeof(int)) != sizeof(int)) return EXIT_FAILURE;
    if (write(fd, &(server.data.value), sizeof(int)) != sizeof(int)) return EXIT_FAILURE;

    return EXIT_SUCCESS;
}


int FRAME_readConnectionResponse(int fd, int *id_server, int *version, int *value) {
    char type;
    if (read(fd, &type, sizeof(char)) != sizeof(char)) return EXIT_FAILURE;
    if (type != CONNECT) return EXIT_FAILURE;
    if (read(fd, id_server, sizeof(int)) != sizeof(int)) return EXIT_FAILURE;
    if (read(fd, version, sizeof(int)) != sizeof(int)) return EXIT_FAILURE;
    if (read(fd, value, sizeof(int)) != sizeof(int)) return EXIT_FAILURE;
    return EXIT_SUCCESS;
}
/*
    READ PROTOCOL
*/
int FRAME_readReadRequest(int fd, int * id_server) {
    if (read(fd, id_server, sizeof(int)) != sizeof(int)) return EXIT_FAILURE;
    return EXIT_SUCCESS;
} 

int FRAME_readReadResponse(int fd, int * version, int * value) {
    char type;
    if (read(fd, &type, sizeof(char)) != sizeof(char)) return EXIT_FAILURE;
    if (type != READ) return EXIT_FAILURE;
    if (read(fd, version, sizeof(int)) != sizeof(int)) return EXIT_FAILURE;
    if (read(fd, value, sizeof(int)) != sizeof(int)) return EXIT_FAILURE;

    return EXIT_SUCCESS;
} 

int FRAME_readOriginReadResponse(int fd, int * version, int * value) {
    if (read(fd, version, sizeof(int)) != sizeof(int)) return EXIT_FAILURE;
    if (read(fd, value, sizeof(int)) != sizeof(int)) return EXIT_FAILURE;

    return EXIT_SUCCESS;
}

int FRAME_readOriginUpdateResponse(int fd, int * version, int * value) {
    if (read(fd, version, sizeof(int)) != sizeof(int)) return EXIT_FAILURE;
    if (read(fd, value, sizeof(int)) != sizeof(int)) return EXIT_FAILURE;

    return EXIT_SUCCESS;
}

int FRAME_sendOriginReadResponse(int fd, int version, int value) {
    if (write(fd, READ_RESPONSE_STR, sizeof(char)) != sizeof(char)) return EXIT_FAILURE;
    if (write(fd, &version, sizeof(int)) != sizeof(int)) return EXIT_FAILURE;
    if (write(fd, &value, sizeof(int)) != sizeof(int))  return EXIT_FAILURE;
    return EXIT_SUCCESS;
}

int FRAME_sendOriginUpdateResponse(int fd, int version, int value) {
    if (write(fd, UPDATE_RESPONSE_STR, sizeof(char)) != sizeof(char)) return EXIT_FAILURE;
    if (write(fd, &version, sizeof(int)) != sizeof(int)) return EXIT_FAILURE;
    if (write(fd, &value, sizeof(int)) != sizeof(int))  return EXIT_FAILURE;
    return EXIT_SUCCESS;
}

int FRAME_sendReadResponse(int fd, int version, int value) {
    if (write(fd, READ_STR, sizeof(char)) != sizeof(char)) return EXIT_FAILURE;
    if (write(fd, &version, sizeof(int)) != sizeof(int)) return EXIT_FAILURE;
    if (write(fd, &value, sizeof(int)) != sizeof(int))  return EXIT_FAILURE;
    return EXIT_SUCCESS;
} 

/**
 * TRAMA = [ R id_server ]
 * @param fd
 * @param id_server
 * @return
 */
int FRAME_sendReadRequest(int fd, int id_server) {
    if (write(fd, READ_STR, sizeof(char)) != sizeof(char)) return EXIT_FAILURE;
    if (write(fd, &id_server, sizeof(int)) != sizeof(int)) return EXIT_FAILURE;
    return EXIT_SUCCESS;
}


int FRAME_sendAck(int fd) {
    if (write(fd, ACK_STR, strlen(ACK_STR)) != sizeof(char)) {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int FRAME_readAck(int fd) {
    char ack;
    
    if (read(fd, &ack, sizeof(char)) != sizeof(char)) return EXIT_FAILURE;
    if (ack != ACK) return EXIT_FAILURE;
    return EXIT_SUCCESS;
}

int FRAME_sendUpdateRequest(int active_fd, int id_server, Operation operation){
    int n;

    n = write(active_fd, "U", sizeof(char));
    if(n <= 0){
        return -1;
    }
    n = write(active_fd, &id_server, sizeof(int));
    if(n <= 0){
        return -1;
    }
    n = write(active_fd, &(operation.operator), sizeof(char));
    if(n <= 0){
        return -1;
    }
    n = write(active_fd, &(operation.operand), sizeof(int));
    if(n <= 0){
        return -1;
    }

    return EXIT_SUCCESS;
}

int FRAME_readUpdateRequest(int fd, int * id_server, Operation* operation) {
    if (read(fd, id_server, sizeof(int)) != sizeof(int)) return EXIT_FAILURE;
    if (read(fd, &(operation->operator), sizeof(char )) != sizeof(char)) return EXIT_FAILURE;
    if (read(fd, &(operation->operand), sizeof(int)) != sizeof(int)) return EXIT_FAILURE;

    return EXIT_SUCCESS;
}

int FRAME_sendUpdateResponse(int fd, int version, int value) {
    if (write(fd, &version, sizeof(int)) != sizeof(int)) {
        return EXIT_FAILURE;
    }

    if (write(fd, &value, sizeof(int)) != sizeof(int)) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int FRAME_readUpdateResponse(int fd, int * version, int * value) {
    if (read(fd, version, sizeof(int)) != sizeof(int)) {
        return EXIT_FAILURE;
    }

    if (read(fd, value, sizeof(int)) != sizeof(int)) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int FRAME_sendPingRequest(int fd) {
    if (write(fd, PING_STR, sizeof(char)) != sizeof(char)) return EXIT_FAILURE;
    return EXIT_SUCCESS;
}

int FRAME_readPingRequest(int fd) {
    char type;
    if (read(fd, &type, sizeof(char)) != sizeof(char)) return EXIT_FAILURE;
    if (type != PING) return EXIT_FAILURE;
    return EXIT_SUCCESS;
}

int FRAME_sendPingResponse(int fd, int version, int value, int isFirst) {
    if (write(fd, PING_STR, sizeof(char)) != sizeof(char)) return EXIT_FAILURE;
    if (write(fd, &version, sizeof(int)) != sizeof(int)) return EXIT_FAILURE;
    if (write(fd, &value, sizeof(int)) != sizeof(int)) return EXIT_FAILURE;
    if (write(fd, &isFirst, sizeof(int)) != sizeof(int)) return EXIT_FAILURE;
    return EXIT_SUCCESS;
}

int FRAME_readPingResponse(int fd, int *version, int *value, int *isFirst) {
    char type;
    if (read(fd, &type, sizeof(char)) != sizeof(char)) return EXIT_FAILURE;
    if (type != PING) return EXIT_FAILURE;
    if (read(fd, version, sizeof(int)) != sizeof(int)) return EXIT_FAILURE;
    if (read(fd, value, sizeof(int)) != sizeof(int)) return EXIT_FAILURE;
    if (read(fd, isFirst, sizeof(int)) != sizeof(int)) return EXIT_FAILURE;
    return EXIT_SUCCESS;
}