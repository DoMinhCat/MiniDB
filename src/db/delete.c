/*
Date of creation : 08/11/2025 (updated 15/11/2025)
Description : ersion 2 for delete to reflect new Hash for unique and pk cols
Group 2 ESGI 2A3
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "../../include/db.h"
#include "../../helper/db/db.h"
#include "../../include/ini.h"
#include "../../include/hash.h"
#include "../../include/clean.h"
#include "../../include/global.h"

void delete_all(Table* table){
    Row* current_row = NULL;
    Row* tmp_row = NULL;
    HashTable* current_ht = NULL;
    Node* tmp_node = NULL;
    Node* current_node = NULL;
    int total_row = table->row_count;
    int i;

    if(!ref_integrity_check_delete(table, NULL, true)) return;

    // del all nodes from hash tables
    current_ht = table->first_hash_table;
        while(current_ht != NULL){
            for(i=0; i<HASH_TABLE_SIZE; i++){
            current_node = current_ht->bucket[i];
            while(current_node != NULL){
                tmp_node = current_node;
                // save pointer to next col, then free current pointer
                current_node =current_node->next_node;
                
                free_node(tmp_node);   
            }
            current_ht->bucket[i] = NULL;
        } 
        current_ht=current_ht->next_hash_table; 
    }

    // del all rows
    current_row=table->first_row;
    while(current_row!=NULL){
        tmp_row = current_row;
        current_row = current_row->next_row;
        free_row(tmp_row);
        tmp_row = NULL;  
    }
    table->first_row = NULL;

    // update table metadata
    table->row_count = 0;
    table->next_id = 1;
    printf("Executed: %d %s deleted from '%s' table.\n\n", total_row, total_row>1?"rows":"row", table->name);
}

FilteredRow* get_null_rows(Table* table, Col* condition_col, int condition_data_index, int* row_count){
    bool is_null;
    Row* current_row = NULL;
    FilteredRow* to_del_list = NULL;
    FilteredRow* new_fr = NULL;
    FilteredRow* last_fr = NULL;

    for(current_row = table->first_row; current_row != NULL; current_row = current_row->next_row){
        is_null = false;
        
        if(condition_col->type == INT) is_null = (current_row->int_list[condition_data_index] == NULL);
        else if(condition_col->type == STRING) is_null = (current_row->str_list[condition_data_index] == NULL); 
        else if(condition_col->type == DOUBLE) is_null = (current_row->double_list[condition_data_index] == NULL); 
        else {
            fprintf(stderr, "Execution error: an unexpected error occured.\n\n");
            return NULL;
        }
        
        if(is_null){
            new_fr = copy_data_lists_to_filtered(current_row, NULL);
            new_fr->row = current_row;
            
            // add to list
            if(to_del_list == NULL) {
                to_del_list = new_fr;
                last_fr = to_del_list;
            } else {
                last_fr->next_filtered_row = new_fr;
                last_fr = new_fr;
            }
            (*row_count)++;
        }
    }
    if(*row_count == 0) return NULL;
    return to_del_list;
}

void delete_where(Table* table, Col* condition_col, char* condition_val){
    HashTable* ht = NULL;
    Row* current_row = NULL;
    Row* prev_row = NULL;
    Col* current_col = NULL;
    FilteredRow* to_del_list = NULL;    
    FilteredRow* new_fr = NULL;    
    FilteredRow* last_fr = NULL;    
    FilteredRow* current_fr = NULL;
    bool should_add;
    
    int row_count = 0;
    int condition_data_index = get_data_list_index(table, condition_col->name);
    int unique_col_data_index;
    char* str_val = NULL;
    int int_val = 0;
    double double_val = 0;
    
    // Where clause on UNIQUE/PK cols: use hash table
    if(condition_col->constraint == PK || condition_col->constraint == UNIQUE){
        Node* hash_node_found = NULL;
        ht = get_ht_by_col_name(table->first_hash_table, condition_col->name);

        // compare with NULL
        if(strcasecmp(condition_val, "NULL") == 0){            
            if(condition_col->constraint == PK){
                // PK value is never NULL
                printf("Executed: 0 row deleted from '%s' table.\n\n", table->name);
                return;
            }
            
            // For UNIQUE columns, traverse to find NULL values and put into list for ref integrity check
            to_del_list = get_null_rows(table, condition_col, condition_data_index, &row_count);
            
            // If no rows found, exit early
            if(row_count == 0){
                printf("Executed: 0 row deleted from '%s' table.\n\n", table->name);
                return;
            }
            
        } else {
            // comparing with a value != NULL
            
            // convert condition val            
            if(condition_col->type == INT){
                if(!str_to_int(condition_val, &int_val, condition_col->name)) return;
            } else if(condition_col->type == STRING){
                str_val = condition_val;
            } else {
                fprintf(stderr, "Execution error: unexpected column type.\n\n");
                return;
            }

            // hash look up, get only 1 row to del (UNIQUE/PK constraint)
            hash_node_found = exist_in_ht(ht, int_val, str_val);
            
            // no row to del
            if(!hash_node_found){
                printf("Executed: 0 row deleted from '%s' table.\n\n", table->name);
                return;
            }
            
            // found row to del, copy to filteredRow for ref integrity check
            to_del_list = copy_data_lists_to_filtered(hash_node_found->row, NULL);
            to_del_list->row = hash_node_found->row;
            row_count = 1;
        }
    } else {
        // Non-UNIQUE/PK columns: traverse all rows
        if(strcasecmp(condition_val,"NULL") == 0){
            to_del_list = get_null_rows(table, condition_col, condition_data_index, &row_count);
            // no row found, return early
            if(row_count==0){    
                printf("Executed: 0 row deleted from table '%s'.\n\n", table->name);
                return;
            }
        }else{
            // convert condition val to compare          
            if(condition_col->type == INT){
                if(!str_to_int(condition_val, &int_val, condition_col->name)) return;
            } else if(condition_col->type == STRING){
                str_val = condition_val;
            }else if(condition_col->type == DOUBLE){
                if(!str_to_double(condition_val, &double_val, condition_col->name)) return;
            }             
            else {
                fprintf(stderr, "Execution error: unexpected column type.\n\n");
                return;
            }

            // traverse all rows to find matching values 
            for(current_row=table->first_row; current_row!=NULL; current_row=current_row->next_row){
                should_add = false;
                switch(condition_col->type){
                    case INT:
                        if(!current_row->int_list[condition_data_index]) continue;
                        if(current_row->int_list[condition_data_index][0] == int_val) should_add = true;
                        break;
                    case DOUBLE:
                        if(!current_row->double_list[condition_data_index]) continue;
                        if(compare_double(current_row->double_list[condition_data_index][0], double_val) == 0) should_add = true;
                        break;
                    case STRING:
                        if(!current_row->str_list[condition_data_index]) continue;
                        if(strcmp(current_row->str_list[condition_data_index], str_val) == 0) should_add = true;
                        break;
                    default:
                        fprintf(stderr, "Execution error: an unexpected error occured.\n\n");
                        return;
                        break;
                }

                if(should_add){
                    // add row to to_del_list
                    new_fr = copy_data_lists_to_filtered(current_row, NULL);
                    new_fr->row = current_row;
                    
                    // add to list
                    if(to_del_list == NULL) {
                        to_del_list = new_fr;
                        last_fr = to_del_list;
                    } else {
                        last_fr->next_filtered_row = new_fr;
                        last_fr = new_fr;
                    }
                    row_count++;
                }
            }
            // exit early if no row found
            if(row_count==0){    
                printf("Executed: 0 row deleted from table '%s'.\n\n", table->name);
                return;
            }
        }
    }

    // check referential integrity of rows to del
    if(!ref_integrity_check_delete(table, to_del_list, false)){
        free_filtered_set(to_del_list);
        return;
    } 

    // remove nodes from hash tables of unique/pk cols for rows to be deleted 
    for(current_col = table->first_col; current_col != NULL; current_col = current_col->next_col){
        if(current_col->constraint == UNIQUE || current_col->constraint == PK){
            ht = get_ht_by_col_name(table->first_hash_table, current_col->name);
            unique_col_data_index = get_data_list_index(table, current_col->name);

            for(current_fr = to_del_list; current_fr != NULL; current_fr = current_fr->next_filtered_row){
                remove_from_ht(ht, current_fr->row, unique_col_data_index, current_col->type);
            }
        }
    }

    // delete all result rows
    for(current_fr = to_del_list; current_fr != NULL; current_fr = current_fr->next_filtered_row){
        prev_row = get_prev_row(table, current_fr->row);
        if(!prev_row){
            // row to del is first row
            table->first_row = current_fr->row->next_row;
        } else {
            prev_row->next_row = current_fr->row->next_row;
        }
        free_row(current_fr->row);
    }

    table->row_count -= row_count;
    free_filtered_set(to_del_list);
    printf("Executed: %d %s deleted from '%s' table.\n\n", row_count, row_count > 1 ? "rows" : "row", table->name);
}

void delete_from_table(Query* query) {
    char* table_name = query->params.delete_params.table_name;
    char* condition_col_name = query->params.delete_params.condition_column;
    char* condition_val = query->params.delete_params.condition_value;
    Table* table = NULL;
    Col* condition_col = NULL;
    
    bool include_where = false;

    // check table exists
    table = get_table_by_name(table_name);
    if (!table) {
        fprintf(stderr, "Execution error: table '%s' not found.\n\n", table_name);
        return;
    }

    // check where
    if(condition_col_name){
        include_where = true;
        
        // check where col exists
        condition_col = get_col_by_name(table, condition_col_name);
        if(!condition_col){
            fprintf(stderr, "Execution error: '%s' column  not found.\n\n", condition_col_name);
            return;
        }
    }

    // no where case
    if(!include_where){
        delete_all(table);
        return;
    }
    // where case
    if(include_where){
        delete_where(table, condition_col, condition_val);
        return;
    }
}