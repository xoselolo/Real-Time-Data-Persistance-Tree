#include <stdio.h>
#include "binary_tree.h"

int main() {

    Node* root = NULL;

    TRANSACTION_BINARY_TREE_add(&root, 4);
    TRANSACTION_BINARY_TREE_add(&root, 6);
    TRANSACTION_BINARY_TREE_add(&root, 3);
    TRANSACTION_BINARY_TREE_add(&root, 2);
    TRANSACTION_BINARY_TREE_add(&root, 1);
    TRANSACTION_BINARY_TREE_add(&root, 10);
    TRANSACTION_BINARY_TREE_add(&root, 8);

    if(TRANSACTION_BINARY_TREE_exists(root, 3)){
        printf("Exists!\n");
    }else{
        printf("Not exists!\n");
    }

    TRANSACTION_BINARY_TREE_destroy(&root);

    if(TRANSACTION_BINARY_TREE_exists(root, 3)){
        printf("Exists!\n");
    }else{
        printf("Not exists!\n");
    }

    return 0;
}
