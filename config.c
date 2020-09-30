
#include "config.h"

Server readConfig(char* filename){
    Server me;
    int fdReadConfig = open(filename, O_RDONLY);
    if(fdReadConfig < 0){
        printf("[ERROR] - Unable to open the config file \n");
    }else{
        read(fdReadConfig, &(me.id_server), sizeof(int));
        read(fdReadConfig, NULL, sizeof(char)); // \n
        
    }
    /*
    id_node
    read-only [R] / not read-only [U]
    IP address
    active_port
    passive_port
    ping_port
    temps de sleep
    llista de id_node + ip/port de cada node existent
     */
}