/*
Date of creation : 27/10/2025
Description : parse_create to analyse create command
Group 2 ESGI 2A3
*/

#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <ctype.h>

#include "../../include/parser.h"
#include "../../include/db.h"
#include "../../helper/parser/parser.h"

void parse_create(Query** query){
    char* token;
    // get column names, type, pk/fk
    char* col_list = NULL;
    char* col_def = NULL;        // single column definition (e.g., "col int pk")
    char* tmp_col_def = NULL;
    char* col_name = NULL;
    char* col_type = NULL;
    char* col_constraint = NULL;
    char* extra_cmd = NULL;
    int fk_count;
    char err_msg[200];

    char *saveptr1, *saveptr2;

    (*query)->cmd_type = CREATE;

    // check TABLE
    token = strtok(NULL, " \t");
    if(!contain_key_word(token, "TABLE", query, "CREATE")){
        return;
    }

    // get table name to create
    token = strtok(NULL, " \t");
    if(!contain_param(token, query, "1 table name is required for CREATE statement")){
        return;
    }

    //check max length
    if(exceed_max_len(token, query, TABLE_NAME_MAX, "table name")){
        return;
    }

    // check reserved keyword and special chars
    if(!is_valid_identifier(token, query)){
        return;
    }

    (*query)->params.create_params.table_name = strdup(token);
    assert((*query)->params.create_params.table_name != NULL);

    // check '('
    token = strtok(NULL, " \t");
    if(!contain_key_word(token, "(", query, (*query)->params.create_params.table_name)){
        return;
    }

    (*query)->params.create_params.fk_count = 0;
    (*query)->params.create_params.col_count = 0;
    
    // get col_list
    col_list = strtok(NULL, ")");
    extra_cmd = strtok(NULL, "\n");
    if(!contain_param(col_list, query, "at least 1 column is required for CREATE statement")){
        return;
    } 

    // get column definitions inside parentheses
    col_def = strtok_r(col_list, ",", &saveptr1);

    while(col_def != NULL){
        fk_count = (*query)->params.create_params.fk_count;

        tmp_col_def = strdup(col_def);
        assert(tmp_col_def != NULL);


        col_name = strtok_r(tmp_col_def, " \t", &saveptr2);
        if(!contain_param(col_name, query, "at least 1 column is required for CREATE statement")){
            free(tmp_col_def);
            return;
        } 
        
        if(exceed_max_len(col_name, query, TABLE_NAME_MAX, "column name")){
            free(tmp_col_def);
            return;
        } 
        
        if(!is_valid_identifier(col_name, query)){
            free(tmp_col_def);
            return;
        } 
        
        (*query)->params.create_params.col_list = (char**)realloc((*query)->params.create_params.col_list, ((*query)->params.create_params.col_count + 1) * sizeof(char*));
        assert((*query)->params.create_params.col_list != NULL);
        (*query)->params.create_params.col_list[(*query)->params.create_params.col_count] = strdup(col_name);
        assert((*query)->params.create_params.col_list[(*query)->params.create_params.col_count] != NULL);
        (*query)->params.create_params.col_count++;

        col_type = strtok_r(NULL, " \t", &saveptr2);
        sprintf(err_msg, "missing type for '%s' column ", (*query)->params.create_params.col_list[(*query)->params.create_params.col_count - 1]);
        if(!contain_param(col_type, query, err_msg)){
            free(tmp_col_def);
            return;
        } 

        (*query)->params.create_params.type_list = (ColType*)realloc((*query)->params.create_params.type_list, (*query)->params.create_params.col_count * sizeof(ColType));
        assert((*query)->params.create_params.type_list != NULL);

        if (strcasecmp(col_type, "INT") == 0)
            (*query)->params.create_params.type_list[(*query)->params.create_params.col_count - 1] = INT;
        else if (strcasecmp(col_type, "STRING") == 0 || strcasecmp(col_type, "STR") == 0)
            (*query)->params.create_params.type_list[(*query)->params.create_params.col_count - 1] = STRING;
        else if (strcasecmp(col_type, "DOUBLE") == 0)
            (*query)->params.create_params.type_list[(*query)->params.create_params.col_count - 1] = DOUBLE;
        else {
            (*query)->cmd_type = INVALID;
            fprintf(stderr, "Syntax error: invalid type '%s' for '%s' column .\n\n", col_type, (*query)->params.create_params.col_list[(*query)->params.create_params.col_count - 1]);
            free(tmp_col_def);
            return;
        }

        col_constraint = strtok_r(NULL, " \t", &saveptr2);
        (*query)->params.create_params.constraint_list = (ColConstraintType*)realloc((*query)->params.create_params.constraint_list, (*query)->params.create_params.col_count * sizeof(ColConstraintType));
        assert((*query)->params.create_params.constraint_list != NULL);

        if(col_constraint){
            if(strcasecmp(col_constraint, "UNIQUE") == 0){
                (*query)->params.create_params.constraint_list[(*query)->params.create_params.col_count - 1] = UNIQUE;
                token = strtok_r(NULL, " \t", &saveptr2);
                if(token){
                    sprintf(err_msg, "constraint declaration of '%s' column ", (*query)->params.create_params.col_list[(*query)->params.create_params.col_count - 1]);
                    check_end_of_cmd(token, query, err_msg);
                }
            }
            else if (strcasecmp(col_constraint, "PK") == 0){
                (*query)->params.create_params.constraint_list[(*query)->params.create_params.col_count - 1] = PK;
                token = strtok_r(NULL, " \t", &saveptr2);
                if(token){
                    sprintf(err_msg, "constraint declaration of '%s' column ", (*query)->params.create_params.col_list[(*query)->params.create_params.col_count - 1]);
                    check_end_of_cmd(token, query, err_msg);
                }
            }
            else if (strcasecmp(col_constraint, "FK") == 0){
                (*query)->params.create_params.constraint_list[(*query)->params.create_params.col_count - 1] = FK;

                token = strtok_r(NULL, " \t", &saveptr2);
                if(!contain_key_word(token, "REFERENCES", query, "FK")) { 
                    free(tmp_col_def); 
                    return; 
                }

                token = strtok_r(NULL, " \t", &saveptr2);
                if(!contain_param(token, query, "1 table is required for REFERENCES clause")) { free(tmp_col_def); return; }
                (*query)->params.create_params.table_refer_list = (char**)realloc((*query)->params.create_params.table_refer_list, (fk_count + 1) * sizeof(char*));
                assert(((*query)->params.create_params.table_refer_list) != NULL);
                (*query)->params.create_params.table_refer_list[fk_count] = strdup(token);
                assert((*query)->params.create_params.table_refer_list[fk_count] != NULL);

                token = strtok_r(NULL, " \t", &saveptr2);
                sprintf(err_msg, "1 column is required for '%s' table in REFERNCES clause", (*query)->params.create_params.table_refer_list[fk_count]);
                if(!contain_param(token, query, err_msg)) { free(tmp_col_def); return; }
                (*query)->params.create_params.col_refer_list = (char**)realloc((*query)->params.create_params.col_refer_list, (fk_count + 1) * sizeof(char*));
                assert(((*query)->params.create_params.col_refer_list) != NULL);
                (*query)->params.create_params.col_refer_list[fk_count] = strdup(token);
                assert((*query)->params.create_params.col_refer_list[fk_count] != NULL);

                (*query)->params.create_params.fk_count++;

                token = strtok_r(NULL, " \t", &saveptr2);
                if(token) check_end_of_cmd(token, query, "REFERENCES clause");
            }
            else {
                check_end_of_cmd(col_constraint, query, (*query)->params.create_params.col_list[(*query)->params.create_params.col_count - 1]);
            }
        }
        else {
            (*query)->params.create_params.constraint_list[(*query)->params.create_params.col_count - 1] = NONE;
        }

        free(tmp_col_def);
        col_def = strtok_r(NULL, ",", &saveptr1);
    }

    if(extra_cmd){
        check_end_of_cmd(extra_cmd, query, "CREATE statement");
        return;
    }
    else return;

    if((*query)->cmd_type == INVALID) return;
}
