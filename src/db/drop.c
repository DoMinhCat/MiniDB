/*
Date of creation : 03/11/2025
Description : Code for drop table operation
Group 2 ESGI 2A3
*/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

#include "../../helper/db/db.h"
#include "../../include/db.h"
#include "../../include/parser.h"
#include "../../include/ini.h"
#include "../../include/hash.h"
#include "../../include/clean.h"
#include "../../include/global.h"

void drop_table(Query* query) {
    Table* current_table = NULL;
    Table* tb_to_del = NULL;
    Table* prev_table = NULL;
    char* table_name = NULL;

    int i;
    int table_count = query->params.drop_params.table_count;

    // Loop through each table provided in query, check 
    for(i=0; i<table_count; i++){
        table_name = query->params.drop_params.table_list[i];
    
        // check existence
        tb_to_del = get_table_by_name(table_name);
        if(!tb_to_del){
            fprintf(stderr, "Execution error: '%s' table not found.\n\n", table_name);
            return;
        } 

        // Check if any other table has a foreign key col references to current table
        if(!ref_integrity_check_delete(tb_to_del, NULL, true)) return;   
    }  

    for(i=0; i<table_count; i++){
        table_name = query->params.drop_params.table_list[i];
        
        // drop table
        current_table = first_table;
        prev_table = NULL;
        while(current_table != NULL) {
            //set prev_table
            if(current_table == first_table) prev_table = NULL; 

            if(strcmp(current_table->name, table_name) == 0) {
                // table found
                tb_to_del = current_table; 

                // tb to del is the first table
                if(tb_to_del == first_table) first_table = tb_to_del->next_table;
                //if tb to del is mid/last
                else prev_table->next_table = tb_to_del->next_table;
                
                // advance to next cuz we will lose pointer to this table to be deleted
                current_table = current_table->next_table;

                // drop table
                free_table(tb_to_del);
                tb_to_del = NULL;
                break;
            } 
            //if current_table is not to be deleted, advance pointers normally
            prev_table = current_table;
            current_table = current_table->next_table;
        }
    }

    table_count--;

    // print result
    fprintf(stdout, "Executed: %s ", table_count > 1 ? "tables" : "table");
    for (i = 0; i < table_count; i++) {
        fprintf(stdout, "'%s'%s", query->params.drop_params.table_list[i], (i < table_count - 1) ? ", " : ""); // add other table names
    }
    fprintf(stdout, " dropped successfully.\n\n");

    return;
}