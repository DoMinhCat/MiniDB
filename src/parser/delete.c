/*
Date of creation : 27/10/2025
Description : parse_delete to analyse delete command
Group 2 ESGI 2A3
*/

#include <string.h>
#include <stdio.h>
#include <assert.h>

#include "../../include/parser.h"
#include "../../helper/parser/parser.h"

void parse_delete(Query** query){
    char* token = NULL;
    char* extra_where_clause = NULL;

    (*query)->cmd_type = DELETE;

     
    token = strtok(NULL, " \t");
    if(!contain_key_word(token, "FROM", query, "DELETE")) return;

     
    token = strtok(NULL, " \n");
    if(!contain_param(token, query, "1 table is required for DELETE statement")) return;
    (*query)->params.delete_params.table_name = strdup(token);
    assert((*query)->params.delete_params.table_name != NULL);

     
    extra_where_clause = strtok(NULL, "\n");

    if (extra_where_clause) {
        token = strtok(extra_where_clause, " \t");
        if(strcasecmp(token, "WHERE") == 0){
             
            token = strtok(NULL, " \t");
            check_where(token, query);
            if((*query)->cmd_type == INVALID) return;
        } else{
            check_end_of_cmd(token, query, "DELETE statement");
            return;
        }
    }else return;
}
