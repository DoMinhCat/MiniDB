/*
Date of creation : 18/10/2025
Description : Free functions  Query pointers to call before exit
Group 2 ESGI 2A3
*/

#include <stdlib.h>

#include "../../include/clean.h"
#include "../../include/parser.h"

/*IMPORTANT : 
- Free all dynamic pointers in a struct before freeing the struct
*/ 

void free_query(Query** query){
    int i;
    if(!*query || !query) return;

    switch ((*query)->cmd_type)
    {
    case DESCRIBE:
        free((*query)->params.describe_params.table_name);
        (*query)->params.describe_params.table_name = NULL;
        break;
    case CREATE:
        // free table_name
        free((*query)->params.create_params.table_name);
        (*query)->params.create_params.table_name = NULL;
        //free col_list
        for(i=0; i<(*query)->params.create_params.col_count; i++){
            free((*query)->params.create_params.col_list[i]);
            (*query)->params.create_params.col_list[i] = NULL;
        }
        free((*query)->params.create_params.col_list);
        (*query)->params.create_params.col_list = NULL;

        //free type_list
        free((*query)->params.create_params.type_list);
        (*query)->params.create_params.type_list = NULL;

        //free constraint_list
        free((*query)->params.create_params.constraint_list);
        (*query)->params.create_params.constraint_list = NULL;

        //free table_refer_list
        for(i=0; i<(*query)->params.create_params.fk_count; i++){
            free((*query)->params.create_params.table_refer_list[i]);
            (*query)->params.create_params.table_refer_list[i] = NULL;
        }
        free((*query)->params.create_params.table_refer_list);
        (*query)->params.create_params.table_refer_list = NULL;

        //free col_refer_list
        for(i=0; i<(*query)->params.create_params.fk_count; i++){
            free((*query)->params.create_params.col_refer_list[i]);
            (*query)->params.create_params.col_refer_list[i] = NULL;
        }
        free((*query)->params.create_params.col_refer_list);
        (*query)->params.create_params.col_refer_list = NULL;

        (*query)->params.create_params.col_count = 0;
        (*query)->params.create_params.fk_count = 0;
        break;
    case INSERT:
        // free table_name
        free((*query)->params.insert_params.table_name);
        (*query)->params.insert_params.table_name = NULL;
        //free col_list
        for(i=0; i<(*query)->params.insert_params.col_count; i++){
            free((*query)->params.insert_params.col_list[i]);
            (*query)->params.insert_params.col_list[i] = NULL;
        }
        free((*query)->params.insert_params.col_list);
        (*query)->params.insert_params.col_list = NULL;
        (*query)->params.insert_params.col_count = 0;

        //free data_list
        for(i=0; i<(*query)->params.insert_params.col_count; i++){
            free((*query)->params.insert_params.data_list[i]);
            (*query)->params.insert_params.data_list[i] = NULL;
        }
        free((*query)->params.insert_params.data_list);
        (*query)->params.insert_params.data_list = NULL;
        break;
    case DELETE:
        // free table_name
        free((*query)->params.delete_params.table_name);
        (*query)->params.delete_params.table_name = NULL;
        // free condition_col 
        free((*query)->params.delete_params.condition_column);
        (*query)->params.delete_params.condition_column = NULL;
        // free condition_val 
        free((*query)->params.delete_params.condition_value);
        (*query)->params.delete_params.condition_value = NULL;
        break;
    case DROP:
        //free table_list
        for(i=0; i<(*query)->params.drop_params.table_count; i++){
            free((*query)->params.drop_params.table_list[i]);
            (*query)->params.drop_params.table_list[i] = NULL;
        }
        free((*query)->params.drop_params.table_list);
        (*query)->params.drop_params.table_list = NULL;
        break;
    case SELECT:
        // free table_name 
        free((*query)->params.select_params.table_name);
        (*query)->params.select_params.table_name = NULL;
        // free table_join_name 
        free((*query)->params.select_params.table_join_name);
        (*query)->params.select_params.table_join_name = NULL;

        // free col_list
        for(i=0; i<(*query)->params.select_params.col_count; i++){
            free((*query)->params.select_params.col_list[i]);
            (*query)->params.select_params.col_list[i] = NULL;
        }
        free((*query)->params.select_params.col_list);
        (*query)->params.select_params.col_list = NULL;

        // free first_col_on 
        free((*query)->params.select_params.first_col_on);
        (*query)->params.select_params.first_col_on = NULL;
        // free second_col_on 
        free((*query)->params.select_params.second_col_on);
        (*query)->params.select_params.second_col_on = NULL;

        // free condition_col 
        free((*query)->params.select_params.condition_col);
        (*query)->params.select_params.condition_col = NULL;
        // free condition_val 
        free((*query)->params.select_params.condition_val);
        (*query)->params.select_params.condition_val = NULL;
        break;
    default:
        // for case INVALID or EXIT, but initialised with NULL, so no need to free
        break;
    }

    free(*query);
    *query = NULL;
}

void free_current_cmd(char** cmd_string, Query** query){
    free(*cmd_string);
    *cmd_string = NULL;
    free_query(query);
    query = NULL;
}
