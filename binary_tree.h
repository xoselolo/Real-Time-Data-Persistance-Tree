

#ifndef TRANSACTIONS_BINARY_TREE_BINARY_TREE_H
#define TRANSACTIONS_BINARY_TREE_BINARY_TREE_H

#include "types.h"

/**
 * TRANSACTION_BINARY_TREE_exists
 *
 * Searches the element, if the node exploring is null it doesn't exist, if it's not the element we can:
 *  - Explore the smaller son if our element is smaller than the explored node
 *  - Explore the bigger son if our element is bigger than the explored node
 *
 * @param node : actual node being explored
 * @param id_transaction : value to search
 * @return 1 if exists, 0 if doesn't
 */
int TRANSACTION_BINARY_TREE_exists(Node* node, int id_transaction);

/**
 * TRANSACTION_BINARY_TREE_add
 *
 * Adds the element, if the node exploring is null we can create it with this value, if it's not the element we can:
 *  - Explore the smaller son if our element is smaller than the explored node
 *  - Explore the bigger son if our element is bigger than the explored node
 *
 * @param node : actual node being explored
 * @param id_transaction : value to search
 */
void TRANSACTION_BINARY_TREE_add(Node** node, int id_transaction, int id_server);

/**
 * TRANSACTION_BINARY_TREE_add
 *
 * If node has children, goes to destroy the sons, if it hasn't got, it destroys itself
 *
 * @param node : actual node being explored
 */
void TRANSACTION_BINARY_TREE_destroy(Node** node);

/**
 * ----------------- Example of use -----------------

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

 */

#endif //TRANSACTIONS_BINARY_TREE_BINARY_TREE_H
