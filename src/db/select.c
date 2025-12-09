/*
last update : 16/11/2025
Description : Code for select_data function
Group 2 ESGI 2A3
*/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>

#include "../../helper/db/db.h"
#include "../../include/db.h"
#include "../../helper/format/format.h"
#include "../../include/parser.h"
#include "../../include/ini.h"
#include "../../include/clean.h"
#include "../../include/hash.h"
#include "../../include/global.h"

void print_header_row(Table* table1, Table* table2, SelectParams* params) {
    bool select_all = params->col_count == 1 && strcmp(params->col_list[0], "*") == 0;
    Col* current_col = NULL;
    int col_count = 0;
    int col_width;
    int current_width = 1;  
    bool truncated_cols = false;

     
    if (select_all) {
        col_count = table1->col_count;
        if (table2) {
            col_count += table2->col_count;
        }
    } else {
        col_count = params->col_count;
    }

     
    col_width = calculate_col_width(col_count);

     
    printf("|");
    if (select_all) {
         
        for (current_col = table1->first_col; current_col != NULL; current_col = current_col->next_col) {
             
            if (current_width + col_width + 1 > MAX_TABLE_WIDTH) {
                printf(" ... |");
                truncated_cols = true;
                break;
            }
            print_cell(current_col->name, col_width);
            printf("|");
            current_width += col_width + 1;
        }
        if (table2 && !truncated_cols) {
            for (current_col = table2->first_col; current_col != NULL; current_col = current_col->next_col) {
                if (current_width + col_width + 1 > MAX_TABLE_WIDTH) {
                    printf(" ... |");
                    truncated_cols = true;
                    break;
                }
                print_cell(current_col->name, col_width);
                printf("|");
                current_width += col_width + 1;
            }
        }
    } else {
         
        for (int i = 0; i < params->col_count; i++) {
            if (current_width + col_width + 1 > MAX_TABLE_WIDTH) {
                printf(" ... |");
                truncated_cols = true;
                break;
            }
            print_cell(params->col_list[i], col_width);
            printf("|");
            current_width += col_width + 1;
        }
    }
    printf("\n");

     
    printf("|");
    current_width = 1;
    int printed_cols = 0;
    for (int i = 0; i < col_count; i++) {
        if (current_width + col_width + 1 > MAX_TABLE_WIDTH) {
            for (int j = 0; j < 5; j++) printf("-");
            printf("|");
            break;
        }
        for (int j = 0; j < col_width; j++) {
            printf("-");
        }
        printf("|");
        current_width += col_width + 1;
        printed_cols++;
    }
    printf("\n");
}

void print_empty_table(Table* table1, Table* table2, SelectParams* params) {
    bool select_all = params->col_count == 1 && strcmp(params->col_list[0], "*") == 0;
    int col_count;
    int col_width;

     
    if (select_all) {
        col_count = table1->col_count;
        if (table2) {
            col_count += table2->col_count;
        }
    } else {
        col_count = params->col_count;
    }

     
    col_width = calculate_col_width(col_count);

     
    printf("|");
    for (int i = 0; i < col_count; i++) {
        printf("%*s|", col_width, "");
    }
    printf("\n\n");
    printf("Found 0 row.\n\n");
}

void print_data(Table* table, Row* current_row, SelectParams* params) {
    Col* current_col = NULL;
    ColType col_type;
    bool select_all = params->col_count == 1 && strcmp(params->col_list[0], "*") == 0;
    int col_count = select_all ? table->col_count : params->col_count;
    int col_width = calculate_col_width(col_count);
    int current_width = 1;  
    
    char value_str[MAX_TABLE_WIDTH];

    printf("|");
    if (select_all) {
         
        for (current_col = table->first_col; current_col != NULL; current_col = current_col->next_col) {
             
            if (current_width + col_width + 1 > MAX_TABLE_WIDTH) {
                printf(" ... |");
                break;
            }
            
            col_type = current_col->type;
            void* value = get_col_value(table, current_row, current_col->name, col_type);
            
             
            format_value_to_string(col_type, value, value_str, sizeof(value_str));
            print_cell(value_str, col_width);
            printf("|");
            current_width += col_width + 1;
        }
    } else {
         
        for (int i = 0; i < params->col_count; i++) {
            if (current_width + col_width + 1 > MAX_TABLE_WIDTH) {
                printf(" ... |");
                break;
            }
            
            col_type = get_col_by_name(table, params->col_list[i])->type;
            void* value = get_col_value(table, current_row, params->col_list[i], col_type);
            
            format_value_to_string(col_type, value, value_str, sizeof(value_str));
            print_cell(value_str, col_width);
            printf("|");
            current_width += col_width + 1;
        }
    }
    printf("\n");
}

