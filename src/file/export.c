/*
Date of creation : 16/11/2025
Description : Code for database export to file 
Group 2 ESGI 2A3
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

#include "../../include/db.h"
#include "../../include/hash.h"
#include "../../include/global.h"

bool write_succeed(int written, int count, char* file_name){
    if(written != count){
        fprintf(stderr, "Exportation error: unable to write to '%s', exportation aborted.\n\n", file_name);
        return false;
    }
    return true;
}

bool export_col(FILE* output_file, char* output_file_name, Col* col){
    int written;
    int len_name = col->name ? strlen(col->name) + 1 : 0;
    int len_ref_tab = col->refer_table ? strlen(col->refer_table) + 1 : 0;
    int len_ref_col = col->refer_col ? strlen(col->refer_col) + 1 : 0;
    
    // len of col name
    written = fwrite(&len_name, sizeof(int), 1, output_file);
    if(!write_succeed(written, 1, output_file_name)) return false;
    // col name
    written = fwrite(col->name, sizeof(char), len_name, output_file);
    if(!write_succeed(written, len_name, output_file_name)) return false;

    // type
    written = fwrite(&col->type, sizeof(ColType), 1, output_file);
    if(!write_succeed(written, 1, output_file_name)) return false;
    // constraint
    written = fwrite(&col->constraint, sizeof(ColConstraintType), 1, output_file);
    if(!write_succeed(written, 1, output_file_name)) return false;

    // len of refer table
    written = fwrite(&len_ref_tab, sizeof(int), 1, output_file);
    if(!write_succeed(written, 1, output_file_name)) return false;
    // refer tab
    if(len_ref_tab!=0){
        written = fwrite(col->refer_table, sizeof(char), len_ref_tab, output_file);
        if(!write_succeed(written, len_ref_tab, output_file_name)) return false;
    }

    // len of refer col
    written = fwrite(&len_ref_col, sizeof(int), 1, output_file);
    if(!write_succeed(written, 1, output_file_name)) return false;
    // refer col
    if(len_ref_col!=0){
        written = fwrite(col->refer_col, sizeof(char), len_ref_col, output_file);
        if(!write_succeed(written, len_ref_col, output_file_name)) return false;    
    }

    return true;
}

bool export_row(FILE* output_file, char* output_file_name, Row* row){
    int written;
    int i;
    int len_str;
    unsigned char null_marker;

    // int count
    written = fwrite(&row->int_count, sizeof(int), 1, output_file);
    if(!write_succeed(written, 1, output_file_name)) return false;
    // double count
    written = fwrite(&row->double_count, sizeof(int), 1, output_file);
    if(!write_succeed(written, 1, output_file_name)) return false;
    // str count
    written = fwrite(&row->str_count, sizeof(int), 1, output_file);
    if(!write_succeed(written, 1, output_file_name)) return false;

    // int list
    for(i=0; i<row->int_count; i++){
        // null marker 
        null_marker = row->int_list[i] ? 1 : 0;
        written = fwrite(&null_marker, sizeof(unsigned char), 1, output_file);
        if(!write_succeed(written, 1, output_file_name)) return false;
        // write value if not null
        if(row->int_list[i]){
            written = fwrite(row->int_list[i], sizeof(int), 1, output_file);
            if(!write_succeed(written, 1, output_file_name)) return false;
        }
    }

    // double list
    for(i=0; i<row->double_count; i++){
        // null marker 
        null_marker = row->double_list[i] ? 1 : 0;
        written = fwrite(&null_marker, sizeof(unsigned char), 1, output_file);
        if(!write_succeed(written, 1, output_file_name)) return false;
        // write value if not null
        if(row->double_list[i]){
            written = fwrite(row->double_list[i], sizeof(double), 1, output_file);
            if(!write_succeed(written, 1, output_file_name)) return false;
        }
    }

    // string list
    for(i=0; i<row->str_count; i++){
        // null marker 
        null_marker = row->str_list[i] ? 1 : 0;
        written = fwrite(&null_marker, sizeof(unsigned char), 1, output_file);
        if(!write_succeed(written, 1, output_file_name)) return false;
        
        // write if not null
        if(row->str_list[i]){
            len_str = strlen(row->str_list[i]) + 1;
            // len of str item
            written = fwrite(&len_str, sizeof(int), 1, output_file);
            if(!write_succeed(written, 1, output_file_name)) return false;
            // string value
            written = fwrite(row->str_list[i], sizeof(char), strlen, output_file);
            if(!write_succeed(written, strlen, output_file_name)) return false;
        }
    }
    return true;
}

bool export_hash_node(FILE* output_file, char* output_file_name, Node* hash_node, Table* table){
    int prev_row_index = 0;
    int row_index;
    Row* current_row = NULL;
    int len_original_val;
    int written;

    // find index of row and prev row
    if(!hash_node->prev_row) prev_row_index=-1;
    else{
        for(current_row=table->first_row; current_row!=NULL; current_row=current_row->next_row){
            if(current_row==hash_node->prev_row) break;
            prev_row_index++;
        }
    }

    // write prev row index
    written = fwrite(&prev_row_index, sizeof(int), 1, output_file);
    if(!write_succeed(written, 1, output_file_name)) return false;
    row_index = prev_row_index+1;
    // write row index
    written = fwrite(&row_index, sizeof(int), 1, output_file);
    if(!write_succeed(written, 1, output_file_name)) return false;

    // original val len
    len_original_val = strlen(hash_node->original_value)+1;
    written = fwrite(&len_original_val, sizeof(int), 1, output_file);
    if(!write_succeed(written, 1, output_file_name)) return false;
    // original val
    written = fwrite(hash_node->original_value, sizeof(char), len_original_val, output_file);
    if(!write_succeed(written, len_original_val, output_file_name)) return false;

    return true;
}

bool export_hash_table(FILE* output_file, char* output_file_name, HashTable* ht, Table* table){
    int written;
    int len_col_name = strlen(ht->col_name)+1;
    int i;
    int node_count;
    Node* current_node = NULL;

    // col name len
    written = fwrite(&len_col_name, sizeof(int), 1, output_file);
    if(!write_succeed(written, 1, output_file_name)) return false;
    // col name
    written = fwrite(ht->col_name, sizeof(char), len_col_name, output_file);
    if(!write_succeed(written, len_col_name, output_file_name)) return false;

    // 67 buckets
    for(i=0; i<HASH_TABLE_SIZE; i++){
        node_count = 0;

        // number of nodes in the current bucket
        for(current_node = ht->bucket[i]; current_node!=NULL; current_node=current_node->next_node) node_count++;
        written = fwrite(&node_count, sizeof(int), 1, output_file);
        if(!write_succeed(written, 1, output_file_name)) return false;

        // write nodes of current bucket
        if(node_count != 0){
            for(current_node = ht->bucket[i]; current_node!=NULL; current_node=current_node->next_node){
                if(!export_hash_node(output_file, output_file_name, current_node, table)) return false;
            }
        }
    }
    return true;
}

bool export_table(FILE* output_file, char* output_file_name, Table* table){
    int written;
    int tab_name_len = strlen(table->name) + 1;
    int i;
    Col* current_col = NULL;
    HashTable* current_ht = NULL;
    Row* current_row = NULL;

    // write length of table name + 1 (for \0)
    written = fwrite(&tab_name_len, sizeof(int), 1, output_file);
    if(!write_succeed(written, 1, output_file_name)) return false;
    // name of table
    written = fwrite(table->name, sizeof(char), tab_name_len, output_file);
    if(!write_succeed(written, tab_name_len, output_file_name)) return false;

    // num of cols
    written = fwrite(&(table->col_count), sizeof(int), 1, output_file);
    if(!write_succeed(written, 1, output_file_name)) return false;
    // num of rows
    written = fwrite(&(table->row_count), sizeof(int), 1, output_file);
    if(!write_succeed(written, 1, output_file_name)) return false;
    // num of hash tables
    written = fwrite(&(table->hash_table_count), sizeof(int), 1, output_file);
    if(!write_succeed(written, 1, output_file_name)) return false;

    // next_id
    written = fwrite(&(table->next_id), sizeof(int), 1, output_file);
    if(!write_succeed(written, 1, output_file_name)) return false;

    // write columns
    current_col = table->first_col;
    for(i=0; i<table->col_count; i++){
        if(!export_col(output_file, output_file_name, current_col)) return false;
        current_col=current_col->next_col;
    }

    // write rows
    if(table->row_count != 0){
        // write rows
        current_row = table->first_row;
        for(i=0; i<table->row_count; i++){
            if(!export_row(output_file, output_file_name, current_row)) return false;
            current_row=current_row->next_row;
        }
    }

    // write hash tables
    if(table->hash_table_count != 0){
        current_ht = table->first_hash_table;
        for(i=0; i<table->hash_table_count; i++){
            if(!export_hash_table(output_file, output_file_name, current_ht, table)) return false;
            current_ht=current_ht->next_hash_table;
        }
    }

    return true;
}

void export_db(char* output_file_name, Table* first_table){
    // export the whole database to a binary file

    FILE* output_file = NULL;
    Table* current_table = NULL;
    char* extension = ".bin";
    char* filename_with_ext = NULL;
    int i;
    int written;

    //add the .bin extension
    filename_with_ext = malloc(strlen(output_file_name) + strlen(extension) + 1);
    assert(filename_with_ext != NULL);
    strcpy(filename_with_ext, output_file_name);
    strcat(filename_with_ext, extension);

    // open file to write
    output_file = fopen(filename_with_ext, "wb");
    if(!output_file){
        fprintf(stderr, "Exportation error: unable to write to '%s', exportation aborted.\n\n", filename_with_ext);
        free(filename_with_ext);
        filename_with_ext = NULL;
        return;
    }

    // write number of tables in db
    written = fwrite(&table_count, sizeof(int), 1, output_file);
    if(!write_succeed(written, 1, filename_with_ext)){
        fclose(output_file);
        free(filename_with_ext);
        filename_with_ext = NULL;
        return;
    } 

    // empty database
    if(table_count==0){
        printf("Current database exported to '%s' successfully.\n\n", filename_with_ext);
        fclose(output_file);
        free(filename_with_ext);
        filename_with_ext = NULL;
        return;
    }
    
    // write table one by one
    current_table = first_table;
    for(i=0; i<table_count; i++){
        if(!export_table(output_file, filename_with_ext, current_table)){
            fclose(output_file);
            free(filename_with_ext);
            filename_with_ext = NULL;
            return;
        }
        current_table = current_table->next_table;
    }

    fclose(output_file);
    printf("Current database exported to '%s' successfully.\n\n", filename_with_ext);
    
    free(filename_with_ext);
    filename_with_ext = NULL;
}