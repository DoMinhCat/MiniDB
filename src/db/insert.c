/*
Date of creation : 08/11/2025
Description : Code of INSERT statement
Group 2 ESGI 2A3
*/

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <assert.h>
#include <string.h>
#include <math.h>

#include "../../helper/db/db.h"
#include "../../include/db.h"
#include "../../include/parser.h"
#include "../../include/ini.h"
#include "../../include/hash.h"

void free_insert_before_exit(
    int*** int_list_to_insert, char*** str_list_to_insert, double*** double_list_to_insert, char** pk_int_col_name, 
    int str_col_count, int int_col_count, int double_col_count, int** int_unique_val_list, char*** str_unique_val_list,
    char*** int_unique_col_name_list, char*** str_unique_col_name_list, int unique_str_col_count, int unique_int_col_count){
    if(*int_list_to_insert){
        for(int i=0; i<int_col_count; i++){
            if((*int_list_to_insert)[i]){
                free((*int_list_to_insert)[i]);
                (*int_list_to_insert)[i] = NULL;
            }
        }
        free(*int_list_to_insert);
        *int_list_to_insert = NULL;
    }
    if(*str_list_to_insert){
        for(int i=0; i<str_col_count; i++){
            if((*str_list_to_insert)[i]){
                free((*str_list_to_insert)[i]);
                (*str_list_to_insert)[i] = NULL;
            }
        }
        free(*str_list_to_insert);
        *str_list_to_insert = NULL;
    }
    if(*double_list_to_insert){
        for(int i=0; i<double_col_count; i++){
            if((*double_list_to_insert)[i]){
                free((*double_list_to_insert)[i]);
                (*double_list_to_insert)[i] = NULL;
            }
        }
        free(*double_list_to_insert);
        *double_list_to_insert = NULL;
    }
    if(*int_unique_val_list){
        free(*int_unique_val_list);
        *int_unique_val_list = NULL;
    }
    if(*str_unique_val_list){
        for(int i=0; i<unique_str_col_count; i++){
            if((*str_unique_val_list)[i]){
                free((*str_unique_val_list)[i]);
                (*str_unique_val_list)[i] = NULL;
            }
        }
        free(*str_unique_val_list);
        *str_unique_val_list = NULL;
    }
    if(*int_unique_col_name_list){
        for(int i=0; i<unique_int_col_count; i++){
            if((*int_unique_col_name_list)[i]){
                free((*int_unique_col_name_list)[i]);
                (*int_unique_col_name_list)[i] = NULL;
            }
        }
        free(*int_unique_col_name_list);
        *int_unique_col_name_list = NULL;
    }
    if(*str_unique_col_name_list){
        for(int i=0; i<unique_str_col_count; i++){
            if((*str_unique_col_name_list)[i]){
                free((*str_unique_col_name_list)[i]);
                (*str_unique_col_name_list)[i] = NULL;
            }
        }
        free(*str_unique_col_name_list);
        *str_unique_col_name_list = NULL;
    }

    if(*pk_int_col_name){
        free(*pk_int_col_name);
        *pk_int_col_name = NULL;
    }
}

