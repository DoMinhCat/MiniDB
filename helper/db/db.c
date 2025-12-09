/*
Date of creation : 29/10/2025
Description : Helper functions for db core functions
Group 2 ESGI 2A3
*/

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <assert.h>
#include <float.h>
#include <math.h>

#include "../../include/parser.h"
#include "../../include/db.h"
#include "../../helper/db/db.h"
#include "../../include/parser.h"
#include "../../include/ini.h"
#include "../../include/hash.h"
#include "../../include/global.h"
#include "../../include/clean.h"

bool table_exists(char* table_name){
    Table* table = get_table_by_name(table_name);
    if(!table) {
        fprintf(stderr, "Execution error: '%s' table not found.\n\n", table_name);
        return false;
    }
    return true;
}

bool col_exists(Table* table, char* col_name){
    Col* col = get_col_by_name(table, col_name);
    if(!col) {
        fprintf(stderr, "Execution error: '%s' column  not found.\n\n", col_name);
        return false;
    }
    return true;
}

int* get_fk_col_list_index(Query* query){
    /*
    The function returns an array/pointer of index matches with col_refer_list and table_refer_list
    Use this to convert col_refer_list and table_refer_list index to col_list, type_list, or constraint_list

    Example : 
    col_list = col1 pk, col2, col3 fk, col4, col5 fk
    fk_index = 2, 4 -> which are indexes of col3 and col5 in col_list
    */
    int fk_count = query->params.create_params.fk_count;
    int* res = (int*)malloc(sizeof(int) * fk_count);
    assert(res != NULL);
    int current_index = 0;
    int i;

    switch (query->cmd_type)
    {
    case CREATE:
        int col_count = query->params.create_params.col_count;
        ColConstraintType* constraint_list = query->params.create_params.constraint_list;
        for(i=0; i<col_count; i++){
            if(constraint_list[i] == FK){
                res[current_index] = i;
                current_index++;
            }
        }
        break;
    
    default:
        free(res);  
        break;
    }
    
    if(current_index != fk_count) free(res);

    return res;
}

Table* get_last_table(Table* first_table){
     
    Table* current_table = first_table;
    
    while(current_table->next_table){
        current_table = current_table->next_table;
    }
    return current_table;
}

Row* get_last_row(Row* first_row){
     
    Row* current_row = first_row;
    
    while(current_row->next_row){
        current_row = current_row->next_row;
    }
    return current_row;
}

Row* get_prev_row(Table* table, Row* target) {
    if (table->first_row == target)
        return NULL;

    Row* curr = table->first_row;
    while (curr && curr->next_row != target)
        curr = curr->next_row;

    return curr; 
}

Table* get_table_by_name(char* table_name) {
    Table* current = first_table;

    while (current != NULL) {
        if (strcmp(current->name, table_name) == 0) {
            return current;
        }
        current = current->next_table;
    }
    return NULL;
}

Col* get_col_by_name(Table* table, char* col_name) {
    Col* current = table->first_col;
    
    while (current != NULL) {
        if (strcmp(current->name, col_name) == 0) return current;
        current = current->next_col;
    }
    return NULL;
}

HashTable* get_ht_by_col_name(HashTable* first_ht, char* col_name){
    while(first_ht!=NULL){
        if(strcmp(first_ht->col_name, col_name)==0) return first_ht;

        first_ht = first_ht->next_hash_table;
    }
    return NULL;
}

void* get_col_value(Table* table, Row* row, char* col_name, ColType col_type) {
    if (table == NULL || row == NULL || col_name == NULL) return NULL;

    int list_index = get_data_list_index(table, col_name);

    switch (col_type) {
        case INT:
            if (row->int_list[list_index]) {
                return &(row->int_list[list_index][0]);
            }
            break;
        case STRING:
            if (row->str_list[list_index]) {
                return row->str_list[list_index];
            }
            break;
        case DOUBLE:
            if (row->double_list[list_index]) {
                return &(row->double_list[list_index][0]);
            }
            break;
        default:
            return NULL;
            break;
    }

    return NULL;
}

