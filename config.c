
#include "config.h"

Server readConfig(char* filename){
    Server me;
    int fdReadConfig = open(filename, O_RDONLY);
    char* aux;
    char salt;

    if(fdReadConfig < 0){
        printf("[ERROR] - Unable to open the config file \n");
    }else{
        aux = TOOLS_read_until(fdReadConfig, '\n');
        me.my_direction.id_server = atoi(aux);
        printf("ID SERVER: %d\n", me.my_direction.id_server);

        read(fdReadConfig, &(me.is_read_only), sizeof(char));
        read(fdReadConfig, &salt, sizeof(char)); // \n
        printf("\tRead only: %c\n", me.is_read_only);

        if(me.is_read_only == 'U'){
            read(fdReadConfig, &(me.operation.operator), sizeof(char));
            read(fdReadConfig, &salt, sizeof(char)); // \n
            printf("\tOperator: %c\n", me.operation.operator);

            aux = TOOLS_read_until(fdReadConfig, '\n');
            me.operation.operand = atoi(aux);
            free(aux);
            printf("\tOperand: %d\n", me.operation.operand);
        }

        me.my_direction.ip_address = TOOLS_read_until(fdReadConfig, '\n');
        printf("\tIp address: %s\n", me.my_direction.ip_address);

        aux = TOOLS_read_until(fdReadConfig, '\n');
        me.my_direction.active_port = atoi(aux);
        free(aux);
        printf("\tActive Port: %d\n", me.my_direction.active_port);

        aux = TOOLS_read_until(fdReadConfig, '\n');
        me.my_direction.passive_port = atoi(aux);
        free(aux);
        printf("\tPassive Port: %d\n", me.my_direction.passive_port);

        aux = TOOLS_read_until(fdReadConfig, '\n');
        me.my_direction.ping_port = atoi(aux);
        free(aux);
        printf("\tPing Port: %d\n", me.my_direction.ping_port);

        aux = TOOLS_read_until(fdReadConfig, '\n');
        me.sleep_time = atoi(aux);
        free(aux);
        printf("\tSleep Time: %d\n", me.sleep_time);

        me.data.value = 0;
        me.data.version = 0;

        me.transaction_trees = (Node**)malloc(sizeof(Node*)); // Reservem un arbre nomes per al nostre servidor
        // nostre server SEMPRE [0]
        me.transaction_trees[0] = (Node*)malloc(sizeof(Node));
        me.transaction_trees[0]->id_server = me.my_direction.id_server;
        me.transaction_trees[0]->id_transaction = -1;
        me.transaction_trees[0]->bigger = NULL;
        me.transaction_trees[0]->smaller = NULL;
        // create another tree for every connection

        me.total_servers = 0; // Probably updated when connection

        me.servers_directions = (Direction*)malloc(sizeof(Direction)); // Nomes per al primer servidor (al que es connecta inicialment)

        me.servers_directions[0].ip_address = TOOLS_read_until(fdReadConfig, '\n');
        if(me.servers_directions[0].ip_address[0] == '\0'){
            // there is no server to connect to (we're the first)
            free(me.servers_directions);
            me.next_server_direction.id_server = -1;
            me.is_first = 1;

        }else{
            me.is_first = 0;
            // there is a server to connect to
            printf("\t\tIp address: %s\n", me.servers_directions[0].ip_address);

            aux = TOOLS_read_until(fdReadConfig, '\n');
            printf("\t\tId server: %d\n", atoi(aux));
            me.servers_directions[0].id_server = atoi(aux);

            aux = TOOLS_read_until(fdReadConfig, '\n');
            printf("\t\tPassive port: %d\n", atoi(aux));
            me.servers_directions[0].passive_port = atoi(aux);

            aux = TOOLS_read_until(fdReadConfig, '\n');
            printf("\t\tPing port: %d\n", atoi(aux));
            me.servers_directions[0].ping_port = atoi(aux);

            me.next_server_direction.id_server = me.servers_directions[0].id_server;
            me.next_server_direction.passive_port = me.servers_directions[0].passive_port;
            me.next_server_direction.ping_port = me.servers_directions[0].ping_port;

            me.next_server_direction.ip_address = (char *)malloc(sizeof(char)*strlen(me.servers_directions[0].ip_address)+1);
            me.next_server_direction.ip_address[0] = 0;
            strcat(me.next_server_direction.ip_address, me.servers_directions[0].ip_address);

            me.total_servers++;

            me.transaction_trees = (Node**)realloc(me.transaction_trees, sizeof(Node*) * 2);
            me.transaction_trees[1] = (Node*)malloc(sizeof(Node));
            me.transaction_trees[1]->id_server = me.next_server_direction.id_server;
            me.transaction_trees[1]->id_transaction = -1;
            me.transaction_trees[1]->bigger = NULL;
            me.transaction_trees[1]->smaller = NULL;

        }

    }

    TOOLS_printServerDirections(me);
    printf("\n\n\n");

    return me;
}