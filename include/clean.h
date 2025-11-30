/*
Date of creation : 26/10/2025
Description : Prototypes for clean.c
Group 2 ESGI 2A3
*/

#ifndef CLEAN_H
#define CLEAN_H

#include "db.h" 
#include "hash.h"

void free_current_cmd(char** cmd_string, Query** query);
void free_query(Query** query);

void free_col(Col* col);
void free_row(Row* row);
void free_table(Table* table);
void free_db(Table* first_table);
void free_hash_table(HashTable* hash_table);
void free_node(Node* node);
void free_filtered_row(FilteredRow* filt_row);
void free_filtered_set(FilteredRow* first_filtered_row);

#endif