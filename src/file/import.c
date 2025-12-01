/*
Date of creation : 27/11/2025
Description : Code for database import from file 
Group 2 ESGI 2A3
*/

/*
IMPORTANT:
    handle edge cases: empty database/table/hash table/hash node

NOTE: import structure for table to fwrite in order:
    table count

    # metadata for each table:
    strlen(tab_name) + 1 done
    Table name done

    Num of Cols
    Num of Rows
    Num of Hash Tables
    Next_id of table (int) done 
    
    Col one by one:
        name
        type
        constraint done
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
                IMPORTANT: for prev_row and row, write the row position (int) from first row (first row=0), not the pointer value. So if prev_row_index=0 => prev_row=first row
                strlen of original_value and original_value (str)
*/ 
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

#include "../../helper/db/db.h"
#include "../../include/db.h"
#include "../../include/ini.h"
#include "../../include/clean.h"
#include "../../include/global.h"

bool read_succeed(int read, int count, char* import_name){
    if(read != count){
        fprintf(stderr, "Importation error: unable to read from '%s', importation aborted.\n\n", import_name);
        fprintf(stderr, "Tip: please restart the program to try importing again.\n\n");
        return false;
    }
    return true;
}

void free_for_import_col(char** col_name, char** ref_tab_name, char** ref_col_name){
    free(*col_name); *col_name = NULL;
    free(*ref_tab_name); *ref_tab_name = NULL;
    free(*ref_col_name); *ref_col_name = NULL;
}

bool import_col(FILE* import_file, char* import_name,Table* table, Col** last_col){
    int read;
    int len_col_name;
    int len_ref_tab;
    int len_ref_col;
    char* col_name = NULL;
    char* ref_tab_name = NULL;
    char* ref_col_name = NULL;
    ColType col_type;
    ColConstraintType col_constraint;
    Col* new_col = NULL;

    // read col name
    read = fread(&len_col_name, sizeof(int), 1, import_file);
    if(!read_succeed(read, 1, import_name)) return false;

    col_name = (char*)malloc(sizeof(char) * len_col_name);
    assert(col_name != NULL);

    read = fread(col_name, sizeof(char), len_col_name, import_file);
    if(!read_succeed(read, len_col_name, import_name)) {
        free_for_import_col(&col_name, &ref_tab_name, &ref_col_name);
        return false;
    }

    //col_type
    read = fread(&col_type, sizeof(ColType), 1, import_file);
    if(!read_succeed(read, 1, import_name)) {
        free_for_import_col(&col_name, &ref_tab_name, &ref_col_name);
        return false;
    }
    // constraint
    read = fread(&col_constraint, sizeof(ColConstraintType), 1, import_file);
    if(!read_succeed(read, 1, import_name)) {
        free(col_name);
        col_name = NULL;
        return false;
    }

    // tab and col this col references (still read if it is 0 to consume the int)
    read = fread(&len_ref_tab, sizeof(int), 1, import_file);
    if(!read_succeed(read, 1, import_name)) {
        free_for_import_col(&col_name, &ref_tab_name, &ref_col_name);
        return false;
    }
    if(len_ref_tab != 0){
        ref_tab_name = (char*)malloc(sizeof(char) * len_ref_tab);
        assert(ref_tab_name != NULL);
        read = fread(ref_tab_name, sizeof(char), len_ref_tab, import_file);
        if(!read_succeed(read, len_ref_tab, import_name)) {
            free_for_import_col(&col_name, &ref_tab_name, &ref_col_name);
            return false;
        }
    }
    // col 
    read = fread(&len_ref_col, sizeof(int), 1, import_file);
    if(!read_succeed(read, 1, import_name)) {
            free_for_import_col(&col_name, &ref_tab_name, &ref_col_name);
        return false;
    }
    if(len_ref_col != 0){
        ref_col_name = (char*)malloc(sizeof(char) * len_ref_col);
        assert(ref_col_name != NULL);
        read = fread(ref_col_name, sizeof(char), len_ref_col, import_file);
        if(!read_succeed(read, len_ref_col, import_name)) {
            free_for_import_col(&col_name, &ref_tab_name, &ref_col_name);
            return false;
        }
    }

    // init and set col info
    new_col = init_col();

    new_col->name = strdup(col_name);
    assert(new_col->name!=NULL);
    new_col->type = col_type;
    new_col->constraint = col_constraint;

    if(ref_tab_name){
        new_col->refer_table = strdup(ref_tab_name);
        assert(new_col->refer_table!=NULL);
    }
    if(ref_col_name){
        new_col->refer_col = strdup(ref_col_name);
        assert(new_col->refer_col!=NULL);
    }

    // add to col linked list of table
    if (table->first_col == NULL || table->first_col->name == NULL) {
        // free the dummy if it exists
        if (table->first_col && table->first_col->name == NULL) free_col(table->first_col);
        table->first_col = new_col;
        *last_col = table->first_col;
    } else {
        // append to the end
        (*last_col)->next_col = new_col;
        *last_col = new_col;
    }

    free_for_import_col(&col_name, &ref_tab_name, &ref_col_name);
    return true;
}

