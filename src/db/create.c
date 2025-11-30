/*
Date of creation : 17/10/2025
Description : Code for create_table function
Group 2 ESGI 2A3
*/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

#include "../../include/db.h"
#include "../../helper/db/db.h"
#include "../../include/parser.h"
#include "../../include/ini.h"
#include "../../include/hash.h"
#include "../../include/global.h"
#include "../../include/clean.h"

void create_table(Query* query){
    Table* current_table = first_table;

    ColType* type_list = query->params.create_params.type_list;
    char* new_tb_name = query->params.create_params.table_name;
    int col_count = query->params.create_params.col_count;
    char** col_list = query->params.create_params.col_list;
    ColConstraintType* constraint_list = query->params.create_params.constraint_list;
    char** refer_table_list =  query->params.create_params.table_refer_list;
    char** refer_col_list =  query->params.create_params.col_refer_list;

    int i,j,k;

    // check max table, if >= max then don't allow table creation
    if(table_count>=MAX_TABLE_COUNT){
        fprintf(stderr, "Execution error: 200 tables limit reached.\n\n");
        return;
    }
    // check max col
    if(col_count>MAX_COL_COUNT){
        fprintf(stderr, "Execution error: 50 columns per table limit reached.\n\n");
        return;
    }
    
    // check table name
    while(current_table){
        if(strcmp(current_table->name, new_tb_name) == 0){
            fprintf(stderr, "Execution error: '%s' table already exist.\n\n", new_tb_name);
            return;
        }
        current_table = current_table->next_table;
    }

    // check col names
    for(i=0; i<(col_count-1); i++){
        for(j=i+1; j<col_count; j++){
            if(strcmp(col_list[i], col_list[j]) == 0){
                fprintf(stderr, "Execution error: duplicated '%s' columns in '%s' table.\n\n", col_list[i], new_tb_name);
                return;
            }
        }
    }

    int pk_count = 0;
    int pk_index;
    char* pk_col_name = NULL; // to set in hash table later
    
    //check 1 pk and check UNIQUE on DOUBLE not allowed
    for(i=0; i<col_count; i++){
        if(constraint_list[i] == PK){ 
            // free before strdup in case 2 pk
            free(pk_col_name);
            pk_col_name = NULL;

            pk_col_name = strdup(col_list[i]);
            pk_index = i;
            assert(pk_col_name != NULL);

            pk_count++;
            if(pk_count > 1){
                fprintf(stderr, "Execution error: a table must not have multiple PRIMARY KEY columns.\n\n");
                free(pk_col_name);
                pk_col_name = NULL;
                return;
            }
        } else if(constraint_list[i] == UNIQUE && type_list[i] == DOUBLE){
            fprintf(stderr, "Execution error: UNIQUE constraint not allowed for type DOUBLE.\n\n");
            return;
        }
    }
    if(pk_count != 1){
        fprintf(stderr, "Execution error: a table must have a PRIMARY KEY column.\n\n");
        free(pk_col_name);
        pk_col_name = NULL;
        return;
    }

    // check type int/string for pk only
    if(type_list[pk_index] != INT && type_list[pk_index] != STRING){
        fprintf(stderr, "Execution error: PRIMARY KEY's type must be INT or STRING.\n\n");
        return;
    }

    //check fk
    int fk_count = query->params.create_params.fk_count;

    if(fk_count>0){
        int* fk_list_index = get_fk_col_list_index(query);
        assert(fk_list_index != NULL);
        bool refer_table_exist;
        bool refer_col_exist;
        char** table_refer_list = query->params.create_params.table_refer_list;
        char** col_refer_list = query->params.create_params.col_refer_list;
        Table* refered_table = NULL;
        Col* refered_col = NULL;
        Col* current_col = NULL;

        // refering to the table itself is not allowed, since it hasn't been created yet
        for(j=0; j<fk_count; j++){
            if (strcmp(new_tb_name, table_refer_list[j]) == 0) {
                fprintf(stderr, "Execution error: a table can't reference itself.\n\n");
                free(pk_col_name);
                pk_col_name = NULL;
                return;
            }
        }
        
        // check many cols refer to same col not allowed
        for(j=0; j<fk_count-1; j++){
            for(k=j+1; k<fk_count; k++){
                if(strcmp(col_refer_list[j],col_refer_list[k]) == 0 && strcmp(table_refer_list[j], table_refer_list[k]) == 0){
                    fprintf(stderr, "Execution error: many columns refering to the same '%s' column of '%s' table is not allowed.\n\n", col_refer_list[j], table_refer_list[j]);
                    free(pk_col_name);
                    pk_col_name = NULL;
                    return;
                }
            }
        }

        // loop through all fk to check other criterias
        for(j=0; j<fk_count; j++){
            refer_table_exist = false;
            refer_col_exist = false;
            // check table refered exists
            //if it is the first table, it can't refer to anything
            if(!first_table){
                fprintf(stderr, "Execution error: '%s' table referenced by '%s' column does not exist.\n\n", table_refer_list[j], col_list[fk_list_index[j]]);
                free(pk_col_name);
                pk_col_name = NULL;
                free(fk_list_index);
                fk_list_index = NULL;
                return;
            }
            for(current_table = first_table; current_table != NULL; current_table = current_table->next_table){
                if(strcmp(current_table->name, table_refer_list[j]) == 0){
                    refer_table_exist = true; // flag to check if all fk refer to existing tables
                    refered_table = current_table; // get the pointer to table to check if refered col exists later
                    break;
                }
            }
            // if a table refered to doesn't exist, return error
            if(!refer_table_exist){
                fprintf(stderr, "Execution error: '%s' table referenced by '%s' column does not exist.\n\n", table_refer_list[j], col_list[fk_list_index[j]]);
                free(pk_col_name);
                pk_col_name = NULL;
                free(fk_list_index);
                fk_list_index = NULL;
                return;
            }

            // table exist, check for col refered to exists in that table
            for(current_col = refered_table->first_col; current_col != NULL; current_col = current_col->next_col){
                if(strcmp(current_col->name, col_refer_list[j]) == 0){
                    refer_col_exist = true; // flag to check if col exist
                    refered_col = current_col;
                    break;
                }
            }
            // if col doesn't exist, return error
            if(!refer_col_exist){
                fprintf(stderr, "Execution error: '%s' column does not exist in the referenced '%s' table.\n\n", col_refer_list[j], table_refer_list[j]);
                free(pk_col_name);
                pk_col_name = NULL;
                free(fk_list_index);
                fk_list_index = NULL;
                return;
            }

            // check if col is pk or unique ?
            if(refered_col->constraint != PK && refered_col->constraint != UNIQUE){
                fprintf(stderr, "Execution error: '%s' column in the referenced '%s' table does not have UNIQUE constraint.\n\n", col_refer_list[j], table_refer_list[j]);
                free(pk_col_name);
                pk_col_name = NULL;
                free(fk_list_index);
                fk_list_index = NULL;
                return;
            }

            // check if col type is the same as col that refer to it
            if(refered_col->type != type_list[fk_list_index[j]]){
                fprintf(stderr, "Execution error: '%s' column in the referenced '%s' table is not the same type as '%s' column.\n\n", col_refer_list[j], table_refer_list[j], col_list[fk_list_index[j]]);
                free(pk_col_name);
                pk_col_name = NULL;
                free(fk_list_index);
                fk_list_index = NULL;
                return;
            }
        }
        free(fk_list_index);
        fk_list_index = NULL;
    }

    // create/malloc new table when all check is passed
    Table* new_tb = init_table();
    Col* new_col = NULL;

    // set table name
    new_tb->name = strdup(new_tb_name);
    assert(new_tb->name != NULL);

    // add col
    Col* last_col = NULL;
    HashTable* new_hash_table = NULL;
    HashTable* last_ht = NULL;
    int refer_list_index=0;
    int ht_count = 0;
    // loop through the col list and add them in the linked list
    for(i=0; i<col_count; i++){
        // set basic info
        new_col = init_col();
        new_col->name = strdup(col_list[i]);
        assert(new_col->name != NULL);
        new_col->type = type_list[i];
        new_col->constraint = constraint_list[i];

        // if col to add is fk, set table and col it refers to
        if(new_col->constraint == FK){
            new_col->refer_table = strdup(refer_table_list[refer_list_index]);
            assert(new_col->refer_table!=NULL);

            new_col->refer_col = strdup(refer_col_list[refer_list_index]);
            assert(new_col->refer_col!=NULL);

            refer_list_index++;
        }
        // set pointer to next col
        if (new_tb->first_col == NULL || new_tb->first_col->name == NULL) {
            // free the dummy if it exists
            if (new_tb->first_col && new_tb->first_col->name == NULL) free_col(new_tb->first_col);
            new_tb->first_col = new_col;
            last_col = new_tb->first_col;
        } else {
            // append to the end
            last_col->next_col = new_col;
            last_col = new_col;
        }

        // init hash table for unique/pk cols
        if(new_col->constraint == UNIQUE || new_col->constraint == PK){
            new_hash_table = init_hash_table();
            // set col name of hash table
            assert((new_hash_table->col_name = strdup(col_list[i])) != NULL);
            
            // set pointer
            if (new_tb->first_hash_table == NULL || new_tb->first_hash_table->col_name == NULL) {
                // free the dummy if it exists
                if (new_tb->first_hash_table && new_tb->first_hash_table->col_name == NULL) free_hash_table(new_tb->first_hash_table);
                new_tb->first_hash_table = new_hash_table;
                last_ht = new_tb->first_hash_table;
            } else {
                // append to the end
                last_ht->next_hash_table = new_hash_table;
                last_ht = new_hash_table;
            }
            ht_count++;
        }
    }
    new_tb->col_count = col_count;
    new_tb->row_count = 0;
    new_tb->hash_table_count = ht_count;

    // add table to the linked list
    if(!first_table) first_table = new_tb;
    else{
        current_table = get_last_table(first_table);
        current_table->next_table = new_tb;
    }

    // global table count
    table_count++;
    // prints success message
    fprintf(stdout, "Executed: '%s' table created successfuly with %d %s.\n\n", new_tb_name, col_count, col_count>1?"columns":"column");
    free(pk_col_name);
    pk_col_name = NULL;
}