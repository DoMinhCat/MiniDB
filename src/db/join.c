/*
Date of creation : 17/11/2025
Description : Code for JOIN operation with bubble sort and linear search
Group 2 ESGI 2A3
*/

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

#include "../../helper/db/db.h"
#include "../../include/db.h"
#include "../../include/ini.h"
#include "../../include/clean.h"

int compare_data_field(int* int_data1, int* int_data2, char* str_data1, char* str_data2, double* double_data1, double* double_data2, ColType col_type){
    // compare any 2 data fields of same type. Return 0 if equal, -1 if val1 < val2, 1 if val1 > val2
    // IMPORTANT: NULL < Not NULL

    int result = 0;
    
    switch (col_type) { 
    case INT:
        if(int_data1 && int_data2){
            if(int_data1[0] > int_data2[0]) result = 1;
            else if(int_data1[0] < int_data2[0]) result = -1;
            else result = 0;
        }else if(int_data1 && !int_data2) result = 1;
        else if(!int_data1 && int_data2) result = -1;
        else result = 0;
        break;
    
    case DOUBLE:
        if(double_data1 && double_data2){
            if(compare_double(double_data1[0], double_data2[0]) == 1) result = 1;
            else if(compare_double(double_data1[0], double_data2[0]) == -1) result = -1;
            else result = 0;
        }else if(double_data1 && !double_data2) result = 1;
        else if(!double_data1 && double_data2) result = -1;
        else result = 0;
        break;

    case STRING:
        if(str_data1 && str_data2){
            if(strcmp(str_data1, str_data2) > 0) result = 1;
            else if(strcmp(str_data1, str_data2) < 0) result = -1;
            else result = 0;
        }else if(str_data1 && !str_data2) result = 1;
        else if(!str_data1 && str_data2) result = -1;
        else result = 0;
        break;
    default:
        break;
    }

    return result;
}

FilteredRow* bubble_sort(FilteredRow* head_list, int row_count, int data_index, ColType col_type){
    // sort a list of rows (ascending) of a table based on joined column value

    if (!head_list || row_count < 2) return head_list;

    bool swap_done;
    bool should_swap;
    int i,j;
    int cmp;

    FilteredRow* current_fr = NULL;
    FilteredRow* next_fr = NULL;
    FilteredRow* prev_fr = NULL;

    Row* actual_row = NULL;
    Row* next_actual_row = NULL;
    int** int_list_to_cmp = NULL;
    int** next_int_list_to_cmp = NULL;
    int* int_data_to_cmp = NULL;
    int* next_int_data_to_cmp = NULL;

    double** double_list_to_cmp = NULL;
    double** next_double_list_to_cmp = NULL;
    double* double_data_to_cmp = NULL;
    double* next_double_data_to_cmp = NULL;

    char** str_list_to_cmp = NULL;
    char** next_str_list_to_cmp = NULL;
    char* str_data_to_cmp = NULL;
    char* next_str_data_to_cmp = NULL;

    // bubble sort
    for(i=0; i<row_count-1; i++){
        swap_done = false;
        current_fr = head_list;
        prev_fr = NULL;

        for(j=0; j<row_count-i-1; j++){ // largest item at the end, no need to compare again items sorted at the back
            actual_row = current_fr->row;
            next_actual_row = current_fr->next_filtered_row->row;
            should_swap = false;

            // get right data field to compare and compare them
            switch (col_type) { 
            case INT:
                // get right data 
                int_list_to_cmp = actual_row->int_list;
                int_data_to_cmp = int_list_to_cmp[data_index];
                next_int_list_to_cmp = next_actual_row->int_list;
                next_int_data_to_cmp = next_int_list_to_cmp[data_index];

                cmp = compare_data_field(int_data_to_cmp, next_int_data_to_cmp, NULL, NULL, NULL, NULL, INT);
                if(cmp == 1) should_swap = true;
                break;
            
            case DOUBLE:
                double_list_to_cmp = actual_row->double_list;
                double_data_to_cmp = double_list_to_cmp[data_index];
                next_double_list_to_cmp = next_actual_row->double_list;
                next_double_data_to_cmp = next_double_list_to_cmp[data_index];
                
                cmp = compare_data_field(NULL, NULL, NULL, NULL, double_data_to_cmp, next_double_data_to_cmp, DOUBLE);
                if(cmp == 1) should_swap = true;
                break;

            case STRING:
                str_list_to_cmp = actual_row->str_list;
                str_data_to_cmp = str_list_to_cmp[data_index];
                next_str_list_to_cmp = next_actual_row->str_list;
                next_str_data_to_cmp = next_str_list_to_cmp[data_index];
                
                cmp = compare_data_field(NULL, NULL, str_data_to_cmp, next_str_data_to_cmp, NULL, NULL, STRING);
                if(cmp == 1) should_swap = true;
                break;
            default:
                break;
            }
            
            // swap if not in right order
            if(should_swap){
                next_fr = current_fr->next_filtered_row;
                current_fr->next_filtered_row = next_fr->next_filtered_row;
                next_fr->next_filtered_row = current_fr;
                
                // handle prev node in 2 cases: swap in middle and swap at head
                if(current_fr == head_list) head_list = next_fr;
                else prev_fr->next_filtered_row = next_fr; 

                swap_done = true;
            }
            // advance pointers
            if(should_swap) {
                prev_fr = next_fr;
            } else{
                prev_fr = current_fr;
                current_fr = current_fr->next_filtered_row;
            }
        }
        // stop sorting if no swapping done
        if(!swap_done) break;
    }

    return head_list;
}

