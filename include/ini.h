/*
Date of creation : 26/10/2025
Description : prototypes for all init functions
Group 2 ESGI 2A3
*/

#ifndef INI_H
#define INI_H

#include "parser.h"
#include "hash.h"
#include "db.h"

Query* init_query();

Node* init_node();
HashTable* init_hash_table();

Table* init_table();
Col* init_col();
Row* init_row();
FilteredRow* init_filtered_row();

#endif
