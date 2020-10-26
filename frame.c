#include "frame.h"

int FRAME_readReadRequest(int fd, int * id_server, int * id_trans) {
    if (read(fd, &id_server, sizeof(int)) != sizeof(int)) {
        return EXIT_FAILURE;
    }
    
    if (read(fd, &id_trans, sizeof(int)) != sizeof(int)) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
} 

int FRAME_readReadResponse(int fd, int * version, int * value) {
    if (read(fd, &version, sizeof(int)) != sizeof(int)) {
        return EXIT_FAILURE;
    }
    
    if (read(fd, &value, sizeof(int)) != sizeof(int)) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
} 

int FRAME_sendReadResponse(int fd, int version, int value) {
    if (write(fd, &version, sizeof(int)) != sizeof(int)) {
        return EXIT_FAILURE;
    }

    if (write(fd, &value, sizeof(int)) != sizeof(int)) {
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
} 

/**
 * TRAMA = [ R id_server # id_trans # ]
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
    n = write(fd, "#", sizeof(char));
    if(n <= 0){
        return -1;
    }
    return EXIT_SUCCESS;
}


int FRAME_sendReadAck(int fd) {
    if (write(fd, READ_STR, sizeof(char)) != sizeof(char)) {
        return EXIT_FAILURE;
    }
    if (write(fd, ACK, strlen(ACK)) != sizeof(char)) {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int FRAME_readReadAck(int fd) {
    char ack[strlen(ACK)];
    
    if (read(fd, ack, strlen(ACK)) != strlen(ACK)) {
        return EXIT_FAILURE;
    }
    if (strcmp(ACK, ack)) {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}