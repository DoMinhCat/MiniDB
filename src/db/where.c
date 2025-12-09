/*
Date of creation : 13/11/2025
Description : Code for WHERE clause with hash look up/traverse
Group 2 ESGI 2A3
*/

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "../../helper/db/db.h"
#include "../../include/ini.h"
#include "../../include/db.h"
#include "../../include/hash.h"

FilteredRow* hash_lookup(HashTable* hash_table, int condition_int, char* condition_str){
     

    FilteredRow* result = init_filtered_row();
     
    Node* hash_node = exist_in_ht(hash_table, condition_int, condition_str);

    if(hash_node){
        result->row = hash_node->row;
        return result;
    }
    return NULL;
}

FilteredRow* traverse_and_compare(Table* table, Col* condition_col, char* str_condition, double double_condition, int int_condition, ColType col_type){
     

    if(!table->first_row) return NULL;
    
    FilteredRow* first_filtered_row = NULL;
    FilteredRow* new_filtered_row = NULL;
    FilteredRow* last_filtered_row = NULL;
    Row* current_row = NULL;
    int data_list_index;
    bool should_add;

    for(current_row = table->first_row; current_row!=NULL; current_row=current_row->next_row){
        data_list_index = get_data_list_index(table, condition_col->name);
        should_add = false;
        bool check_with_null = strcasecmp("NULL", str_condition) == 0;

        switch (col_type)
        {
        case INT:
            if ((check_with_null && !current_row->int_list[data_list_index]) ||
            (!check_with_null && current_row->int_list[data_list_index] && current_row->int_list[data_list_index][0] == int_condition)) should_add = true; 
            break;
        case STRING:
            if ((check_with_null && !current_row->str_list[data_list_index]) ||
            (!check_with_null && current_row->str_list[data_list_index] && strcmp(current_row->str_list[data_list_index],str_condition)==0)) should_add = true; 
            break;
        case DOUBLE:
            if ((check_with_null && !current_row->double_list[data_list_index]) ||
            (!check_with_null && current_row->double_list[data_list_index] && current_row->double_list[data_list_index][0] == double_condition)) should_add = true; 
            break;
        
        default:  
            return NULL;
            break;
        }

         
        if(should_add){
            new_filtered_row = init_filtered_row();
            new_filtered_row->row = current_row;

            if(first_filtered_row == NULL) {
                first_filtered_row = new_filtered_row;
                last_filtered_row = first_filtered_row;
            } else {
                 
                last_filtered_row->next_filtered_row = new_filtered_row;
                last_filtered_row = new_filtered_row;
            }
        }
    }
    return first_filtered_row;
}

FilteredRow* where_for_select(Table* table, Col* condition_col, char* str_condition, double double_condition, int int_condition, ColType col_type){
     
     
     

    FilteredRow* res = NULL;
    HashTable* ht_of_col = NULL;
    char* col_name = condition_col->name;
    bool check_with_null = strcasecmp("NULL", str_condition) == 0;

     
    ht_of_col = get_ht_by_col_name(table->first_hash_table, col_name);

    if(ht_of_col && !check_with_null){
        res = hash_lookup(ht_of_col, int_condition, str_condition);        
    }else{
        res = traverse_and_compare(table, condition_col, str_condition, double_condition, int_condition, col_type);
    }

    return res;
}