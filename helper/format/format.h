
#ifndef FORMAT_H
#define FORMAT_H

// format printing
int calculate_col_width(int col_count);
void print_separator_line(int col_count, int col_width);
void print_cell(char* content, int width);
void print_divider();

extern int MAX_TABLE_WIDTH;
extern int MIN_COL_WIDTH;

#endif
