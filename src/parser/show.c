/*
Date of creation : 07/11/2025
Description : analyse show command
Group 2 ESGI 2A3
*/

#include <stdlib.h>
#include <string.h>

#include "../../include/parser.h"
#include "../../helper/parser/parser.h"

void parse_show(Query** query){
    char* token = NULL;
    (*query)->cmd_type = SHOW;

    //check TABLES
    token = strtok(NULL, " \t");
    if(!contain_key_word(token, "TABLES", query, "SHOW")) return;

    // check extra invalid cmd
    token = strtok(NULL, " \t");
    if(token){
        check_end_of_cmd(token, query, "SHOW statement");
        return;
    }
}