FilteredRow* merge_sorted_lists(Table* tab1, Table* tab2, SelectParams* params, FilteredRow* list1, FilteredRow* list2, int data_index1, int data_index2, ColType col_on_type){
    /*
    compare joined col value of 2 pointers 
        if == : merge data fields and save to result struct 
        if != : the smaller one advance then compare again
    */  

    FilteredRow* result = NULL;
    FilteredRow* new_node = NULL;
    FilteredRow* last_node = NULL;
    FilteredRow* start_dup1 = NULL;
    FilteredRow* start_dup2 = NULL;
    FilteredRow* end1 = NULL;
    FilteredRow* end2 = NULL;
    FilteredRow* p1 = list1;
    FilteredRow* p2 = list2;
    Row* row1 = NULL;
    Row* row2 = NULL;
    Col* current_col = NULL;
    SelectedColInfo col_info[params->col_count];
    int cmp = 0;
    int cmp1 = 0;
    int cmp2 = 0;
    int i;
    int int_count = 0;
    int double_count = 0;
    int str_count = 0;
    int int_index = 0;
    int double_index = 0;
    int str_index = 0;
    bool select_all = params->col_count == 1 && strcmp(params->col_list[0], "*") == 0;
    
    // collect metadata for data insert into joined lists
    if(!select_all){
        for(i=0; i<params->col_count; i++){   
            // all selected cols have been checked in select that they exist
            current_col = NULL;

            current_col = get_col_by_name(tab1, params->col_list[i]);
            if(!current_col) {
                current_col = get_col_by_name(tab2, params->col_list[i]);
                col_info[i].table_id = 2;
                col_info[i].data_index = get_data_list_index(tab2, params->col_list[i]);
            }else {
                col_info[i].table_id = 1;
                col_info[i].data_index = get_data_list_index(tab1, params->col_list[i]);
            }
            col_info[i].type = current_col->type;

            // get size to alloc joined lists
            if(current_col->type == INT) int_count++;
            else if(current_col->type == DOUBLE) double_count++;
            else str_count++;

        }
    }

    while(p1 && p2){
        row1 = p1->row;
        row2 = p2->row;
        start_dup1 = p1;
        start_dup2 = p2;

        //compare 
        switch (col_on_type) { 
        case INT:
            cmp = compare_data_field(row1->int_list[data_index1], row2->int_list[data_index2], NULL, NULL, NULL, NULL, INT);
            break;
        case DOUBLE:
            cmp = compare_data_field(NULL, NULL, NULL, NULL, row1->double_list[data_index1], row2->double_list[data_index2], DOUBLE);
            break;
        case STRING:
            cmp = compare_data_field(NULL, NULL, row1->str_list[data_index1], row2->str_list[data_index2], NULL, NULL, STRING);
            break;
        default:
            break;
        }

        if(cmp == 0){
            // collect duplicates of list1
            end1 = p1;
            while(end1){
                switch (col_on_type) { 
                case INT:
                    cmp1 = compare_data_field(start_dup1->row->int_list[data_index1], end1->row->int_list[data_index1], NULL, NULL, NULL, NULL, INT);
                    break;
                case DOUBLE:
                    cmp1 = compare_data_field(NULL, NULL, NULL, NULL, start_dup1->row->double_list[data_index1], end1->row->double_list[data_index1], DOUBLE);
                    break;
                case STRING:
                    cmp1 = compare_data_field(NULL, NULL, start_dup1->row->str_list[data_index1], end1->row->str_list[data_index1], NULL, NULL, STRING);
                    break;
                default:
                    break;
                }
                if(cmp1!=0) break;
                end1 = end1->next_filtered_row;
            }

            // collect duplicates of list2
            end2 = p2;
            while(end2){
                switch (col_on_type) { 
                case INT:
                    cmp2 = compare_data_field(start_dup2->row->int_list[data_index2], end2->row->int_list[data_index2], NULL, NULL, NULL, NULL, INT);
                    break;
                case DOUBLE:
                    cmp2 = compare_data_field(NULL, NULL, NULL, NULL, start_dup2->row->double_list[data_index2], end2->row->double_list[data_index2], DOUBLE);
                    break;
                case STRING:
                    cmp2 = compare_data_field(NULL, NULL, start_dup2->row->str_list[data_index2], end2->row->str_list[data_index2], NULL, NULL, STRING);
                    break;
                default:
                    break;
                }
                if(cmp2 != 0) break;
                end2 = end2->next_filtered_row;
            }

            // combine all duplicates
            for(FilteredRow* r1 = start_dup1; r1 != end1; r1 = r1->next_filtered_row) {
                for(FilteredRow* r2 = start_dup2; r2 != end2; r2 = r2->next_filtered_row) {
                    row1 = r1->row;
                    row2 = r2->row;

                    // set data for required cols and append to result list
                    if(select_all){
                        copy_data_lists_to_filtered(row1, row2);

                        if(result == NULL) {
                            result = new_node;
                            last_node = result;
                        } else {
                            // append to the end
                            last_node->next_filtered_row = new_node;
                            last_node = new_node;
                        }
                    }else{
                        int_index = 0;
                        double_index = 0;
                        str_index = 0;
                        new_node = init_filtered_row();

                        // calloc joined lists
                        assert((new_node->int_joined_list = (int**)calloc(int_count, sizeof(int*))) != NULL);
                        assert((new_node->double_joined_list = (double**)calloc(double_count, sizeof(double*))) != NULL);
                        assert((new_node->str_joined_list = (char**)calloc(str_count, sizeof(char*))) != NULL);

                        // set data for joined lists
                        for(i=0; i<params->col_count; i++){ 
                            switch (col_info[i].type) {
                            case INT:
                                assert((new_node->int_joined_list[int_index] = malloc(sizeof(int))) != NULL);
                                if(col_info[i].table_id==1) new_node->int_joined_list[int_index][0] = row1->int_list[col_info[i].data_index][0];
                                else new_node->int_joined_list[int_index][0] = row2->int_list[col_info[i].data_index][0];
                                int_index++;
                                break;

                            case DOUBLE:
                                assert((new_node->double_joined_list[double_index] = malloc(sizeof(double))) != NULL);
                                if(col_info[i].table_id==1) new_node->double_joined_list[double_index][0] = row1->double_list[col_info[i].data_index][0];
                                else new_node->double_joined_list[double_index][0] = row2->double_list[col_info[i].data_index][0];
                                double_index++;
                                break;

                            case STRING:
                                if(col_info[i].table_id==1) new_node->str_joined_list[str_index] = strdup(row1->str_list[col_info[i].data_index]);
                                else new_node->str_joined_list[str_index] = strdup(row2->str_list[col_info[i].data_index]);
                                assert(new_node->str_joined_list[str_index] != NULL);
                                str_index++;
                                break;                    
                            default:
                                break;
                            }  
                        }
                        new_node->int_join_count = int_count;
                        new_node->double_join_count = double_count;
                        new_node->str_join_count = str_count;

                        if(result == NULL) {
                            result = new_node;
                            last_node = result;
                        } else {
                            // append to the end
                            last_node->next_filtered_row = new_node;
                            last_node = new_node;
                        }
                    }            
                }
            }
            // advannce both pointer after matching duplicates
            p1 = end1;
            p2 = end2;
        }else if(cmp == 1){
            // advance p2
            p2 = p2->next_filtered_row;
        }else{
            //advance p1
            p1 = p1->next_filtered_row;
        }
    }

    // Free 2 lists before exit
    free_filtered_set(list1);
    free_filtered_set(list2);
    return result;
}

