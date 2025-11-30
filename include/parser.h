
/*
Date of creation : 26/10/2025
Description : Declaration of prototypes for parser.c, Query struct
Group 2 ESGI 2A3
*/

#ifndef PARSER_H
#define PARSER_H

#define MAX_BATCH_SIZE 10000
#define MAX_CMD_SIZE 2048
#define TABLE_NAME_MAX 101
#define MAX_FILE_NAME 255

typedef enum ColType ColType;
typedef enum ColConstraintType ColConstraintType;

typedef enum CommandType{
    CREATE,
    INSERT,
    DELETE,
    SELECT,
    DROP,
    SHOW,
    DESCRIBE,
    EXIT,
    INVALID
} CommandType;

typedef struct DescribeParams{
    char* table_name;
} DescribeParams;

typedef struct{
    char* table_name;

    char **col_list; // list of column names passed in the query
    int col_count; // number of columns

    ColType *type_list;  // list of types corresponding order of col_list
    ColConstraintType *constraint_list; // list of constraint corresponding to order of col_list

    char** table_refer_list; //list of tables refered to by fk cols, doesnt follow order of col_list
    char** col_refer_list; // list of cols of refered tables refered to by fk cols, doesnt follow order of col_list
    int fk_count; // number of fk to free 2 lists above, done in clean.c
} CreateParams;

typedef struct{
    char* table_name;

    char **col_list; // list of column names passed in the query
    int col_count;          // number of columns to free col_list and

    char **data_list; // list of input for each column, saved as string in rax form, process later in insert db
} InsertParams;

typedef struct {
    char* table_name;
    char* condition_column; 
    char* condition_value; 
} DeleteParams;

typedef struct SelectParams{
    char* table_name; 
    char* table_join_name; 

    char** col_list;
    int col_count; //to free col_list
    char* first_col_on; // from tab1
    char* second_col_on; // join tab2
    char* condition_col; 
    char* condition_val; 
} SelectParams;

typedef struct {
    char** table_list;
    int table_count; // to free table_list
} DropParams;

typedef struct Query{
    CommandType cmd_type; 

    // take one of these params based on cmd_type
    union{
        CreateParams create_params;
        InsertParams insert_params;
        DeleteParams delete_params;
        DropParams drop_params;
        SelectParams select_params;
        DescribeParams describe_params;
    } params;
} Query;


// char* read_cmd(char* cmd_buffer);
char* read_file_name(char* file_buffer);

char** split_commands(char* batch, int* cmd_count);
char* read_batch_cmd(char* batch_buffer);
Query* parse_cmd(char* cmd);

void parse_delete(Query** query);
void parse_drop(Query** query);
void parse_select(Query** query);
void parse_insert(Query** query);
void parse_create(Query** query);
void parse_show(Query** query);
void parse_describe(Query** query);

#endif


