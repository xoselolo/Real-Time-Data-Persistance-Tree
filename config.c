
#include "config.h"

Server readConfig(char* filename){
    Server me;
    int fdReadConfig = open(filename, O_RDONLY);
    if(fdReadConfig < 0){
        printf("[ERROR] - Unable to open the config file \n");
    }else{
        read(fdReadConfig, &(me.id_server), sizeof(int));
        read(fdReadConfig, NULL, sizeof(char)); // \n

        read(fdReadConfig, &(me.is_read_only), sizeof(char));
        read(fdReadConfig, NULL, sizeof(char)); // \n

        // todo: read line until the \n character

        read(fdReadConfig, &(me.my_direction.active_port), sizeof(int));
        read(fdReadConfig, NULL, sizeof(char)); // \n
        read(fdReadConfig, &(me.my_direction.passive_port), sizeof(int));
        read(fdReadConfig, NULL, sizeof(char)); // \n
        read(fdReadConfig, &(me.my_direction.ping_port), sizeof(int));
        read(fdReadConfig, NULL, sizeof(char)); // \n

        read(fdReadConfig, &(me.sleep_time), sizeof(int));
        read(fdReadConfig, NULL, sizeof(char)); // \n

        me.data.value = 0;
        me.data.version = 0;

        me.transaction_trees = (Node**)malloc(sizeof(Node*)); // Reservem un arbre nomes per al primer servidor on ens connectem

        read(fdReadConfig, &(me.operation.operator), sizeof(char));
        read(fdReadConfig, NULL, sizeof(char)); // \n
        read(fdReadConfig, &(me.operation.operand), sizeof(int));
        read(fdReadConfig, NULL, sizeof(char)); // \n

        me.total_servers = 1;

        me.servers_directions = (Direction*)malloc(sizeof(Direction)); // Nomes per al primer servidor (al que es connecta inicialment)

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