FilteredRow* join(Table* tab1, Table* tab2, Col* col1, Col* col2, SelectParams* params){
    // join the 2 tables, return filtered rows for final result print of select
    /*
    copy row linked list of 2 tables to 2 filtered struct
    bubble_sort 2 sorted filtered struct
    merge_sorted_lists into 1 final filtered struct
    return that struct
    */ 

    // edge cases JOIN empty-empty or empty-not empty tables shall return NULL
    if(!tab1->first_row || !tab2->first_row) return NULL;

    FilteredRow* head_list1 = NULL;
    FilteredRow* head_list2 = NULL;
    FilteredRow* result = NULL;

    int row_count1 = tab1->row_count;
    int row_count2 = tab2->row_count;
    int data_index1 = get_data_list_index(tab1, col1->name);
    int data_index2 = get_data_list_index(tab2, col2->name);

    // copy linked list Row of tab1
    head_list1 = copy_rows_to_filtered(tab1);
    
    //copy linked list Row of tab2
    head_list2 = copy_rows_to_filtered(tab2);

    // bubble sort
    head_list1 = bubble_sort(head_list1, row_count1, data_index1, col1->type);
    head_list2 = bubble_sort(head_list2, row_count2, data_index2, col1->type);

    //merge, 2 input lists freed in merge_sorted_lists before exit
    result = merge_sorted_lists(tab1, tab2, params, head_list1, head_list2, data_index1, data_index2, col1->type);

    return result;
}

