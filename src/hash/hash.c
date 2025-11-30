/*
Date of creation : 18/10/2025
Description : All hash-related functions/operations are here
Group 2 ESGI 2A3
*/

#include <string.h>
#include <assert.h>
#include <stdlib.h>

#include "../../include/hash.h"
#include "../../helper/db/db.h"
#include "../../include/db.h"
#include "../../include/ini.h"
#include "../../include/clean.h"

unsigned int hash_string(char* string_to_hash){
    //DJB2 Algorithm
    unsigned int res = 5381; // prime seed (known to work well)
    unsigned char c;

    if (string_to_hash == NULL) {
        return 0; 
    }
    while ((c = *string_to_hash++)) res = ((res << 5) + res) + c; 
    return res % HASH_TABLE_SIZE;
}

unsigned int hash_int(int num_to_hash){
    unsigned int val = (unsigned int)num_to_hash;
    
    // some hash func i found on internet doing bit shift and bitwise XOR
    val = (val ^ 61) ^ (val >> 16);
    val = val + (val << 3);
    val = val ^ (val >> 4);
    
    return val % HASH_TABLE_SIZE;
}

void add_to_ht(HashTable* hash_table, int key, char* value, Row* prev_row, Row* new_row){
    // function to add a node to the indicated hash table, original value can be str/int, must be converted to str before passing in this function
    Node* new_node = NULL;

    new_node = init_node();
    new_node->original_value = value;
    new_node->row = new_row;
    new_node->prev_row = prev_row;

    // set pointer for node in the linked list that handles collisions
    if(!hash_table->bucket[key]){
        hash_table->bucket[key] = new_node;
    }else{
        // insert at head
        new_node->next_node = hash_table->bucket[key];
        hash_table->bucket[key] = new_node;
    }
}

void remove_from_ht(HashTable* ht, Row* row, int data_index, ColType col_type){
    // remove 1 node from hash table, used in DELETE

    int key;
    Node* prev_node = NULL;
    Node* current_node = NULL;
    char* val_to_cmp = NULL;
    bool should_free = false;

    if(col_type == INT){
        key = hash_int(row->int_list[data_index][0]);
        should_free = true;
        val_to_cmp = int_to_str(row->int_list[data_index][0]);
    }
    else{
        key = hash_string(row->str_list[data_index]);
        val_to_cmp = row->str_list[data_index];
    }

    // for sure will exist in hash table since insertion
    current_node=ht->bucket[key];
    while(current_node){
        if(strcmp(val_to_cmp,current_node->original_value)==0){
            // remove node from linked list in bucket
            if (prev_node) prev_node->next_node = current_node->next_node;
            else ht->bucket[key] = current_node->next_node;

            free_node(current_node);
            break;
        }
        prev_node = current_node;
        current_node = current_node->next_node;
    }
    if(should_free){
        free(val_to_cmp);
        val_to_cmp = NULL;
    }
}

Node* exist_in_ht(HashTable* hash_tab, int condition_int, char* condition_str){
    // hash lookup for WHERE clause, then return the matching hash node for later processing, NULL if not found 
    int key;
    Node* current_node = NULL;
    char* val_to_cmp = NULL;
    bool should_free = false;

    // convert and hash int/str condition value 
    if(!condition_str) {
        val_to_cmp = int_to_str(condition_int);
        key = hash_int(condition_int);
        should_free = true;
    } else {
        val_to_cmp = condition_str;
        key = hash_string(condition_str);
    }

    // no key found
    if(!hash_tab->bucket[key]) {
        if(should_free){
            free(val_to_cmp);
            val_to_cmp = NULL;
        }
        return NULL;
    }

    // loop the node list that handles collision
    for(current_node=hash_tab->bucket[key]; current_node!=NULL; current_node=current_node->next_node){     
        if(strcmp(val_to_cmp,current_node->original_value)==0){
            // found a match
            if(should_free){
                free(val_to_cmp);
                val_to_cmp = NULL;
            }
            return current_node;
        }
    }   
    // not in collision linked list (other values hashed into the same key as this value)
    if(should_free){
        free(val_to_cmp);
        val_to_cmp = NULL;
    }
    return NULL;
}
