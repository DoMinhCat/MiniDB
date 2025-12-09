/*
Date of creation : 31/10/2025
Description : Free functions for Row, Col and Table pointers to call before exit
Group 2 ESGI 2A3
*/

#include <stdlib.h>

#include "../../include/clean.h"
#include "../../include/db.h"
#include "../../include/hash.h"

void free_col(Col* col){
    free(col->name);
    col->name = NULL;
    
    free(col->refer_table);
    col->refer_table = NULL;

    free(col->refer_col);
    col->refer_col = NULL;
    
    free(col);
    col = NULL;
}

void free_row(Row* row){
    if (!row) return;
    int i;
    if (row->int_list) {
        for(i=0; i<row->int_count; i++){
            free(row->int_list[i]);
            row->int_list[i] = NULL;
        }
        free(row->int_list);
        row->int_list = NULL;
    }
    if (row->str_list) {
        for(i=0; i<row->str_count; i++){
            free(row->str_list[i]);
            row->str_list[i] = NULL;
        }
        free(row->str_list);
        row->str_list = NULL;
    }
    if (row->double_list) {
        for(i=0; i<row->double_count; i++){
            free(row->double_list[i]);
            row->double_list[i] = NULL;
        }
        free(row->double_list);
        row->double_list = NULL;
    }

    free(row);
    row = NULL;
}

void free_node(Node* node){
    free(node->original_value);
    node->original_value = NULL;
    free(node);
    node = NULL;
}

void free_hash_table(HashTable* hash_table){
    if (!hash_table) return; 

    int i;
    Node* current_node = NULL;
    Node* tmp_node = NULL;

    free(hash_table->col_name);
    hash_table->col_name = NULL;

    for(i=0; i<HASH_TABLE_SIZE; i++){
        current_node = hash_table->bucket[i];
        while(current_node != NULL){
            tmp_node = current_node;
            current_node =current_node->next_node;
            
            free_node(tmp_node);   
        }
        hash_table->bucket[i] = NULL;
    }

    free(hash_table);
    hash_table = NULL;
}

void free_table(Table* table){
    Col* current_col = table->first_col;
    Col* tmp_col;
    Row* current_row = table->first_row;
    Row* tmp_row;
    HashTable* current_hash_table = table->first_hash_table;
    HashTable* tmp_hash_table;
    

    free(table->name);
    table->name = NULL;

    while(current_col != NULL){
        tmp_col = current_col;
        current_col = current_col->next_col;
        
        free_col(tmp_col);   
        tmp_col = NULL;
    }

    while(current_row != NULL){
        tmp_row = current_row;
        current_row = current_row->next_row;
        
        free_row(tmp_row); 
        tmp_row = NULL;  
    }

    while(current_hash_table != NULL){
        tmp_hash_table = current_hash_table;
        current_hash_table = current_hash_table->next_hash_table;
        
        free_hash_table(tmp_hash_table);   
    }

    free(table);
    table = NULL;
} 

void free_db(Table* first_table){
    Table* current_table = first_table;
    Table* tmp_table = NULL;

    while(current_table!=NULL){
        tmp_table = current_table;
        current_table = current_table->next_table;

        free_table(tmp_table);
        tmp_table = NULL;
    }
}

void free_filtered_row(FilteredRow* filt_row){
    int i;
    
    if(filt_row->int_joined_list){
        for(i=0; i<filt_row->int_join_count;i++){
            free(filt_row->int_joined_list[i]);
            filt_row->int_joined_list[i] = NULL;
        }
        free(filt_row->int_joined_list);
        filt_row->int_joined_list = NULL;
    }

    if(filt_row->str_joined_list){
        for(i=0; i<filt_row->str_join_count;i++){
            free(filt_row->str_joined_list[i]);
            filt_row->str_joined_list[i] = NULL;
        }
        free(filt_row->str_joined_list);
        filt_row->str_joined_list = NULL;
    }

    if(filt_row->double_joined_list){
        for(i=0; i<filt_row->double_join_count;i++){
            free(filt_row->double_joined_list[i]);
            filt_row->double_joined_list[i] = NULL;
        }
        free(filt_row->double_joined_list);
        filt_row->double_joined_list = NULL;
    }

    free(filt_row);
    filt_row = NULL;
}

void free_filtered_set(FilteredRow* first_filtered_row){
    FilteredRow* current = first_filtered_row;
    FilteredRow* tmp = NULL;

    while(current!=NULL){
        tmp = current;
        current = current->next_filtered_row;

        free_filtered_row(tmp);
        tmp = NULL;
    }
}