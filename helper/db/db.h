/*
Date of creation : 29/10/2025
Description : Helper functions protypes are declared here
Group 2 ESGI 2A3
*/
#include <stdbool.h>

#include "../../include/db.h"

#ifndef HELPER_DB_H
#define HELPER_DB_H

// get node of linked list functions
int* get_fk_col_list_index(Query* query);
// Col* get_last_col(Col* first_col);
Row* get_last_row(Row* first_row);
Row* get_prev_row(Table* table, Row* target);
Table* get_last_table(Table* first_table);
// HashTable* get_last_hash_table(HashTable* first_ht);
int get_data_list_index(Table* table, char* col_name);
Table* get_table_by_name(char* table_name);
Col* get_col_by_name(Table* table, char* col_name);
HashTable* get_ht_by_col_name(HashTable* first_ht, char* col_name); 

// check functions
bool table_exists(char* table_name);
bool col_exists(Table* table, char* col_name);
bool refer_val_exists(char* str_to_check, int val_to_check, char* ref_table_name, char* ref_col_name);
bool pk_value_is_unique(char* str_to_check, int val_to_check, HashTable* hash_tab, char* constraint);
int compare_double(double val1, double val2);
bool ref_integrity_check_delete(Table* table, FilteredRow* rows_to_del_fr, bool delete_all);

// functions for select
void* get_col_value(Table* table, Row* row, char* col_name, ColType col_type);
void format_value_to_string(ColType col_type, void* value, char* buffer, size_t buffer_size);

// type conversion
char* int_to_str(int val);
bool str_to_int(char *str_val, int *int_output, char *col_name);
bool str_to_double(char *str_val, double *double_output, char *col_name);

// functions for where & join
FilteredRow* copy_rows_to_filtered(Table* tab);
FilteredRow* copy_data_lists_to_filtered(Row* row1, Row* row2);
SelectedColInfo* build_col_info_list(Table* tab1, Table* tab2, SelectParams* params, int list_size);
void* get_col_value_for_join(FilteredRow* filtered_set, SelectedColInfo col_info);
bool str_to_col_type(Col* condition_col, char* condition_val, int* int_val, double* double_val, char** str_val);

#endif