void free_for_import_row(int*** int_list, int int_count, double*** double_list, int double_count, char*** str_list, int str_count){
    int i;

    if(*int_list){
        for(i=0; i<int_count; i++){
            if((*int_list)[i]){
                free((*int_list)[i]);
                (*int_list)[i] = NULL;
            }
        }
        free(*int_list);
        *int_list = NULL;
    }
    if(*str_list){
        for(i=0; i<str_count; i++){
            if((*str_list)[i]){
                free((*str_list)[i]);
                (*str_list)[i] = NULL;
            }
        }
        free(*str_list);
        *str_list = NULL;
    }
    if(*double_list){
        for(i=0; i<double_count; i++){
            if((*double_list)[i]){
                free((*double_list)[i]);
                (*double_list)[i] = NULL;
            }
        }
        free(*double_list);
        *double_list = NULL;
    }
}

bool import_row(FILE* import_file, char* import_name, Table* table, Row** last_row){
    int read;
    int int_count;
    int double_count;
    int str_count;
    int** int_list = NULL;
    double** double_list = NULL;
    char** str_list = NULL;
    int i;
    unsigned char null_marker;
    Row* new_row = NULL;

    // read counts
    read = fread(&int_count, sizeof(int), 1, import_file);
    if(!read_succeed(read, 1, import_name)) return false;
    read = fread(&double_count, sizeof(int), 1, import_file);
    if(!read_succeed(read, 1, import_name)) return false;
    read = fread(&str_count, sizeof(int), 1, import_file);
    if(!read_succeed(read, 1, import_name)) return false;

    // calloc data lists
    int_list = (int**)calloc(int_count, sizeof(int*));
    if(int_count!=0) assert(int_list!=NULL);
    double_list = (double**)calloc(double_count, sizeof(double*));
    if(double_count!=0) assert(double_list!=NULL);
    str_list = (char**)calloc(str_count, sizeof(char*));
    if(str_count!=0) assert(str_list!=NULL);
    
    // int list
    for(i=0; i<int_count; i++){
        read = fread(&null_marker, sizeof(unsigned char), 1, import_file);
        if(!read_succeed(read, 1, import_name)){
            free_for_import_row(&int_list, int_count, &double_list, double_count, &str_list, str_count);
            return false;
        }
        if(null_marker != 0){
            int_list[i] = (int*)malloc(sizeof(int));
            assert(int_list[i] != NULL);
            read = fread(int_list[i], sizeof(int), 1, import_file);
            if(!read_succeed(read, 1, import_name)){
                free_for_import_row(&int_list, int_count, &double_list, double_count, &str_list, str_count);
                return false;
            }
        }
    }

    // double list
    for(i=0; i<double_count; i++){
        read = fread(&null_marker, sizeof(unsigned char), 1, import_file);
        if(!read_succeed(read, 1, import_name)){
            free_for_import_row(&int_list, int_count, &double_list, double_count, &str_list, str_count);
            return false;
        }
        if(null_marker != 0){
            double_list[i] = (double*)malloc(sizeof(double));
            assert(double_list[i] != NULL);
            read = fread(double_list[i], sizeof(double), 1, import_file);
            if(!read_succeed(read, 1, import_name)){
                free_for_import_row(&int_list, int_count, &double_list, double_count, &str_list, str_count);
                return false;
            }
        }
    }

    int len_str_item;
    // str list
    for(i=0; i<str_count; i++){
        read = fread(&null_marker, sizeof(unsigned char), 1, import_file);
        if(!read_succeed(read, 1, import_name)){
            free_for_import_row(&int_list, int_count, &double_list, double_count, &str_list, str_count);
            return false;
        }
        if(null_marker != 0){
            read = fread(&len_str_item, sizeof(int), 1, import_file);
            if(!read_succeed(read, 1, import_name)){
                free_for_import_row(&int_list, int_count, &double_list, double_count, &str_list, str_count);
                return false;
            }
            str_list[i] = (char*)malloc(sizeof(char) * len_str_item);
            assert(str_list[i] != NULL);
            read = fread(str_list[i], sizeof(char), len_str_item, import_file);
            if(!read_succeed(read, len_str_item, import_name)){
                free_for_import_row(&int_list, int_count, &double_list, double_count, &str_list, str_count);
                return false;
            }
        }
    }
    
    // set data lists to row
    new_row = init_row();
    new_row->int_list = int_list;
    new_row->double_list = double_list;
    new_row->str_list = str_list;

    // set pointer
    if (!table->first_row) {
        table->first_row = new_row;
        *last_row = table->first_row;
    } else {
        // append to the end
        (*last_row)->next_row = new_row;
        *last_row = new_row;
    }

    return true;
}

