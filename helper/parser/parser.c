/*
Date of creation : 2/11/2025
Description : helper functions for parsing functions
Group 2 ESGI 2A3
*/

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>

#include "../../include/parser.h"
#include "parser.h"

bool contains_visible_char(char* str_to_check){
    int i;

    if(!str_to_check) return false;

    for(i=0; str_to_check[i] != '\0'; i++){
        if(!isspace(str_to_check[i])) return true;
    }

    return false;
}

void check_end_of_cmd(char* last_token, Query** query, char* current_stmt){
    if(contains_visible_char(last_token)){
        (*query)->cmd_type = INVALID;
        fprintf(stderr, "Syntax error: invalid command '%s' after '%s'.\n\n", last_token, current_stmt);
    }
}

bool contain_key_word(char* token, char* keyword, Query** query, char* current_stmt){
    if(!token){
        (*query)->cmd_type = INVALID;
        fprintf(stderr, "Syntax error: missing '%s' after '%s'.\n\n", keyword, current_stmt);
        return false;
    }
    if(strcasecmp(token, keyword) != 0){
        (*query)->cmd_type = INVALID;
        fprintf(stderr, "Syntax error: missing '%s' after '%s'.\n\n", keyword, current_stmt);
        return false;
    }
    return true;
}

bool contain_param(char* token, Query** query, char* err_msg){
    if(!token || strlen(token) == 0){
        (*query)->cmd_type = INVALID;
        fprintf(stderr, "Syntax error: %s.\n\n", err_msg);
        return false;
    }
    return true;
}

void check_where(char* token, Query** query){
     

    char error_msg[200];

    if(!contain_param(token, query, "at least 1 column is required for WHERE clause")) return;
    if((*query)->cmd_type == SELECT) {
        (*query)->params.select_params.condition_col = strdup(token);
        assert((*query)->params.select_params.condition_col != NULL);
    }
    else if((*query)->cmd_type == DELETE) {
        (*query)->params.delete_params.condition_column = strdup(token);
        assert((*query)->params.delete_params.condition_column != NULL);
    }
    else{
        (*query)->cmd_type = INVALID;
        fprintf(stderr, "Syntax error: invalid use of WHERE clause.\n\n");
        return;
    }

    token = strtok(NULL, " \t");
    if((*query)->cmd_type == SELECT){
        if(!contain_key_word(token, "=", query, (*query)->params.select_params.condition_col)) return;
    } else if((*query)->cmd_type == DELETE){
        if(!contain_key_word(token, "=", query, (*query)->params.delete_params.condition_column)) return;
    }

    token = strtok(NULL, " \t");
    if((*query)->cmd_type == SELECT) sprintf(error_msg, "1 value is required for '%s' column  in WHERE clause", (*query)->params.select_params.condition_col);
    else if((*query)->cmd_type == DELETE) sprintf(error_msg, "1 value is required for '%s' column  in WHERE clause", (*query)->params.delete_params.condition_column);
    if(!contain_param(token, query, error_msg)) return; 
    
    if((*query)->cmd_type == SELECT) {
        (*query)->params.select_params.condition_val = strdup(token);
        assert((*query)->params.select_params.condition_val != NULL);
    }
    else if((*query)->cmd_type == DELETE) {
        (*query)->params.delete_params.condition_value = strdup(token);
        assert((*query)->params.delete_params.condition_value != NULL);
    }
    else{
        (*query)->cmd_type = INVALID;
        fprintf(stderr, "Syntax error: invalid use of WHERE clause.\n\n");
        return;
    }
                
    token = strtok(NULL, "\n");
    if(token){
        check_end_of_cmd(token, query, "WHERE clause");
        if((*query)->cmd_type == INVALID) return;
    } else return;
}

bool exceed_max_len(char* token, Query** query, int max_len, char* current_str){
    if ((int)strlen(token)>max_len){
        (*query)->cmd_type = INVALID;
        fprintf(stderr, "Syntax error: %d characters maximum allowed for %s.\n\n", max_len-1, current_str);
        return true;
    }
    return false;
}

bool is_valid_identifier(char* token, Query** query){
    char* banned_name_list[] = {
        "SELECT", "INSERT", "VALUES", "DROP", "DELETE", "TABLE", "SHOW", "DESCRIBE", "TABLES", "PK", "FK", "UNIQUE", 
        "FROM", "INTO", "WHERE", "JOIN", "ON", "INT", "STRING", "DOUBLE"};
    int i;

    for(i=0; i<(int)(sizeof(banned_name_list) / sizeof(banned_name_list[0])); i++){
        if(strcasecmp(token, banned_name_list[i]) == 0){
            (*query)->cmd_type = INVALID;
            fprintf(stderr, "Syntax error: '%s' is a reserved keyword.\n\n", token);
            return false;
        }
    }
    
    for(i=0; i<(int)strlen(token); i++){
        if(!isalnum(token[i]) && token[i] != '_'){
            (*query)->cmd_type = INVALID;
            fprintf(stderr, "Syntax error: special character '%c' is not allowed.\n\n", token[i]);
            return false;
        }
    }

    return true;
}