void* get_col_value_for_join(FilteredRow* filtered_set, SelectedColInfo col_info) {
    if(!filtered_set) return NULL;

    int data_index = col_info.data_index;

    switch (col_info.type) {
    case INT:
        if (filtered_set->int_joined_list[data_index]) {
            return &(filtered_set->int_joined_list[data_index][0]);
        }
        break;
    case STRING:
        if (filtered_set->str_joined_list[data_index]) {
            return filtered_set->str_joined_list[data_index];
        }
        break;
    case DOUBLE:
        if (filtered_set->double_joined_list[data_index]) {
            return &(filtered_set->double_joined_list[data_index][0]);
        }
        break;
    default:
        return NULL;
        break;
    }
    return NULL;
}

void format_value_to_string(ColType col_type, void* value, char* buffer, size_t buffer_size) {
    if (value == NULL) {
        snprintf(buffer, buffer_size, "NULL");
        return;
    }

    switch (col_type) {
        case INT:
            snprintf(buffer, buffer_size, "%d", *(int*)value);
            break;
        case DOUBLE:
            snprintf(buffer, buffer_size, "%g", *(double*)value);
            break;
        case STRING:
            snprintf(buffer, buffer_size, "%s", (char*)value);
            break;
        default:
            snprintf(buffer, buffer_size, "UNKNOWN");
    }
}

int get_data_list_index(Table* table, char* col_name){
    Col* current = table->first_col;
    int i_int = -1;
    int i_str = -1;
    int i_double = -1;
    
    while (current != NULL) {
        if(current->type == INT) i_int++;
        else if(current->type == DOUBLE) i_double++;
        else if(current->type == STRING) i_str++;

        if (strcmp(current->name, col_name) == 0) {
            switch(current->type) {
            case INT:
                return i_int;
                break;
            case STRING:
                return i_str;
                break;
            case DOUBLE:
                return i_double;
                break;
            default:
                return -1;  
                break;
            }
        }
        current = current->next_col;
    }
    return -1;  
}

int compare_double(double val1, double val2){
    double epsilon = DBL_EPSILON * 10.0;

    if (fabs(val1 - val2) <= epsilon) return 0;
    else if(val1<val2) return -1;
    else return 1;
}

bool refer_val_exists(char* str_to_check, int val_to_check, char* ref_table_name, char* ref_col_name) {
    HashTable* hash_tab = get_ht_by_col_name(get_table_by_name(ref_table_name)->first_hash_table, ref_col_name);
    Node* found_node = NULL;

    if (str_to_check != NULL) {
        found_node = exist_in_ht(hash_tab, 0, str_to_check);
        if (!found_node) {
            fprintf(stderr, "Execution error: referential integrity violated. Value '%s' for '%s' column of '%s' table does not exist.\n\n",
                    str_to_check, ref_col_name, ref_table_name);
            return false;
        }
    } else {
        found_node = exist_in_ht(hash_tab, val_to_check, NULL);
        if (!found_node) {
            fprintf(stderr, "Execution error: referential integrity violated. Value '%d' for '%s' column of '%s' table does not exist.\n\n",
                    val_to_check, ref_col_name, ref_table_name);
            return false;
        }
    }
     
    return true;
}

