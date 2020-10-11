
#include "transaction.h"
#include <time.h>
#include <stdlib.h>

char initialized = 0;

/**
 * Generates a random id between 0 and 32,767
 * Limit of our system, every server can only make 32,767 transactions
 */
int TRANSACTION_generateId(Node* root){
    if(initialized == 0){
        srand(time(NULL));
    }
    int transaction_id = rand();
    while (TRANSACTION_BINARY_TREE_exists(root, transaction_id)){
        transaction_id = rand();
    }
    return transaction_id;
}