void insert(Query* query){
    char** col_list = query->params.insert_params.col_list;
    int col_count = query->params.insert_params.col_count;
    char** data_list = query->params.insert_params.data_list;
    Table* table = NULL;
    Col* current_col = NULL;
    Row* new_row = NULL;
    HashTable* first_hash_tab = NULL;
    HashTable* hash_tab_of_col = NULL;
    bool int_pk_provided = false;
    bool str_pk_provided = false;
    bool pk_is_int = false;
    bool pk_is_str = false;
    char* pk_int_col_name = NULL;
    int str_col_count = 0;
    int double_col_count = 0;
    int int_col_count = 0;

    // lists to store validated values for insert later
    int** int_list_to_insert = NULL;
    char** str_list_to_insert = NULL;
    double** double_list_to_insert = NULL;
    // lists of values to be hashed and inserted into hash table of unique cols
    int* int_unique_val_list = NULL;
    char** str_unique_val_list = NULL;
    char** int_unique_col_name_list = NULL;
    char** str_unique_col_name_list = NULL;
    int unique_int_col_count = 0;
    int unique_int_col_index = 0;
    int unique_str_col_count = 0;
    int unique_str_col_index = 0;

    // vars for type conversion
    double double_val;
    int safe_val;

    int int_pk_val;
    int int_pk_index;
    int col_index;
    int i,j;

    // check table exist
    table = get_table_by_name(query->params.insert_params.table_name);
    if(!table) {
        fprintf(stderr, "Execution error: '%s' table not found.\n\n", query->params.insert_params.table_name);
        return;
    }
    first_hash_tab = table->first_hash_table;

    // check no duplicate of col to insert
    for(i=0; i<col_count-1;i++){
        for(j=i+1; j<col_count;j++){
            if(strcmp(col_list[i], col_list[j])==0){
                fprintf(stderr, "Execution error: duplicate '%s' column not allowed.\n\n", col_list[i]);
                free_insert_before_exit(&int_list_to_insert, &str_list_to_insert, &double_list_to_insert, &pk_int_col_name, str_col_count, int_col_count, double_col_count, &int_unique_val_list, &str_unique_val_list, &int_unique_col_name_list, &str_unique_col_name_list, unique_str_col_count, unique_int_col_count);
                return;
            }
        }
    }

    // gather general info of table
    for(current_col = table->first_col; current_col!=NULL; current_col = current_col->next_col) {
        // get size to malloc for list of each type later
        if(current_col->type == INT) int_col_count++;
        else if(current_col->type == STRING) str_col_count++;
        else if(current_col->type == DOUBLE) double_col_count++;
        
        // check pk is string or int for later checks
        if(current_col->constraint == PK) {
            if(current_col->type == STRING) pk_is_str = true;
            else {
                pk_is_int = true;
                assert((pk_int_col_name = strdup(current_col->name)) != NULL);
            }
        }

        //get size to malloc for list of unique cols
        if(current_col->constraint == PK || current_col->constraint == UNIQUE){ 
            if(current_col->type == INT) unique_int_col_count++;
            else unique_str_col_count++;
        }
    }

    // calloc 3 lists str/int/double of Row
    double_list_to_insert = (double**)calloc(double_col_count, sizeof(double*));
    assert(double_list_to_insert!=NULL);
    int_list_to_insert = (int**)calloc(int_col_count, sizeof(int*));
    assert(int_list_to_insert!=NULL);
    str_list_to_insert = (char**)calloc(str_col_count, sizeof(char*));
    assert(str_list_to_insert!=NULL);

    //malloc for unique lists
    assert((int_unique_val_list = (int*)calloc(unique_int_col_count, sizeof(int))) != NULL);
    assert((str_unique_val_list = (char**)calloc(unique_str_col_count, sizeof(char*))) != NULL);
    assert((int_unique_col_name_list = (char**)calloc(unique_int_col_count, sizeof(char*))) != NULL);
    assert((str_unique_col_name_list = (char**)calloc(unique_str_col_count, sizeof(char*))) != NULL);

    // checks for cols to insert and init list for later insert
    for(i=0; i<col_count; i++){
        current_col = get_col_by_name(table, col_list[i]);

        //check for col existence
        if(!current_col){
            fprintf(stderr, "'%s' column  not found.\n\n", col_list[i]);
            free_insert_before_exit(&int_list_to_insert, &str_list_to_insert, &double_list_to_insert, &pk_int_col_name, str_col_count, int_col_count, double_col_count, &int_unique_val_list, &str_unique_val_list, &int_unique_col_name_list, &str_unique_col_name_list, unique_str_col_count, unique_int_col_count);
            return;
        }

        col_index = get_data_list_index(table, current_col->name);
        errno = 0;

        // based on col type
        switch (current_col->type){
        case INT:
            if(!str_to_int(data_list[i],&safe_val, col_list[i])){
                free_insert_before_exit(&int_list_to_insert, &str_list_to_insert, &double_list_to_insert, &pk_int_col_name, str_col_count, int_col_count, double_col_count, &int_unique_val_list, &str_unique_val_list, &int_unique_col_name_list, &str_unique_col_name_list, unique_str_col_count, unique_int_col_count);
                return;
            }

            // FK check for referential integrity
            if (current_col->constraint == FK) {
                if(safe_val <= 0){
                    fprintf(stderr, "Execution error: values with FOREIGN KEY constraint must be 1 or larger.\n\n");
                    free_insert_before_exit(&int_list_to_insert, &str_list_to_insert, &double_list_to_insert, &pk_int_col_name, str_col_count, int_col_count, double_col_count, &int_unique_val_list, &str_unique_val_list, &int_unique_col_name_list, &str_unique_col_name_list, unique_str_col_count, unique_int_col_count);
                    return;
                }
                if(!refer_val_exists(NULL, safe_val, current_col->refer_table, current_col->refer_col)){
                    free_insert_before_exit(&int_list_to_insert, &str_list_to_insert, &double_list_to_insert, &pk_int_col_name, str_col_count, int_col_count, double_col_count, &int_unique_val_list, &str_unique_val_list, &int_unique_col_name_list, &str_unique_col_name_list, unique_str_col_count, unique_int_col_count);
                    return;
                }
            }

            // checks for unique cols
            if (current_col->constraint == PK || current_col->constraint == UNIQUE) {
                // flag for auto increment later
                if(current_col->constraint == PK){
                    int_pk_provided = true;
                    int_pk_val = safe_val;
                }
                // save value and col name to insert into hash table later
                int_unique_val_list[unique_int_col_index] = safe_val;
                assert((int_unique_col_name_list[unique_int_col_index] = strdup(current_col->name)) != NULL);
                unique_int_col_index++;

                // check uniqueness of values to be inserted
                hash_tab_of_col = get_ht_by_col_name(first_hash_tab, current_col->name);
                if(!pk_value_is_unique(NULL, safe_val, hash_tab_of_col, current_col->constraint == UNIQUE?"UNIQUE":"PRIMARY KEY")){
                    free_insert_before_exit(&int_list_to_insert, &str_list_to_insert, &double_list_to_insert, &pk_int_col_name, str_col_count, int_col_count, double_col_count, &int_unique_val_list, &str_unique_val_list, &int_unique_col_name_list, &str_unique_col_name_list, unique_str_col_count, unique_int_col_count);
                    return;
                }
            }

            // Store validated value in int list to insert later
            assert((int_list_to_insert[col_index] = (int*)malloc(sizeof(int))) != NULL);
            int_list_to_insert[col_index][0] = safe_val;
            break;
        case DOUBLE:
            if(!str_to_double(data_list[i],&double_val, col_list[i])){
                free_insert_before_exit(&int_list_to_insert, &str_list_to_insert, &double_list_to_insert, &pk_int_col_name, str_col_count, int_col_count, double_col_count, &int_unique_val_list, &str_unique_val_list, &int_unique_col_name_list, &str_unique_col_name_list, unique_str_col_count, unique_int_col_count);
                return;
            }
            // no check for unique/pk/fk needed, double type not allowed to have unique/pk/fk constraint

            // expand temp list and store validated value
            assert((double_list_to_insert[col_index] = (double*)malloc(sizeof(double))) != NULL);
            double_list_to_insert[col_index][0] = (double)double_val;
            break;
        case STRING:
            // no need to convert type, data_list is stored as string

            // empty value not allowed
            if(data_list[i]){
                if(strcmp(data_list[i], "") == 0){
                    fprintf(stderr, "Execution error: empty string not allowed.\n\n");
                    free_insert_before_exit(&int_list_to_insert, &str_list_to_insert, &double_list_to_insert, &pk_int_col_name, str_col_count, int_col_count, double_col_count, &int_unique_val_list, &str_unique_val_list, &int_unique_col_name_list, &str_unique_col_name_list, unique_str_col_count, unique_int_col_count);
                    return;
                }
            }

            // check max_len 256
            if(strlen(data_list[i]) > MAX_STR_LEN){
                fprintf(stderr, "Execution error: 256 characters maximum allowed for STRING values.\n\n");
                free_insert_before_exit(&int_list_to_insert, &str_list_to_insert, &double_list_to_insert, &pk_int_col_name, str_col_count, int_col_count, double_col_count, &int_unique_val_list, &str_unique_val_list, &int_unique_col_name_list, &str_unique_col_name_list, unique_str_col_count, unique_int_col_count);
                return;
            }

            //check fk: referential integrity
            if(current_col->constraint == FK){
                if(!refer_val_exists(data_list[i], 0, current_col->refer_table, current_col->refer_col)){
                    free_insert_before_exit(&int_list_to_insert, &str_list_to_insert, &double_list_to_insert, &pk_int_col_name, str_col_count, int_col_count, double_col_count, &int_unique_val_list, &str_unique_val_list, &int_unique_col_name_list, &str_unique_col_name_list, unique_str_col_count, unique_int_col_count);
                    return;
                }
            }
            
            // check UNIQUE constraint and pk uniqueness
            if(current_col->constraint == PK || current_col->constraint == UNIQUE){ 
                if(current_col->constraint == PK) str_pk_provided = true;
                hash_tab_of_col = get_ht_by_col_name(first_hash_tab, current_col->name);
                if(!pk_value_is_unique(data_list[i], 0, hash_tab_of_col, current_col->constraint == UNIQUE?"UNIQUE":"PRIMARY KEY")){
                    free_insert_before_exit(&int_list_to_insert, &str_list_to_insert, &double_list_to_insert, &pk_int_col_name, str_col_count, int_col_count, double_col_count, &int_unique_val_list, &str_unique_val_list, &int_unique_col_name_list, &str_unique_col_name_list, unique_str_col_count, unique_int_col_count);
                    return;
                }
                // save value and col name to insert into hash table later
                assert((str_unique_val_list[unique_str_col_index] = strdup(data_list[i])) != NULL);
                assert((str_unique_col_name_list[unique_str_col_index] = strdup(current_col->name)) != NULL);
                unique_str_col_index++;
            }

            // expand temp list and store validated value
            str_list_to_insert[col_index] = strdup(data_list[i]);
            assert(str_list_to_insert[col_index]!=NULL);
            break;
        default:
            fprintf(stderr, "Execution error: unknown type for '%s' column .\n\n", col_list[i]);
            free_insert_before_exit(&int_list_to_insert, &str_list_to_insert, &double_list_to_insert, &pk_int_col_name, str_col_count, int_col_count, double_col_count, &int_unique_val_list, &str_unique_val_list, &int_unique_col_name_list, &str_unique_col_name_list, unique_str_col_count, unique_int_col_count);
            return;
            break;
        }
    }

    // throw error if no value to insert for str pk
    if(pk_is_str && !str_pk_provided){
        fprintf(stderr, "Execution error: value required for PRIMARY KEY column of '%s' table.\n\n", table->name);
        return;
    }

    // start inserting after all checks have passed
    new_row = init_row();

    new_row->int_count = int_col_count;
    new_row->str_count = str_col_count;
    new_row->double_count = double_col_count;
    
    // handle auto incrementation of pk type int
    if(int_pk_provided && pk_is_int){
        table->next_id = int_pk_val+1; // skip unused id gap
    }else if(pk_is_int){
        // auto set id, save value to insert into hash table later and increase next_id
        int_pk_index = get_data_list_index(table, pk_int_col_name);
        assert((int_list_to_insert[int_pk_index] = (int*)malloc(sizeof(int))) != NULL);
        int_list_to_insert[int_pk_index][0] = table->next_id;

        int_unique_val_list[unique_int_col_index] = table->next_id;
        assert((int_unique_col_name_list[unique_int_col_index] = strdup(pk_int_col_name)) != NULL);
        unique_int_col_index++;
        table->next_id++;
    }

    // copy saved list during check to data list 
    new_row->double_list = double_list_to_insert;
    new_row->str_list = str_list_to_insert;
    new_row->int_list = int_list_to_insert;

    // hash int values of unique int cols and add to their hash table
    int hashed_val;
    char* value = NULL;
    Row* last_row = !(table->first_row)? NULL : get_last_row(table->first_row);

    for(i=0; i<unique_int_col_index; i++){
        value = int_to_str(int_unique_val_list[i]);
        hashed_val = hash_int(int_unique_val_list[i]);
        hash_tab_of_col = get_ht_by_col_name(first_hash_tab, int_unique_col_name_list[i]);

        // add key-value pair to hash table of the correct int unique column
        add_to_ht(hash_tab_of_col, hashed_val, value, last_row, new_row);
    }
    // hash str values of unique str cols and add to their hash table
    for(i=0; i<unique_str_col_index; i++){
        hashed_val = hash_string(str_unique_val_list[i]);
        hash_tab_of_col = get_ht_by_col_name(first_hash_tab, str_unique_col_name_list[i]);

        // add key-value pair to hash table of the correct int unique column
        add_to_ht(hash_tab_of_col, hashed_val, str_unique_val_list[i], last_row, new_row);
    }

    // set pointer for row/next row
    last_row = NULL;
    if (!table->first_row) {
        table->first_row = new_row;
    } else {
        // append to the end
        last_row = get_last_row(table->first_row);
        last_row->next_row = new_row;
    }
    table->row_count++;
    // row insertion complete
    fprintf(stdout, "Executed: a new row was inserted into '%s' table.\n\n", table->name);
}