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

     
    for(i=0; i<table_count; i++){
        table_name = query->params.drop_params.table_list[i];
    
         
        tb_to_del = get_table_by_name(table_name);
        if(!tb_to_del){
            fprintf(stderr, "Execution error: '%s' table not found.\n\n", table_name);
            return;
        } 

         
        if(!ref_integrity_check_delete(tb_to_del, NULL, true)) return;   
    }  

    for(i=0; i<table_count; i++){
        table_name = query->params.drop_params.table_list[i];
        
         
        current_table = first_table;
        prev_table = NULL;
        while(current_table != NULL) {
             
            if(current_table == first_table) prev_table = NULL; 

            if(strcmp(current_table->name, table_name) == 0) {
                 
                tb_to_del = current_table; 

                 
                if(tb_to_del == first_table) first_table = tb_to_del->next_table;
                 
                else prev_table->next_table = tb_to_del->next_table;
                
                 
                current_table = current_table->next_table;

                 
                free_table(tb_to_del);
                tb_to_del = NULL;
                break;
            } 
             
            prev_table = current_table;
            current_table = current_table->next_table;
        }
    }

    table_count--;

     
    fprintf(stdout, "Executed: %s ", table_count > 1 ? "tables" : "table");
    for (i = 0; i < table_count; i++) {
        fprintf(stdout, "'%s'%s", query->params.drop_params.table_list[i], (i < table_count - 1) ? ", " : "");  
    }
    fprintf(stdout, " dropped successfully.\n\n");

    return;
}