bool import_hash_node(FILE* import_file, char* import_name, Table* table, HashTable* ht, int key){
    int read;
    int prev_row_index;
    int row_index;
    int len_val;
    char* val = NULL;
    Node* new_node = NULL;
    int i;
    Row* current_row = NULL;

    // prev row index
    read = fread(&prev_row_index, sizeof(int), 1, import_file);
    if(!read_succeed(read, 1, import_name)) return false; 
    // row index
    read = fread(&row_index, sizeof(int), 1, import_file);
    if(!read_succeed(read, 1, import_name)) return false; 

    // original val len
    read = fread(&len_val, sizeof(int), 1, import_file);
    if(!read_succeed(read, 1, import_name)) return false; 
    // original val
    val = (char*)malloc(sizeof(char) * len_val);
    assert(val!=NULL);
    read = fread(val, sizeof(char), len_val, import_file);
    if(!read_succeed(read, len_val, import_name)) {
        free(val);
        val = NULL;
        return false;
    }

    // init and set hash node
    new_node = init_node();

    // prev row and row (first_row index = 0 when export)
    if(prev_row_index==-1){
        new_node->prev_row = NULL;
        new_node->row = table->first_row;
    }
    else{
        current_row = table->first_row;
        for (i = 0; i < prev_row_index; i++) {
            current_row = current_row->next_row;
        }
        new_node->prev_row = current_row;
        new_node->row = current_row->next_row;
    }

    // set pointer of node
    if(!ht->bucket[key]){
        ht->bucket[key] = new_node;
    }else{
        // insert at head
        new_node->next_node = ht->bucket[key];
        ht->bucket[key] = new_node;
    }

    // original val
    new_node->original_value = strdup(val);
    assert(new_node->original_value!=NULL);

    free(val);
    val = NULL;
    return true;
}

bool import_hash_table(FILE* import_file, char* import_name, Table* table, HashTable** last_ht){
    int read;
    int len_col_name;
    int node_count;
    char* col_name = NULL;
    HashTable* new_ht = NULL;
    int i, j;

    // read col name
    read = fread(&len_col_name, sizeof(int), 1, import_file);
    if(!read_succeed(read, 1, import_name)) return false;

    col_name = (char*)malloc(sizeof(char) * len_col_name);
    assert(col_name != NULL);

    read = fread(col_name, sizeof(char), len_col_name, import_file);
    if(!read_succeed(read, len_col_name, import_name)){
        free(col_name);
        col_name = NULL;
        return false;
    }

    // init ht and set col name
    new_ht = init_hash_table();

    new_ht->col_name = strdup(col_name);
    assert(new_ht->col_name!=NULL);

    for(i=0; i<HASH_TABLE_SIZE; i++){
        // read number of nodes in current bucket
        read = fread(&node_count, sizeof(int), 1, import_file);
        if(!read_succeed(read, 1, import_name)){
            free(col_name);
            col_name = NULL;
            return false;
        }

        for(j=0; j<node_count; j++){
            if(!import_hash_node(import_file, import_name, table, new_ht, i)){
                free(col_name);
                col_name = NULL;
                return false;
            }
        }
    }

    // add ht to table
    if (table->first_hash_table == NULL || table->first_hash_table->col_name == NULL) {
        // free the dummy if it exists
        if (table->first_hash_table && table->first_hash_table->col_name == NULL) free_hash_table(table->first_hash_table);
        table->first_hash_table = new_ht;
        *last_ht = table->first_hash_table;
    } else {
        // append to the end
        (*last_ht)->next_hash_table = new_ht;
        *last_ht = new_ht;
    }

    free(col_name);
    col_name = NULL;
    return true;
}

