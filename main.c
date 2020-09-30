#include <stdio.h>
#include "config.h"

int main(int argc, char** argv) {

    Server me;

    me = readConfig(argv[1]);

    printf("Server id: %d\n", me.id_server);

    return 0;
}
