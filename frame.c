#include "frame.h"

int FRAME_readReadRequest(int fd, int * id_server, int * id_trans) {
    if (read(fd, id_server, sizeof(int)) != sizeof(int)) {
        return EXIT_FAILURE;
    }
    
    if (read(fd, id_trans, sizeof(int)) != sizeof(int)) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
} 

int FRAME_readReadResponse(int fd, int * version, int * value) {
    if (read(fd, version, sizeof(int)) != sizeof(int)) {
        return EXIT_FAILURE;
    }
    
    if (read(fd, value, sizeof(int)) != sizeof(int)) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
} 

int FRAME_sendReadResponse(int fd, int version, int value) {
    write(1, "Sending...\n", strlen("Sending...\n") * sizeof(char));

    if (write(fd, &version, sizeof(int)) != sizeof(int)) {
        return EXIT_FAILURE;
    }

    if (write(fd, &value, sizeof(int)) != sizeof(int)) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
} 

/**
 * TRAMA = [ R id_server id_trans ]
 * @param fd
 * @param id_server
 * @param id_trans
 * @return
 */
int FRAME_sendReadRequest(int fd, int id_server, int id_trans) {
    int n;

    n = write(fd, "R", sizeof(char));
    if(n <= 0){
        return -1;
    }
    n = write(fd, &id_server, sizeof(int));
    if(n <= 0){
        return -1;
    }
    n = write(fd, &id_trans, sizeof(int));
    if(n <= 0){
        return -1;
    }

    return EXIT_SUCCESS;
}


int FRAME_sendReadAck(int fd) {
    if (write(fd, ACK_STR, sizeof(char)) != sizeof(char)) {
        return EXIT_FAILURE;
    }
    if (write(fd, ACK_STR, strlen(ACK_STR)) != sizeof(char)) {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int FRAME_readReadAck(int fd) {
    char ack[strlen(ACK_STR)];
    
    if (read(fd, ack, strlen(ACK_STR)) != strlen(ACK_STR)) {
        return EXIT_FAILURE;
    }
    if (strcmp(ACK_STR, ack)) {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int FRAME_sendUpdateRequest(int active_fd, int id_server, int id_transaction, Operation operation);int FRAME_sendUpdateRequest(int active_fd, int id_server, int id_transaction, Operation operation){
    int n;

    n = write(active_fd, "U", sizeof(char));
    if(n <= 0){
        return -1;
    }
    n = write(active_fd, &id_server, sizeof(int));
    if(n <= 0){
        return -1;
    }
    n = write(active_fd, &id_transaction, sizeof(int));
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

int FRAME_readUpdateRequest(int fd, int * id_server, int * id_trans, Operation* operation) {
    if (read(fd, id_server, sizeof(int)) != sizeof(int)) {
        return EXIT_FAILURE;
    }

    if (read(fd, id_trans, sizeof(int)) != sizeof(int)) {
        return EXIT_FAILURE;
    }

    if (read(fd, &(operation->operator), sizeof(char )) != sizeof(char)) {
        return EXIT_FAILURE;
    }

    if (read(fd, &(operation->operand), sizeof(int)) != sizeof(int)) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int FRAME_sendUpdateResponse(int fd, int version, int value) {
    write(1, "Sending...\n", strlen("Sending...\n") * sizeof(char));

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