bool import_table(FILE* import_file, char* import_name){
    int read;
    int len_tab_name;
    int col_count;
    int row_count;
    int ht_count;
    int next_id;
    int i;
    char* tab_name = NULL;
    Table* new_tab = NULL;
    Table* current_tab = NULL;
    Col* last_col = NULL;
    Row* last_row = NULL;
    HashTable* last_ht = NULL;

    // tab name
    read = fread(&len_tab_name, sizeof(int), 1, import_file);
    if(!read_succeed(read, 1, import_name)) {
        printf("Failed reading tab name len");
        return false;
    }

    tab_name = (char*)malloc(sizeof(char) * len_tab_name);
    assert(tab_name != NULL);

    read = fread(tab_name, sizeof(char), len_tab_name, import_file);
    if(!read_succeed(read, len_tab_name, import_name)){
        free(tab_name);
        tab_name = NULL;
        printf("Failed reading tab name");
        return false;
    }

    // num of col
    read = fread(&col_count, sizeof(int), 1, import_file);
    if(!read_succeed(read, 1, import_name)){
        free(tab_name);
        printf("Failed reading col num");
        tab_name = NULL;
        return false;
    }
    // num of row
    read = fread(&row_count, sizeof(int), 1, import_file);
    if(!read_succeed(read, 1, import_name)){
        free(tab_name);
        printf("Failed reading row num");
        tab_name = NULL;
        return false;
    }
    // num of ht
    read = fread(&ht_count, sizeof(int), 1, import_file);
    if(!read_succeed(read, 1, import_name)){
        free(tab_name);
        tab_name = NULL;
        printf("Failed reading ht num");
        return false;
    }
        // next_id
        read = fread(&next_id, sizeof(int), 1, import_file);
        if(!read_succeed(read, 1, import_name)){
        free(tab_name);
        tab_name = NULL;
        printf("Failed reading next id");
        return false;
    }

    // init table and set info read above
    new_tab = init_table();

    new_tab->name = strdup(tab_name);
    assert(new_tab->name!=NULL);
    new_tab->col_count = col_count;
    new_tab->row_count = row_count;
    new_tab->hash_table_count = ht_count;
    new_tab->next_id = next_id;

    // import cols
    for(i=0; i<col_count; i++){
        if(!import_col(import_file, import_name, new_tab, &last_col)){
            free(tab_name);
            tab_name = NULL;
        printf("Failed reading cols");
            return false;
        }
    }

    // import rows
    for(i=0; i<row_count; i++){
        if(!import_row(import_file, import_name, new_tab, &last_row)){
            free(tab_name);
            tab_name = NULL;
        printf("Failed reading rows");
            return false;
        }
    }

    // import hash tables
    for(i=0; i<ht_count; i++){
        if(!import_hash_table(import_file, import_name, new_tab, &last_ht)){
            free(tab_name);
            tab_name = NULL;
        printf("Failed reading ht");
            return false;
        }
    }

    // add table to linked list
    if(!first_table) first_table = new_tab;
    else{
        current_tab = get_last_table(first_table);
        current_tab->next_table = new_tab;
    }

    free(tab_name);
    tab_name = NULL;
    return true;
}

void import_db(char* import_name){
    FILE* import_file = NULL;
    int read;
    int table_count;
    int i;
    char* extension = ".bin";
    char* filename_with_ext = NULL;

    //add the .bin extension
    filename_with_ext = malloc(strlen(import_name) + strlen(extension) + 1);
    assert(filename_with_ext != NULL);
    strcpy(filename_with_ext, import_name);
    strcat(filename_with_ext, extension);

    import_file = fopen(filename_with_ext, "rb");
    if(!import_file){
        fprintf(stderr, "Importation error: unable to read from '%s', importation aborted.\n\n", filename_with_ext);
        fprintf(stderr, "Tip: please restart the program to try importing again.\n\n");
        free(filename_with_ext);
        filename_with_ext = NULL;
        return;
    }

    // read metadata
    // table count
    read = fread(&table_count, sizeof(int), 1, import_file);
    if(!read_succeed(read, 1, filename_with_ext)){
        free(filename_with_ext);
        filename_with_ext = NULL;
        fclose(import_file);
        return;
    }

    // read tables
    for(i=0; i<table_count; i++){
        if(!import_table(import_file, filename_with_ext)){
            free_db(first_table);
            first_table = NULL;
            free(filename_with_ext);
            filename_with_ext = NULL;
            fclose(import_file);
            return;
        }
    }

    fclose(import_file);
    printf("Database imported from '%s' successfully.\n\n", filename_with_ext);
    free(filename_with_ext);
    filename_with_ext = NULL;
}
