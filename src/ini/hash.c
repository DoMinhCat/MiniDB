/*
Date of creation : 29/10/2025
Description : Clean initialisation of Hash table
Group 2 ESGI 2A3
*/

#include <stdlib.h>
#include <assert.h>

#include "../../include/hash.h"
#include "../../include/ini.h"

Node* init_node(){
    Node* node = NULL;
    assert((node = (Node*)malloc(sizeof(Node))) != NULL);

    node->row = NULL;
    node->prev_row = NULL;
    node->original_value = NULL;
    node->next_node = NULL;
    return node;
}

HashTable* init_hash_table(){
    HashTable* hash_tab = NULL;
    assert((hash_tab = (HashTable*)malloc(sizeof(HashTable))) != NULL);
    
    hash_tab->col_name = NULL;
    hash_tab->next_hash_table = NULL;

     
    for(int i=0; i<HASH_TABLE_SIZE; i++){
        hash_tab->bucket[i] = NULL;
    }
    return hash_tab;
}