bool pk_value_is_unique(char* str_to_check, int val_to_check, HashTable* hash_tab, char* constraint){
    int sscanf_check = 0;
    int hashed_int;
    Node* current_hash_node = NULL;

    if(str_to_check!=NULL){
        hashed_int = hash_string(str_to_check);  

        if(hash_tab->bucket[hashed_int] != NULL){
            for(current_hash_node = hash_tab->bucket[hashed_int]; current_hash_node!=NULL; current_hash_node=current_hash_node->next_node){
                if(strcmp(current_hash_node->original_value, str_to_check) == 0){
                    fprintf(stderr, "Execution error: %s constraint violated.\n\n", constraint);
                    return false;
                }
            }
        }
        return true;
    
    }else{
        if(strcasecmp("PRIMARY KEY", constraint)==0 && val_to_check<=0){
            fprintf(stderr, "Execution error: values with PRIMARY KEY constraint must be 1 or larger.\n\n");
            return false;
        }
        hashed_int = hash_int(val_to_check);  
        int val_db;

        if(hash_tab->bucket[hashed_int] != NULL){
            for(current_hash_node = hash_tab->bucket[hashed_int]; current_hash_node!=NULL; current_hash_node=current_hash_node->next_node){
                 
                sscanf_check = sscanf(current_hash_node->original_value, "%d", &val_db); 
                if(sscanf_check != 1){
                    fprintf(stderr, "Execution error: an error occured while hashing.\n\n");
                    return false;
                }
                if(val_db == val_to_check){
                    fprintf(stderr, "Execution error: UNIQUE constraint violated.\n\n");
                    return false;
                }
            }
        }
        return true;
    }
}

char* int_to_str(int val){
    char buffer[20];
    char* res = NULL;

    snprintf(buffer, 20, "%d", val);
    assert((res = strdup(buffer))!=NULL);
    return res;
}

bool str_to_int(char *str_val, int *int_output, char *col_name) {
    errno = 0;
    char *endptr;
    long long parsed_val = strtoll(str_val, &endptr, 10);

    if (endptr == str_val || *endptr != '\0') {
        fprintf(stderr, "Execution error: invalid value '%s' for '%s' column type INT.\n\n", str_val, col_name);
        return false;
    }

    if (errno == ERANGE || parsed_val > INT_MAX || parsed_val < INT_MIN) {
        fprintf(stderr, "Execution error: incompatible size of '%s' for type INT.\n\n", str_val);
        return false;
    }

    *int_output = (int)parsed_val;
    return true;
}

bool str_to_double(char *str_val, double *double_output, char *col_name) {
    errno = 0;
    char *endptr;

    double parsed_val = strtod(str_val, &endptr);

    if (errno == ERANGE || isinf(parsed_val) || isnan(parsed_val) || endptr == str_val || *endptr != '\0') {
        fprintf(stderr, "Execution error: invalid value '%s' for '%s' column type DOUBLE.\n\n", str_val, col_name);
        return false;
    }

    *double_output = parsed_val;
    return true;
}

SelectedColInfo* build_col_info_list(Table* tab1, Table* tab2, SelectParams* params, int list_size){
    bool select_all = params->col_count == 1 && strcmp(params->col_list[0], "*") == 0;
    int int_index = 0;
    int double_index = 0;
    int str_index = 0;
    SelectedColInfo* output_col_info = NULL;
    Col* current_col = NULL;
    int i;
    int out_i = 0;

    assert((output_col_info = malloc(sizeof(SelectedColInfo) * list_size))!=NULL);

    if (select_all) {
        current_col = tab1->first_col;
        while (current_col != NULL && out_i < list_size) {
            output_col_info[out_i].table_id = 1;
            output_col_info[out_i].type = current_col->type;
            if (current_col->type == INT) {
                output_col_info[out_i].data_index = int_index++;
            } else if (current_col->type == DOUBLE) {
                output_col_info[out_i].data_index = double_index++;
            } else {
                output_col_info[out_i].data_index = str_index++;
            }
            out_i++;
            current_col = current_col->next_col;
        }
        current_col = tab2->first_col;
        while (current_col != NULL && out_i < list_size) {
            output_col_info[out_i].table_id = 2;
            output_col_info[out_i].type = current_col->type;
            if (current_col->type == INT) {
                output_col_info[out_i].data_index = int_index++;
            } else if (current_col->type == DOUBLE) {
                output_col_info[out_i].data_index = double_index++;
            } else {
                output_col_info[out_i].data_index = str_index++;
            }
            out_i++;
            current_col = current_col->next_col;
        }
    } else {
        for (i = 0; i < list_size; i++) {
            current_col = get_col_by_name(tab1, params->col_list[i]);
            if (current_col) {
                output_col_info[i].table_id = 1;
                output_col_info[i].type = current_col->type;
                output_col_info[i].data_index = get_data_list_index(tab1, params->col_list[i]);
            } else {
                current_col = get_col_by_name(tab2, params->col_list[i]);
                output_col_info[i].table_id = 2;
                output_col_info[i].type = current_col->type;
                output_col_info[i].data_index = get_data_list_index(tab2, params->col_list[i]);
            }
        }
    }

    return output_col_info;
}

