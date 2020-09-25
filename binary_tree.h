//
// Created by xoselolo on 24/09/2020.
//

#ifndef TRANSACTIONS_BINARY_TREE_BINARY_TREE_H
#define TRANSACTIONS_BINARY_TREE_BINARY_TREE_H

#include <stdlib.h>

typedef struct Node{
    int id_transaction;
    struct Node* smaller;
    struct Node* bigger;
}Node;

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
void TRANSACTION_BINARY_TREE_add(Node** node, int id_transaction);

/**
 * TRANSACTION_BINARY_TREE_add
 *
 * If node has children, goes to destroy the sons, if it hasn't got, it destroys itself
 *
 * @param node : actual node being explored
 */
void TRANSACTION_BINARY_TREE_destroy(Node** node);

#endif //TRANSACTIONS_BINARY_TREE_BINARY_TREE_H
