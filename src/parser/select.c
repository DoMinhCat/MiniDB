/*
Date of creation : 27/10/2025
Description : parse_select to analyse select command
Group 2 ESGI 2A3
*/
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#include "../../include/parser.h"
#include "../../helper/parser/parser.h"

void parse_select(Query** query){
    char* token = NULL;
    char* col_list = NULL;
    char* from = NULL;
    char* table = NULL;
    char* extra_clause = NULL;
    char* extra_of_join = NULL;
    int current_col_count = 0;

    (*query)->cmd_type = SELECT;
    (*query)->params.select_params.col_count = 0;

     
    token = strtok(NULL, " \t");  
    if(!token || strlen(token) == 0){
        (*query)->cmd_type = INVALID;
        fprintf(stderr, "Syntax error: missing '(' or '*' after 'SELECT statement'.\n\n");
        return;
    } 
    if((strcmp(token, "(") != 0) && strcmp(token, "*") != 0){
        (*query)->cmd_type = INVALID;
        fprintf(stderr, "Syntax error: invalid command '%s' after 'SELECT statement'.\n\n", token);
        return;
    } 

     
    if(strcmp(token, "*") == 0){
        (*query)->params.select_params.col_list = (char**)malloc(sizeof(char*));
        assert((*query)->params.select_params.col_list != NULL);
         
        (*query)->params.select_params.col_list[0] = strdup(token);
        assert((*query)->params.select_params.col_list[0] != NULL);

        (*query)->params.select_params.col_count++;

         
        token = strtok(NULL, " \t");
        if(!contain_key_word(token, "FROM", query, "*")) return;
         
        token = strtok(NULL, " \t");
        if(!contain_param(token, query, "at least 1 table is required")) return;
         
        (*query)->params.select_params.table_name = strdup(token);
        assert((*query)->params.select_params.table_name != NULL);
        
        extra_clause = strtok(NULL, "\n");
    } 
     
    else if(strcmp(token, "(") == 0){
         
        col_list = strtok(NULL, ")");
         
        from = strtok(NULL, " \t");
         
        table = strtok(NULL, " \t");
         
        extra_clause = strtok(NULL, "\n");

         
        if(!contain_param(col_list, query, "at least 1 column is required for SELECT statement")) return;
         
        token = strtok(col_list, " ,\t");  
        while(token != NULL){
            current_col_count = (*query)->params.select_params.col_count;

             
            (*query)->params.select_params.col_list = (char**)realloc((*query)->params.select_params.col_list, (current_col_count+1) * sizeof(char*));
            assert(((*query)->params.select_params.col_list) != NULL);
             
            (*query)->params.select_params.col_list[current_col_count] = strdup(token);
            assert((*query)->params.select_params.col_list[current_col_count] != NULL);

            (*query)->params.select_params.col_count++;
             
            token = strtok(NULL, " ,\t");
        }

         
        if(!contain_key_word(from, "FROM", query, ")")) return;
    
         
        if(!contain_param(table, query, "at least 1 table is required for SELECT statement")) return;

         
        (*query)->params.select_params.table_name = strdup(table);
        assert((*query)->params.select_params.table_name != NULL);
    }

     
    if(extra_clause){
         
        token = strtok(extra_clause, " \t");
        if(strcasecmp(token, "JOIN") == 0){
    
             
            token = strtok(NULL, " \t");
            if(!contain_param(token, query, "at least 1 table is required for JOIN clause")) return;
            (*query)->params.select_params.table_join_name = strdup(token);
            assert((*query)->params.select_params.table_join_name != NULL);
            
             
            token = strtok(NULL, " \t");
            if(!contain_key_word(token, "ON", query, (*query)->params.select_params.table_join_name)) return;
    
             
            token = strtok(NULL, " \t");
            if(!contain_param(token, query, "2 columns are required for ON clause")) return;
            (*query)->params.select_params.first_col_on = strdup(token);
            assert((*query)->params.select_params.first_col_on != NULL);
    
             
            token = strtok(NULL, " \t");
            if(!contain_key_word(token, "=", query, (*query)->params.select_params.first_col_on)) return;
    
             
            token = strtok(NULL, " \t");
            if(!contain_param(token, query, "2 columns are required for ON clause")) return;
            (*query)->params.select_params.second_col_on = strdup(token);
            assert((*query)->params.select_params.second_col_on != NULL);

             
            extra_of_join = strtok(NULL, "\n");
            if(extra_of_join){
                
                 
                token = strtok(extra_of_join, " \t");
                if(strcasecmp(token, "WHERE") == 0){
                     
                    token = strtok(NULL, " \t");
                    check_where(token, query);
                    if((*query)->cmd_type == INVALID) return;
                }else{
                    check_end_of_cmd(token, query, "JOIN clause");
                    return;
                }
            } 
             
            else return; 

         
        }else if(strcasecmp(token, "WHERE") == 0){
             
            token = strtok(NULL, " \t");
            check_where(token, query);
            if((*query)->cmd_type == INVALID) return;
        }else{
            check_end_of_cmd(token, query, "SELECT statement");
            return;
        }
    }
    else return;
}