bool str_to_col_type(Col* condition_col, char* condition_val, int* int_val, double* double_val, char** str_val){
    switch (condition_col->type) {
    case INT:
        if(!str_to_int(condition_val, int_val, condition_col->name)) return false;
        break;
    case DOUBLE:
        if(!str_to_double(condition_val, double_val, condition_col->name)) return false;
        break;
    case STRING:
        *str_val = condition_val;
        break;
    default:
        break;
    }
    return true;
}

FilteredRow* copy_rows_to_filtered(Table* tab){
    if(!tab || !tab->first_row) return NULL;

    Row* current_row = NULL;
    FilteredRow* head_list = NULL;
    FilteredRow* new_node = NULL;
    FilteredRow* last_node = NULL;

    for(current_row = tab->first_row; current_row; current_row=current_row->next_row){
        new_node = init_filtered_row();
        new_node->row = current_row;

        if(head_list == NULL) {
            head_list = new_node;
            last_node = head_list;
        } else {
             
            last_node->next_filtered_row = new_node;
            last_node = new_node;
        }
    }
    return head_list;
}

FilteredRow* copy_data_lists_to_filtered(Row* row1, Row* row2){
    FilteredRow* new_node = init_filtered_row();
    int i;
    int int_size = row2 ? row1->int_count + row2->int_count : row1->int_count;
    int double_size = row2 ? row1->double_count + row2->double_count : row1->double_count;
    int str_size = row2 ? row1->str_count + row2->str_count : row1->str_count;

    assert((new_node->int_joined_list = (int**)calloc(int_size, sizeof(int*))) != NULL);
    assert((new_node->double_joined_list = (double**)calloc(double_size, sizeof(double*))) != NULL);
    assert((new_node->str_joined_list = (char**)calloc(str_size, sizeof(char*))) != NULL);

    for (i = 0; i < row1->int_count; ++i) {
        if(row1->int_list[i] != NULL){
            new_node->int_joined_list[i] = malloc(sizeof(int));
            *new_node->int_joined_list[i] = *row1->int_list[i];
        }else new_node->int_joined_list[i] = NULL;
    }

    if(row2){
        for (i = 0; i < row2->int_count; ++i) {
            if(row2->int_list[row1->int_count + i] != NULL){
                new_node->int_joined_list[row1->int_count + i] = malloc(sizeof(int));
                *new_node->int_joined_list[row1->int_count + i] = *row2->int_list[i];
            }else new_node->int_joined_list[row1->int_count + i] = NULL;
        }
    }

    for (i = 0; i < row1->double_count; ++i) {
        if(row1->double_list[i] != NULL){
            new_node->double_joined_list[i] = malloc(sizeof(double));
            *new_node->double_joined_list[i] = *row1->double_list[i];
        }else new_node->double_joined_list[i] = NULL;
    }
    if(row2){
        for (i = 0; i < row2->double_count; ++i) {
            if(row1->double_list[row1->double_count + i] != NULL){
                new_node->double_joined_list[row1->double_count + i] = malloc(sizeof(double));
                *new_node->double_joined_list[row1->double_count + i] = *row2->double_list[i];
            }else new_node->double_joined_list[row1->double_count + i] = NULL;
        }
    }

    for(i = 0; i < str_size; i++){
        if(row1->str_list[i]) new_node->str_joined_list[i] = strdup(row1->str_list[i]);
        else new_node->str_joined_list[i] = NULL;
    }
    
    new_node->int_join_count = int_size;
    new_node->double_join_count = double_size;
    new_node->str_join_count = str_size;

    return new_node;
}