void print_data_for_join(FilteredRow* filtered_row, SelectedColInfo* col_info, SelectParams* params) {
    bool select_all = params->col_count == 1 && strcmp(params->col_list[0], "*") == 0;
    int col_count;
    int col_width;
    int current_width = 1;  
    char value_str[MAX_TABLE_WIDTH];

     
    if (select_all) {
        col_count = get_table_by_name(params->table_name)->col_count + 
                    get_table_by_name(params->table_join_name)->col_count;
    } else {
        col_count = params->col_count;
    }

    col_width = calculate_col_width(col_count);

    printf("|");
    for (int i = 0; i < params->col_count; i++) {
         
        if (current_width + col_width + 1 > MAX_TABLE_WIDTH) {
            printf(" ... |");
            break;
        }
        
        void* value = get_col_value_for_join(filtered_row, col_info[i]);
        
        format_value_to_string(col_info[i].type, value, value_str, sizeof(value_str));
        print_cell(value_str, col_width);
        printf("|");
        current_width += col_width + 1;
    }
    printf("\n");
}

void select_simple(SelectParams* params, Table* table){
     

    print_header_row(table, NULL, params);

     
    Row* current_row = table->first_row;
    int row_count = 0;

     
    if(!table->first_row){
        print_empty_table(table, NULL, params);
        return;
    }

     
    while (current_row != NULL) {
        print_data(table, current_row, params);
        
        row_count++;
        current_row = current_row->next_row;
    }

     
    printf("\nFound %d %s.\n\n", row_count, row_count>1?"rows":"row");
}

void select_where_only(SelectParams* params, Table* table){
     
    
    FilteredRow* filtered = NULL;
    Col* condition_col = get_col_by_name(table, params->condition_col);
    char* condition_val = params->condition_val;
    ColType col_type = condition_col->type;
    Row* current_row = NULL;
    int row_count = 0;

    int int_val;
    double double_val;
    char* str_val = NULL;
    
     
     
    if(strcasecmp(condition_val, "NULL") == 0){
        filtered = where_for_select(table, condition_col, "NULL", 0, 0, col_type);
    } else{
         
        if(!str_to_col_type(condition_col, condition_val, &int_val, &double_val, &str_val)) return;
        filtered = where_for_select(table, condition_col, str_val, double_val, int_val, col_type);
    }

     
    print_header_row(table, NULL, params);
    if(!filtered){
        print_empty_table(table, NULL, params);
        return;
    }

    while(filtered){
        current_row = filtered->row;
        print_data(table, current_row, params);

        row_count++;
        filtered = filtered->next_filtered_row;
    }

     
    free_filtered_set(filtered);
    printf("\nFound %d %s.\n\n", row_count, row_count>1?"rows":"row");
}

void select_join_where(SelectParams* params, Table* tab, Table* condition_tab, Col* col_tab, Col* col_tab_where, SelectedColInfo* col_info){
     

    FilteredRow* filtered_where = NULL;
    FilteredRow* result = NULL;
    FilteredRow* current_fr = NULL;
    Col* condition_col = NULL;
    char* condition_val = params->condition_val;
    int int_val;
    double double_val;
    char* str_val = NULL;
    int row_count = 0;
    bool where_tab_is_first = strcmp(params->table_name, condition_tab->name) == 0; 
    Table* tab1 = NULL;
    Table* tab2 = NULL;

     
    if(where_tab_is_first){
        tab1 = condition_tab;
        tab2 = tab;
    }else{
        tab2 = condition_tab;
        tab1 = tab;
    }
    
     
    condition_col = get_col_by_name(condition_tab, params->condition_col);

     
    if(strcasecmp(condition_val, "NULL") == 0){
        filtered_where = where_for_select(condition_tab, condition_col, "NULL", 0, 0, condition_col->type);
    } else{
         
        if(!str_to_col_type(condition_col, condition_val, &int_val, &double_val, &str_val)) return;
        filtered_where = where_for_select(condition_tab, condition_col, str_val, double_val, int_val, condition_col->type);
    }

     
    result = join_with_where(filtered_where, tab, condition_tab, col_tab, col_tab_where, params);

    print_header_row(tab1, tab2, params);
    if(!result){
        print_empty_table(tab1, tab2, params);
        return;
    }

     
    for(current_fr=result; current_fr!=NULL; current_fr=current_fr->next_filtered_row){
        print_data_for_join(current_fr, col_info, params);
        row_count++;
    }

     
    free_filtered_set(result);
    printf("\nFound %d %s.\n\n", row_count, row_count>1?"rows":"row");
}

