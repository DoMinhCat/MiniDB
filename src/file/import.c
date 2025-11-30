/*
Date of creation : 27/11/2025
Description : Code for database import from file 
Group 2 ESGI 2A3
*/

/*
IMPORTANT:
    handle edge cases: empty database/table/hash table/hash node

NOTE: export structure for table to fwrite in order:
    table count

    # metadata for each table:
    strlen(tab_name) + 1
    Table name

    Num of Cols
    Num of Rows
    Num of Hash Tables

    Next_id of table (int)
    Col one by one:
        name
        type
        constraint
        strlen of refer_table
        refer_table (str)   
        strlen of refer_col
        refer_col (str)

    Row one by one:
        int_count
        double_count
        str_count

        IMPORTANT: before each item is a char 1 or 0 to indicate if it is null or not
        example: 0|1value

        int items: null marker then item
        double items: null marker then item
        str items: null marker, string len + 1 then item

    Write hash table one by one:
        strlen of col_name, col_name

        write 67 buckets one by one:
            Loop to count then write num of nodes that handle collision
            write each collision node:
                IMPORTANT: for prev_row and row, write the row position (int) from first row (first row=1), not the pointer value. So if prev_row_index=0 => prev_row=NULL
                strlen of original_value and original_value (str)
*/ 
#include <stdio.h>
#include <stdbool.h>

#include "../db/db.h"

bool read_succeed(int read, int count, char* import_name){
    if(read != count){
        fprintf(stderr, "Importation error: unable to read from '%s', importation aborted.\n\n", import_name);
        fprintf(stderr, "Tip: please restart the program to try importing again.\n\n");
        return false;
    }
    return true;
}

void import_db(char* import_name){
    FILE* import_file = NULL;
    int read;
    int table_count;
    int col_count;
    int row_count;
    int i;

    import_file = fopen(import_name, "rb");
    if(!import_file){
        fprintf(stderr, "Importation error: unable to read from '%s', importation aborted.\n\n", import_name);
        fprintf(stderr, "Tip: please restart the program to try importing again.\n\n");
        return;
    }

    // read metadata
    // table count
    read = fread(&table_count, sizeof(int), 1, import_file);
    if(!read_succeed(read, 1, import_name)){
        fclose(import_file);
        return;
    }

    // read tables
    for(i=0; i<table_count; i++){
        
    }

}