FilteredRow* join_with_where(FilteredRow* head_list_where, Table* tab, Table* tab_where, Col* col_tab, Col* col_tab_where, SelectParams* params){
    // same as join() but with filtered list done by WHERE instead of 2 tables

    // edge cases JOIN empty-empty or empty-not empty returns NULL
    if(!head_list_where || !tab->first_row) return NULL;

    FilteredRow* head_list_tab = NULL;
    FilteredRow* result = NULL;
    FilteredRow* current_fr = NULL;
    int data_index_tab = get_data_list_index(tab, col_tab->name);
    int data_index_tab_where = get_data_list_index(tab_where, col_tab_where->name);
    int row_count_where = 0;

    head_list_tab = copy_rows_to_filtered(tab);

    // get number of rows filtered from where
    for(current_fr = head_list_where; current_fr!=NULL; current_fr=current_fr->next_filtered_row) row_count_where++;

    // bubble sort
    head_list_tab = bubble_sort(head_list_tab, tab->row_count, data_index_tab, col_tab->type);
    head_list_where = bubble_sort(head_list_where, row_count_where, row_count_where, col_tab->type);

    //merge, 2 input lists freed in merge_sorted_lists before exit
    result = merge_sorted_lists(tab, tab_where, params, head_list_tab, head_list_where, data_index_tab, data_index_tab_where, col_tab->type);

    return result;
}