void select_join_only(Table* tab1, Table* tab2, SelectParams* params, SelectedColInfo* col_info, Col* col1, Col* col2){
     

    FilteredRow* filtered = join(tab1, tab2, col1, col2, params);
    FilteredRow* current_fr = NULL;
    
    int row_count = 0;

    print_header_row(tab1, tab2, params);
    if(!filtered){
        print_empty_table(tab1, tab2, params);
        return;
    }

     
    for(current_fr=filtered; current_fr!=NULL; current_fr=current_fr->next_filtered_row){
        print_data_for_join(current_fr, col_info, params);
        row_count++;
    }

    free_filtered_set(filtered);
    printf("\nFound %d %s.\n\n", row_count, row_count>1?"rows":"row");
}

void select(Query* query) {
     

    SelectParams* params = &query->params.select_params;
    bool select_all = params->col_count == 1 && strcmp(params->col_list[0], "*") == 0;
    bool include_join = false;
    bool include_where = false;
    Table* table = NULL;
    Table* join_table = NULL;
    Table* where_table = NULL;
    Table* non_where_table = NULL;
    Col* col_on1 = NULL;
    Col* col_on2 = NULL;
    Col* col_on_where = NULL;
    Col* col_on_no_where = NULL;
    int i;
    SelectedColInfo* output_col_info = NULL;
    int col_info_list_size;

     
    table = get_table_by_name(params->table_name);
    if (table == NULL) {
        fprintf(stderr, "Execution error: '%s' table not found.\n\n", params->table_name);
        return;
    }

     
    if(params->table_join_name){
        include_join = true;
        
         
        join_table = get_table_by_name(params->table_join_name);
        if(!join_table){
            fprintf(stderr, "Execution error: '%s' table not found.\n\n", params->table_join_name);
            return;
        }
         
        col_on1 = get_col_by_name(table, params->first_col_on);
        col_on2 = get_col_by_name(join_table, params->second_col_on);
        if(!col_on1 || !col_on2) return;
         
        if(col_on1->type != col_on2->type){
            fprintf(stderr, "Execution error: '%s' and '%s' columns have different types.\n\n", col_on1->name, col_on2->name);
            return;
        }
    }

     
    if (params->col_count > 0 && !select_all) {
        for (i = 0; i < params->col_count; i++) {
            if (!get_col_by_name(table, params->col_list[i])) {
                if(include_join){
                    if(!get_col_by_name(join_table, params->col_list[i])){
                        fprintf(stderr, "Execution error: '%s' column  not found.\n\n", params->col_list[i]);
                        return;
                    }
                }else{
                    fprintf(stderr, "Execution error: '%s' column  not found.\n\n", params->col_list[i]);
                    return;
                }
            }
        }
    }

     
    if (params->condition_col) {
        include_where = true;

        where_table = table;
        non_where_table = join_table;
        col_on_where = col_on1;
        col_on_no_where = col_on2;
         
        if (!get_col_by_name(table, params->condition_col)) {
            if (include_join) {
                if (!get_col_by_name(join_table, params->condition_col)) {
                    fprintf(stderr, "Execution error: '%s' column  not found.\n\n", params->condition_col);
                    return;
                }
                where_table = join_table;
                non_where_table = table;
                col_on_where = col_on2;
                col_on_no_where = col_on1;
            } else{
                fprintf(stderr, "Execution error: '%s' column  not found.\n\n", params->condition_col);
                return;
            }
        }
    }
    printf("\n");

     
    if (include_join && include_where) {
        if(select_all) col_info_list_size = table->col_count + join_table->col_count;
        else col_info_list_size = params->col_count;
        output_col_info = build_col_info_list(table, join_table, params, col_info_list_size);
        select_join_where(params,non_where_table, where_table, col_on_no_where, col_on_where, output_col_info);
        free(output_col_info);
        return;
    }
    if (include_join) {
        if(select_all) col_info_list_size = table->col_count + join_table->col_count;
        else col_info_list_size = params->col_count;
        output_col_info = build_col_info_list(table, join_table, params, col_info_list_size);

        select_join_only(table, join_table, params, output_col_info, col_on1, col_on2);
         
        free(output_col_info);
         
        return;
    }
    if (include_where) {
        select_where_only(params, table);
         
        return;
    }
    select_simple(params, table);
}
