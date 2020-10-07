
#include "transaction.h"
#include <time.h>
#include <stdlib.h>

/**
 * Generates a random id between 0 and 32767
 * Limit of our system, every server can only make 32767 transactions
 */
int TRANSACTION_generateId(){
    if(initialized == 0){
        srand(time(NULL));
    }
    return rand();
}