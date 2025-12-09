/*
Date of creation : 27/10/2025
Description : parse_select to analyse select command
Group 2 ESGI 2A3
*/

#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "../../include/parser.h"
#include "../../helper/parser/parser.h"

void parse_drop(Query** query){
    char* token;
    char* table_list;
    int table_count;

    (*query)->cmd_type = DROP;

     
    token = strtok(NULL, " \t");
    if(!contain_key_word(token, "TABLE", query, "DROP")) return;

     
    table_list = strtok(NULL, "\n");  
    if(!contain_param(table_list, query, "at least 1 table is required for DROP statement")) return;

    (*query)->params.drop_params.table_count = 0;

    token = strtok(table_list, " ,\t");  
    while(token != NULL){
        table_count = (*query)->params.drop_params.table_count;

         
        (*query)->params.drop_params.table_list = (char**)realloc((*query)->params.drop_params.table_list, (table_count+1) * sizeof(char*));
        assert((*query)->params.drop_params.table_list != NULL);
        (*query)->params.drop_params.table_list[table_count] = strdup(token);
        assert((*query)->params.drop_params.table_list[table_count] != NULL);
        
        (*query)->params.drop_params.table_count++;
        token = strtok(NULL, " ,\t");
    }
    
     
}