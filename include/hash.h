/*
Date of creation : 17/10/2025
Description : All structures of hashing are delcared here
Group 2 ESGI 2A3
*/

#ifndef HASH_H
#define HASH_H

#define HASH_TABLE_SIZE 67 // prime number size for better key distribution

typedef struct Row Row;
typedef enum ColType ColType;

//linked list of nodes in a bucket
typedef struct Node{
    Row* row; //what row is store at this node
    Row* prev_row; // prev row for delete operation to update the linked list

    // the actual value before hashed/value of pk, we can access to it by accessing to row above but will be complicated, so store it here for fast access
    char* original_value; // actual value can be int/str but converted upon inserting, need to convert back to original type to cmp

    struct Node* next_node; 
} Node;

typedef struct HashTable{
    char* col_name; // name of pk col to hash
    // linked list of buckets, 67 buckets max -> bucket[66][linkedlist collision]
    Node* bucket[HASH_TABLE_SIZE]; // this is also the "first_node"

    struct HashTable* next_hash_table;
} HashTable;


unsigned int hash_int(int);
unsigned int hash_string(char*);
Node* exist_in_ht(HashTable* hash_tab, int condition_int, char* condition_str);
void add_to_ht(HashTable* hash_table, int key, char* value, Row* prev_row, Row* new_row);
void remove_from_ht(HashTable* ht, Row* row, int data_index, ColType col_type);

#endif