bool ref_integrity_check_delete(Table* table, FilteredRow* rows_to_del_fr, bool delete_all){
    Table* current_table = NULL;
    HashTable* ht_check = NULL;
    Node* value_exist;
    Col* current_col = NULL;
    Row* current_row = NULL;
    FilteredRow* current_row_to_del = NULL;
    Row* row_to_check = NULL;
    int data_index_referencing;
    int data_index_referenced;  

    for(current_table=first_table; current_table!=NULL; current_table=current_table->next_table){
        if(strcmp(current_table->name, table->name) != 0){
            
            for(current_col=current_table->first_col; current_col!=NULL; current_col=current_col->next_col){
                if(current_col->constraint == FK && strcmp(current_col->refer_table, table->name) == 0){
                    data_index_referencing = get_data_list_index(current_table, current_col->name);
                    data_index_referenced = get_data_list_index(table, current_col->refer_col);
                    ht_check = get_ht_by_col_name(table->first_hash_table, current_col->refer_col);

                    for(current_row = current_table->first_row; current_row!=NULL; current_row=current_row->next_row){
                        if(current_col->type == INT){
                             
                            if(!current_row->int_list[data_index_referencing]){
                                continue;
                            }
                            
                            int fk_value = current_row->int_list[data_index_referencing][0];
                            
                            value_exist = exist_in_ht(ht_check, fk_value, NULL);  
                            if(value_exist){
                                
                                if(delete_all){
                                    fprintf(stderr, "Execution error: referential integrity violated, '%s' column is referenced by '%s' column of '%s' table.\n\n", current_col->refer_col, current_col->name, current_table->name);
                                    return false;
                                }else{
                                    
                                    for(current_row_to_del=rows_to_del_fr; current_row_to_del!=NULL; current_row_to_del=current_row_to_del->next_filtered_row){
                                        row_to_check = current_row_to_del->row;
                                        
                                        if(!row_to_check->int_list[data_index_referenced]){
                                            continue;
                                        }
                                        
                                        int ref_value = row_to_check->int_list[data_index_referenced][0];
                                        
                                        if(fk_value == ref_value){
                                            fprintf(stderr, "Execution error: referential integrity violated, '%s' column is referenced by '%s' column of '%s' table.\n\n", current_col->refer_col, current_col->name, current_table->name);
                                            return false;
                                        }
                                    }
                                }
                            } 
                        }else if(current_col->type == STRING){
                            if(!current_row->str_list[data_index_referencing]) continue;
                            
                            value_exist = exist_in_ht(ht_check, 0, current_row->str_list[data_index_referencing]);  
                            if(value_exist){
                                if(delete_all){
                                    fprintf(stderr, "Execution error: referential integrity violated, '%s' column is referenced by '%s' column of '%s' table.\n\n", current_col->refer_col, current_col->name, current_table->name);
                                    return false;
                                }else{
                                    for(current_row_to_del=rows_to_del_fr; current_row_to_del!=NULL; current_row_to_del=current_row_to_del->next_filtered_row){
                                        row_to_check = current_row_to_del->row;
                                        
                                        if(!row_to_check->str_list[data_index_referenced]) continue;
                                        
                                        if(strcmp(current_row->str_list[data_index_referencing], row_to_check->str_list[data_index_referenced]) == 0){
                                            fprintf(stderr, "Execution error: referential integrity violated, '%s' column is referenced by '%s' column of '%s' table.\n\n", current_col->refer_col, current_col->name, current_table->name);
                                            return false;
                                        }
                                    }
                                }
                            }
                        }else{
                            fprintf(stderr, "Execution error: an unexpected error occured.\n\n");
                            return false;
                        }
                    }
                }
            }
        }
    }
    
    return true;
}
