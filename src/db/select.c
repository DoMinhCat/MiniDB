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
    int current_width = 1; // Start with 1 for initial "|"
    bool truncated_cols = false;

    // Calculate total column count
    if (select_all) {
        col_count = table1->col_count;
        if (table2) {
            col_count += table2->col_count;
        }
    } else {
        col_count = params->col_count;
    }

    // Calculate column width
    col_width = calculate_col_width(col_count);

    // Print column headers
    printf("|");
    if (select_all) {
        // SELECT *
        for (current_col = table1->first_col; current_col != NULL; current_col = current_col->next_col) {
            // Check if adding this column exceeds max width
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
        // SELECT col1, col2, ...
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

    // Print separator line (matching the actual printed columns)
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

    // Calculate column count
    if (select_all) {
        col_count = table1->col_count;
        if (table2) {
            col_count += table2->col_count;
        }
    } else {
        col_count = params->col_count;
    }

    // Calculate column width
    col_width = calculate_col_width(col_count);

    // Print empty row
    printf("|");
    for (int i = 0; i < col_count; i++) {
        printf("%*s|", col_width, "");
    }
    printf("\n\n");
    printf("Found 0 row.\n");
}

void print_data(Table* table, Row* current_row, SelectParams* params) {
    Col* current_col = NULL;
    ColType col_type;
    bool select_all = params->col_count == 1 && strcmp(params->col_list[0], "*") == 0;
    int col_count = select_all ? table->col_count : params->col_count;
    int col_width = calculate_col_width(col_count);
    int current_width = 1; // Start with 1 for initial "|"
    
    char value_str[MAX_TABLE_WIDTH];

    printf("|");
    if (select_all) {
        // Print all columns in the right order
        for (current_col = table->first_col; current_col != NULL; current_col = current_col->next_col) {
            // Check if adding this column exceeds max width
            if (current_width + col_width + 1 > MAX_TABLE_WIDTH) {
                printf(" ... |");
                break;
            }
            
            col_type = current_col->type;
            void* value = get_col_value(table, current_row, current_col->name, col_type);
            
            // Format value to string
            format_value_to_string(col_type, value, value_str, sizeof(value_str));
            print_cell(value_str, col_width);
            printf("|");
            current_width += col_width + 1;
        }
    } else {
        // SELECT col1, col2: print the right columns
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
    int current_width = 1; // Start with 1 for initial "|"
    char value_str[MAX_TABLE_WIDTH];

    // Calculate column count
    if (select_all) {
        col_count = get_table_by_name(params->table_name)->col_count + 
                    get_table_by_name(params->table_join_name)->col_count;
    } else {
        col_count = params->col_count;
    }

    col_width = calculate_col_width(col_count);

    printf("|");
    for (int i = 0; i < params->col_count; i++) {
        // Check if adding this column exceeds max width
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
    // select without JOIN and WHERE: SELECT */col1, col2,... from tab1

    print_header_row(table, NULL, params);

    // starts printing data
    Row* current_row = table->first_row;
    int row_count = 0;

    // if table is empty
    if(!table->first_row){
        print_empty_table(table, NULL, params);
        return;
    }

    // table is not empty
    while (current_row != NULL) {
        print_data(table, current_row, params);
        
        row_count++;
        current_row = current_row->next_row;
    }

    // print number of rows
    printf("\nFound %d %s.\n", row_count, row_count>1?"rows":"row");
}

void select_where_only(SelectParams* params, Table* table){
    // select with WHERE
    
    FilteredRow* filtered = NULL;
    Col* condition_col = get_col_by_name(table, params->condition_col);
    char* condition_val = params->condition_val;
    ColType col_type = condition_col->type;
    Row* current_row = NULL;
    int row_count = 0;

    int int_val;
    double double_val;
    char* str_val = NULL;
    
    // NOTE: could make a function for "type conversion" if repititive across select cases
    // special case compare with NULL
    if(strcasecmp(condition_val, "NULL") == 0){
        filtered = where_for_select(table, condition_col, "NULL", 0, 0, col_type);
    } else{
        // convert condition value to the correct type to compare
        if(!str_to_col_type(condition_col, condition_val, &int_val, &double_val, &str_val)) return;
        filtered = where_for_select(table, condition_col, str_val, double_val, int_val, col_type);
    }

    // print results
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

    // free before exit
    free_filtered_set(filtered);
    printf("\nFound %d %s.\n", row_count, row_count>1?"rows":"row");
}

void select_join_where(SelectParams* params, Table* tab, Table* condition_tab, Col* col_tab, Col* col_tab_where, SelectedColInfo* col_info){
    // SELECT with WHERE and JOIN

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

    // determine tab and col order to print in correct order
    if(where_tab_is_first){
        tab1 = condition_tab;
        tab2 = tab;
    }else{
        tab2 = condition_tab;
        tab1 = tab;
    }
    
    // get condition column
    condition_col = get_col_by_name(condition_tab, params->condition_col);

    // filter rows by WHERE
    if(strcasecmp(condition_val, "NULL") == 0){
        filtered_where = where_for_select(condition_tab, condition_col, "NULL", 0, 0, condition_col->type);
    } else{
        // convert condition value to the correct type to compare
        if(!str_to_col_type(condition_col, condition_val, &int_val, &double_val, &str_val)) return;
        filtered_where = where_for_select(condition_tab, condition_col, str_val, double_val, int_val, condition_col->type);
    }

    // filter by JOIN, filtered_where freed by merge func in join_with_where
    result = join_with_where(filtered_where, tab, condition_tab, col_tab, col_tab_where, params);

    print_header_row(tab1, tab2, params);
    if(!result){
        print_empty_table(tab1, tab2, params);
        return;
    }

    // print results
    for(current_fr=result; current_fr!=NULL; current_fr=current_fr->next_filtered_row){
        print_data_for_join(current_fr, col_info, params);
        row_count++;
    }

    // IMPORTANT free filtered set after print, before return
    free_filtered_set(result);
    printf("\nFound %d %s.\n", row_count, row_count>1?"rows":"row");
}

void select_join_only(Table* tab1, Table* tab2, SelectParams* params, SelectedColInfo* col_info, Col* col1, Col* col2){
    // select with JOIN

    FilteredRow* filtered = join(tab1, tab2, col1, col2, params);
    FilteredRow* current_fr = NULL;
    
    int row_count = 0;

    print_header_row(tab1, tab2, params);
    if(!filtered){
        print_empty_table(tab1, tab2, params);
        return;
    }

    // print results
    for(current_fr=filtered; current_fr!=NULL; current_fr=current_fr->next_filtered_row){
        print_data_for_join(current_fr, col_info, params);
        row_count++;
    }

    free_filtered_set(filtered);
    printf("\nFound %d %s.\n", row_count, row_count>1?"rows":"row");
}

void select(Query* query) {
    // main function of select, perform checks and call to select of specfic cases

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

    // Make sure table exists
    table = get_table_by_name(params->table_name);
    if (table == NULL) {
        fprintf(stderr, "Execution error: '%s' table not found.\n\n", params->table_name);
        return;
    }

    // check join params
    if(params->table_join_name){
        include_join = true;
        
        // join table exists ?
        join_table = get_table_by_name(params->table_join_name);
        if(!join_table){
            fprintf(stderr, "Execution error: '%s' table not found.\n\n", params->table_join_name);
            return;
        }
        // join columns exist ?
        col_on1 = get_col_by_name(table, params->first_col_on);
        col_on2 = get_col_by_name(join_table, params->second_col_on);
        if(!col_on1 || !col_on2) return;
        // join columns same type?
        if(col_on1->type != col_on2->type){
            fprintf(stderr, "Execution error: '%s' and '%s' columns have different types.\n\n", col_on1->name, col_on2->name);
            return;
        }
    }

    //make sure all selected columns exist
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

    // check where params
    if (params->condition_col) {
        include_where = true;

        where_table = table;
        non_where_table = join_table;
        col_on_where = col_on1;
        col_on_no_where = col_on2;
        // default where column belongs to the first table, if not then search at the second table
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

    // call to select of each case
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
        // free dynamic col_output_info list
        free(output_col_info);
        // already freed filtered set in select_join_only
        return;
    }
    if (include_where) {
        select_where_only(params, table);
        // already freed filtered set in select_where_only
        return;
    }
    select_simple(params, table);
}
