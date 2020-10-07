
#include "config.h"

Server readConfig(char* filename){
    Server me;
    int fdReadConfig = open(filename, O_RDONLY);
    int end = 0;
    int i = 0;

    if(fdReadConfig < 0){
        printf("[ERROR] - Unable to open the config file \n");
    }else{
        read(fdReadConfig, &(me.my_direction.id_server), sizeof(int));
        read(fdReadConfig, NULL, sizeof(char)); // \n

        read(fdReadConfig, &(me.is_read_only), sizeof(char));
        read(fdReadConfig, NULL, sizeof(char)); // \n

        me.my_direction.ip_address = TOOLS_read_until(fdReadConfig, '\n');
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

        me.total_servers = 1; // Probably deleted
        i = 0;

        me.servers_directions = (Direction*)malloc(sizeof(Direction)); // Nomes per al primer servidor (al que es connecta inicialment)
        while (end == 0){
            me.servers_directions[0].ip_address = TOOLS_read_until(fdReadConfig, '\n');
            if (me.servers_directions[i].ip_address[0] == '\0'){
                me.servers_directions = realloc(me.servers_directions, sizeof(Direction) * i);
                end = 1;
                // No more
                printf("End of config file!\n");
            }else{
                read(fdReadConfig, NULL, sizeof(char)); // \n
                read(fdReadConfig, &(me.servers_directions->id_server), sizeof(int));
                read(fdReadConfig, NULL, sizeof(char)); // \n
                read(fdReadConfig, &(me.servers_directions->passive_port), sizeof(int));
                read(fdReadConfig, NULL, sizeof(char)); // \n
                read(fdReadConfig, &(me.servers_directions->ping_port), sizeof(int));
                i++;
                me.servers_directions = realloc(me.servers_directions, sizeof(Direction) * (i + 1));
            }
        }

        me.total_servers = i + 1;
    }
    return me;
}