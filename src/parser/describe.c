/*
Date of creation : 07/11/2025
Description : analyse describe command
Group 2 ESGI 2A3
*/

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "../../include/parser.h"
#include "../../helper/parser/parser.h"

void parse_describe(Query** query){
    char* token = NULL;

    (*query)->cmd_type = DESCRIBE;

    //check TABLES
    token = strtok(NULL, " \t");
    if(!contain_key_word(token, "TABLE", query, "DESCRIBE")) return;

    // check table name
    token = strtok(NULL, " \t");
    if(!contain_param(token, query, "1 table name is required for DESCRIBE statement")) return;
    (*query)->params.describe_params.table_name = strdup(token);
    assert((*query)->params.describe_params.table_name!=NULL);

    // check extra invalid cmd
    token = strtok(NULL, " \t");
    if(token){
        check_end_of_cmd(token, query, "DESCRIBE statement");
        return;
    }
}