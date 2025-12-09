/*
Date of creation : 17/10/2025
Description : All structures of hashing are delcared here
Group 2 ESGI 2A3
*/

#ifndef HASH_H
#define HASH_H

#define HASH_TABLE_SIZE 67

typedef struct Row Row;
typedef enum ColType ColType;

typedef struct Node{
    Row* row;
    Row* prev_row;

    char* original_value;

    struct Node* next_node; 
} Node;

typedef struct HashTable{
    char* col_name;
    Node* bucket[HASH_TABLE_SIZE];

    struct HashTable* next_hash_table;
} HashTable;


unsigned int hash_int(int);
unsigned int hash_string(char*);
Node* exist_in_ht(HashTable* hash_tab, int condition_int, char* condition_str);
void add_to_ht(HashTable* hash_table, int key, char* value, Row* prev_row, Row* new_row);
void remove_from_ht(HashTable* ht, Row* row, int data_index, ColType col_type);

#endif
