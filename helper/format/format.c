#include <stdio.h>
#include <string.h>

#include "format.h"

int MAX_TABLE_WIDTH = 150;
int MIN_COL_WIDTH = 15;

void print_divider(){
    for(int i=0; i<20; i++) printf("-");
    printf("\n");
}

void print_separator_line(int col_count, int col_width) {
    printf("|");
    for (int i = 0; i < col_count; i++) {
        for (int j = 0; j < col_width; j++) {
            printf("-");
        }
        printf("|");
    }
    printf("\n");
}

void print_cell(char* content, int width) {
    // Helper function to print a cell with proper width and truncation
    int content_len = (int)strlen(content);
    
    // Width includes the spaces, so actual content space is width - 2
    int available_width = width - 2;
    
    if (content_len > available_width) {
        // Need to truncate with "..."
        if (available_width >= 3) {
            // Print content with "..." at the end
            printf(" ");
            for (int i = 0; i < available_width - 3; i++) {
                printf("%c", content[i]);
            }
            printf("... ");
        } else {
            // Not enough space for "...", just truncate
            printf(" %-*.*s ", available_width, available_width, content);
        }
    } else {
        // Content fits, pad with spaces
        printf(" %-*s ", available_width, content);
    }
}

int calculate_col_width(int col_count) {
    int separators = col_count + 1; // number of "|" characters
    int available = MAX_TABLE_WIDTH - separators;
    int width = available / col_count;
    
    // Ensure we don't go below minimum
    return width < MIN_COL_WIDTH ? MIN_COL_WIDTH : width;
}

