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

    // check "(" or *
    token = strtok(NULL, " \t"); // got ( or *
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

    // case SELECT *
    if(strcmp(token, "*") == 0){
        (*query)->params.select_params.col_list = (char**)malloc(sizeof(char*));
        assert((*query)->params.select_params.col_list != NULL);
        // no need memory allocation thanks to strdup :))
        (*query)->params.select_params.col_list[0] = strdup(token);
        assert((*query)->params.select_params.col_list[0] != NULL);

        (*query)->params.select_params.col_count++;

        //check FROM
        token = strtok(NULL, " \t");
        if(!contain_key_word(token, "FROM", query, "*")) return;
        //check table name
        token = strtok(NULL, " \t");
        if(!contain_param(token, query, "at least 1 table is required")) return;
        //put table name in param
        (*query)->params.select_params.table_name = strdup(token);
        assert((*query)->params.select_params.table_name != NULL);
        
        extra_clause = strtok(NULL, "\n");
    } 
    // case SELECT ( col1, col2 )
    else if(strcmp(token, "(") == 0){
        //get col list " col1, col2, ... "
        col_list = strtok(NULL, ")");
        //get FROM 
        from = strtok(NULL, " \t");
        //get table name
        table = strtok(NULL, " \t");
        //get end of command
        extra_clause = strtok(NULL, "\n");

        //check col_list
        if(!contain_param(col_list, query, "at least 1 column is required for SELECT statement")) return;
        //get each col and put into col_list
        token = strtok(col_list, " ,\t"); // got col1
        while(token != NULL){
            current_col_count = (*query)->params.select_params.col_count;

            // increase size of col_list 
            (*query)->params.select_params.col_list = (char**)realloc((*query)->params.select_params.col_list, (current_col_count+1) * sizeof(char*));
            assert(((*query)->params.select_params.col_list) != NULL);
            // put into col_list
            (*query)->params.select_params.col_list[current_col_count] = strdup(token);
            assert((*query)->params.select_params.col_list[current_col_count] != NULL);

            (*query)->params.select_params.col_count++;
            // get next col
            token = strtok(NULL, " ,\t");
        }

        // check from
        if(!contain_key_word(from, "FROM", query, ")")) return;
    
        // check table
        if(!contain_param(table, query, "at least 1 table is required for SELECT statement")) return;

        //put table name in param
        (*query)->params.select_params.table_name = strdup(table);
        assert((*query)->params.select_params.table_name != NULL);
    }

    // check optional where or join
    if(extra_clause){
        // case join
        token = strtok(extra_clause, " \t");
        if(strcasecmp(token, "JOIN") == 0){
    
            // check tab to join
            token = strtok(NULL, " \t");
            if(!contain_param(token, query, "at least 1 table is required for JOIN clause")) return;
            (*query)->params.select_params.table_join_name = strdup(token);
            assert((*query)->params.select_params.table_join_name != NULL);
            
            //check ON
            token = strtok(NULL, " \t");
            if(!contain_key_word(token, "ON", query, (*query)->params.select_params.table_join_name)) return;
    
            //check first col of ON
            token = strtok(NULL, " \t");
            if(!contain_param(token, query, "2 columns are required for ON clause")) return;
            (*query)->params.select_params.first_col_on = strdup(token);
            assert((*query)->params.select_params.first_col_on != NULL);
    
            //check =
            token = strtok(NULL, " \t");
            if(!contain_key_word(token, "=", query, (*query)->params.select_params.first_col_on)) return;
    
            //check second col on
            token = strtok(NULL, " \t");
            if(!contain_param(token, query, "2 columns are required for ON clause")) return;
            (*query)->params.select_params.second_col_on = strdup(token);
            assert((*query)->params.select_params.second_col_on != NULL);

            // check optional where after join
            extra_of_join = strtok(NULL, "\n");
            if(extra_of_join){
                
                //check WHERE 
                token = strtok(extra_of_join, " \t");
                if(strcasecmp(token, "WHERE") == 0){
                    //check condition column
                    token = strtok(NULL, " \t");
                    check_where(token, query);
                    if((*query)->cmd_type == INVALID) return;
                }else{
                    check_end_of_cmd(token, query, "JOIN clause");
                    return;
                }
            } 
            // no extra clause after join
            else return; 

        // case where
        }else if(strcasecmp(token, "WHERE") == 0){
            //check condition column
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