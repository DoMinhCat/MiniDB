/*
Date of creation : 27/10/2025
Description : parse_insert to analyse insert command
Group 2 ESGI 2A3
*/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "../../include/parser.h"
#include "../../helper/parser/parser.h"

void parse_insert(Query** query){
    char* token;

    (*query)->cmd_type = INSERT;

     
    token = strtok(NULL, " \t");
    if(!contain_key_word(token, "INTO", query, "INSERT statement")) return;

     
    token = strtok(NULL, " \t");
    if(!contain_param(token, query, "1 table is required for INSERT statement")) return;
    (*query)->params.insert_params.table_name = strdup(token);
    assert((*query)->params.insert_params.table_name != NULL);
 
     
    token = strtok(NULL, " \t");
    if(!contain_key_word(token, "(", query, (*query)->params.insert_params.table_name)) return;

     
    char* col_list = NULL;
    char* value_keyword = NULL;
    char* open_value = NULL;
    char* data_list = NULL;
    char* extra_cmd = NULL;
    int current_col_count;
    (*query)->params.insert_params.col_count = 0;

    col_list = strtok(NULL, ")");  
    value_keyword = strtok(NULL, " \t");  
    open_value = strtok(NULL, " \t");  
    data_list = strtok(NULL, ")");  
    extra_cmd = strtok(NULL, "\n");

    if(!contain_param(col_list, query, "at least 1 column is required for INSERT statement")) return;

     
    token = strtok(col_list, " ,\t");  
    while(token != NULL){
        current_col_count = (*query)->params.insert_params.col_count;

         
        (*query)->params.insert_params.col_list = (char**)realloc((*query)->params.insert_params.col_list, (current_col_count+1) * sizeof(char*));
        assert(((*query)->params.insert_params.col_list) != NULL);
         
        (*query)->params.insert_params.col_list[current_col_count] = strdup(token);
        assert((*query)->params.insert_params.col_list[current_col_count] != NULL);

        (*query)->params.insert_params.col_count++;
         
        token = strtok(NULL, "\t, ");
    }

     
    if(!contain_key_word(value_keyword, "VALUES", query, "column list in INSERT statement")) return;

     
    if(!contain_key_word(open_value, "(", query, "VALUES")) return;

     
    int val_count = 0;
    if(!contain_param(data_list, query, "at least 1 value is required for INSERT statement")) return;

     
    token = strtok(data_list, " ,\t");  
    while(token != NULL){
         
        (*query)->params.insert_params.data_list = (char**)realloc((*query)->params.insert_params.data_list, (val_count+1) * sizeof(char*)); 
        assert(((*query)->params.insert_params.data_list) != NULL);
         
        (*query)->params.insert_params.data_list[val_count] = strdup(token);
        assert((*query)->params.insert_params.data_list[val_count] != NULL);

        val_count++;
         
        token = strtok(NULL, "\t, ");
    }

     
    if(extra_cmd){
        check_end_of_cmd(extra_cmd, query, "INSERT statement");
        return;
    }

     
    if(val_count != (*query)->params.insert_params.col_count){
        (*query)->cmd_type = INVALID;
        fprintf(stderr, "Syntax error: %d %s provided for %d %s.\n\n", val_count, val_count>1?"values":"value", (*query)->params.insert_params.col_count, (*query)->params.insert_params.col_count>1?"columns":"column");
        return;
    }
}