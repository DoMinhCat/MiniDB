/*
Date of creation : 07/11/2025
Description : Code for describe function
Group 2 ESGI 2A3
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

typedef struct Query Query;
#include "db.h"
#include "../../include/global.h"
#include "../../helper/db/db.h"
#include "../../helper/format/format.h"

void describe_table(Query* query){
    Table* table = NULL;
    Col* current_col = NULL;
    char* name = NULL;
    char* type = NULL;
    char* constraint = NULL;
    char* table_refer = NULL;
    char* col_refer = NULL;
    int current_width = 0;
    int col_width = 20;  
    int num_cols = 5;
    int max_width = MAX_TABLE_WIDTH;
    int i;

     
    table = get_table_by_name(query->params.describe_params.table_name);

    if(!table){
        fprintf(stderr, "Execution error: '%s' table not found.\n\n", query->params.describe_params.table_name);
        return;
    } 

     
    current_width = 0;
    char* headers[] = {"Column", "Type", "Constraint", "Table of Reference", "Column of Reference"};
    int printed_headers = 0;
    
    for(i = 0; i < num_cols; i++) {
        if(current_width + col_width + 3 > max_width && i > 0) {   
            printf("| ...");
            break;
        }
        if(i == 0) {
            printf("| %-*s", col_width, headers[i]);
            current_width += col_width;
        } else {
            printf(" | %-*s", col_width, headers[i]);
            current_width += col_width + 3;
        }
        printed_headers++;
    }
    printf(" |\n");

     
    current_width = 0;
    for(i = 0; i < printed_headers; i++) {
        if(i == 0) {
            printf("|-");
            for(int j = 0; j < col_width; j++) printf("-");
            current_width += col_width;
        } else {
            printf("-|-");
            for(int j = 0; j < col_width; j++) printf("-");
            current_width += col_width + 3;
        }
    }
    if(printed_headers < num_cols) {
        printf("-|-----");
    }
    printf("-|\n");

     
    for(current_col=table->first_col; current_col!=NULL; current_col = current_col->next_col){
        name = strdup(current_col->name);
        assert(name!=NULL);

        if(current_col->type == STRING) type = "String";
        else if(current_col->type == INT) type = "Int";
        else if(current_col->type == DOUBLE) type = "Double";
        else type = "Unknown";

        if(current_col->constraint == PK) constraint = "Primary key";
        else if(current_col->constraint == FK) constraint = "Foreign key";
        else if(current_col->constraint == UNIQUE) constraint = "Unique";
        else if(current_col->constraint == NONE) constraint = "None";
        else constraint = "Unknown";

        if(current_col->refer_table){
            table_refer = strdup(current_col->refer_table);
            assert(table_refer!=NULL);
        } else table_refer = NULL;

        if(current_col->refer_col){
            col_refer = strdup(current_col->refer_col);
            assert(col_refer!=NULL);
        } else col_refer = NULL;

         
        current_width = 0;
        char* values[] = {name, type, constraint, table_refer?table_refer:"None", col_refer?col_refer:"None"};
        char truncated[col_width + 1];
        
        for(int i = 0; i < num_cols; i++) {
            if(current_width + col_width + 3 > max_width && i > 0) {
                printf("| ...");
                break;
            }
            
             
            if((int)strlen(values[i]) > col_width) {
                strncpy(truncated, values[i], col_width - 3);
                truncated[col_width - 3] = '\0';
                strcat(truncated, "...");
            } else {
                strcpy(truncated, values[i]);
            }
            
            if(i == 0) {
                printf("| %-*s", col_width, truncated);
                current_width += col_width;
            } else {
                printf(" | %-*s", col_width, truncated);
                current_width += col_width + 3;
            }
        }
        printf(" |\n");
        
        free(name);
        name = NULL;
        free(col_refer);
        col_refer = NULL;
        free(table_refer);
        table_refer = NULL;
    }
    printf("\n%d %s, %d %s in total.\n\n", table->col_count, table->col_count>1?"columns":"column", table->row_count,  table->row_count>1?"rows":"row");
}