/*
Date of creation : 17/10/2025
Description : All structures of database and db prototypes are declared here
Group 2 ESGI 2A3
*/

#ifndef DB_H
#define DB_H

#define MAX_TABLE_COUNT 100
#define MAX_COL_COUNT 50
#define MAX_STR_LEN 256

#include <stdbool.h>

#include "parser.h"

typedef struct HashTable HashTable;
typedef struct Query Query;
typedef struct SelectParams SelectParams;

typedef enum ColConstraintType{
    NONE,
    FK,
    PK,
    UNIQUE
} ColConstraintType;

typedef enum ColType{
    INT,
    DOUBLE,
    STRING    
} ColType;

typedef struct Row{
    int **int_list;  
    char **str_list;
    double **double_list;  

    int int_count;
    int str_count;
    int double_count;

    struct Row *next_row;
} Row;

typedef struct Col{
    char *name;
    ColType type;
    ColConstraintType constraint;

    char *refer_table;
    char *refer_col;

    struct Col *next_col;
} Col;

typedef struct Table{
    char *name;
    Row *first_row;
    Col *first_col;
    HashTable *first_hash_table;
    int next_id;

    int col_count;
    int row_count;
    int hash_table_count;

    struct Table *next_table;
} Table;

typedef struct FilteredRow{
    Row* row;

    int** int_joined_list;
    char** str_joined_list;
    double** double_joined_list;
    int int_join_count;
    int double_join_count;
    int str_join_count;

    struct FilteredRow* next_filtered_row;
} FilteredRow;

 
typedef struct SelectedColInfo{
    ColType type;    
    int table_id; 
    int data_index;   
} SelectedColInfo;

void create_table(Query *query);
void drop_table(Query *query);
void insert(Query *query);
void delete_from_table(Query* query);
void describe_table(Query *query);
void show();

void select(Query* query);
void select_simple(SelectParams* params, Table* table);

FilteredRow* where_for_select(Table* table, Col* condition_col, char* str_condition, double double_condition, int int_condition, ColType col_type);
FilteredRow* join(Table* tab1, Table* tab2, Col* col1, Col* col2, SelectParams* params);
FilteredRow* join_with_where(FilteredRow* head_list_where, Table* tab, Table* tab_where, Col* col_tab, Col* col_tab_where, SelectParams* params);

#endif
