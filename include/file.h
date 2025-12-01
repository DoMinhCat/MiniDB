/*
Date of creation : 16/11/2025
Description : header file for i/o
Group 2 ESGI 2A3
*/

#ifndef FILE_H
#define FILE_H

#include "db.h"

void export_db(char* output_file_name, Table* first_table);
void import_db(char* import_name);

#endif

