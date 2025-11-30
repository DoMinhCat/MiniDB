/*
Date of creation : 16/10/2025
Description : Main entry point of the program
Group 2 ESGI 2A3
*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "../include/clean.h"
#include "../include/parser.h"
#include "../include/db.h"
#include "../include/ini.h"
#include "../include/global.h"
#include "../include/hash.h"
#include "../include/file.h"
#include "../helper/format/format.h"

void flush_extra(){
    int c = getchar();
    if (c != '\n' && c != EOF) {
        while ((c = getchar()) != '\n' && c != EOF);
    }
}

int main(){
    char import_export_choice;
    char* export_name = NULL;
    char* import_name = NULL;

    char batch_buffer[MAX_BATCH_SIZE];
    char* batch_input = NULL;
    char** commands = NULL;
    int cmd_count;
    bool exit_cmd = false;

    char confirm;
    char file_buffer[MAX_FILE_NAME];

    Query* parser_output = NULL;

    printf("Welcome to MiniDB !\n");
    printf("The Final Project developed in C by Minh Cat, Paco, Bamba. 2A3 ESGI 2025-2026.\n");
    printf("Please refer to README.md for usage and additional information.\n");
    print_divider();

    // Prompt for file import
    do{
        printf("Do you want to import an existing database, do it now or never (y/n) : ");
        scanf(" %c", &import_export_choice);
        flush_extra();
    } while (import_export_choice != 'y' && import_export_choice != 'Y' && 
             import_export_choice != 'n' && import_export_choice != 'N');
    
    if(import_export_choice == 'y' || import_export_choice == 'Y'){
        print_divider();
        do{
            printf("\nChoose the binary file to be imported (with extension): ");
            import_name = read_file_name(file_buffer);
        } while(!import_name);

        printf("Importing database from '%s'...\n", import_name);
        // import_db();

        free(import_name);
        import_name = NULL;
        print_divider();
    }else {
        print_divider();
        printf("Database importation aborted.\n");
        print_divider();
    }

    // Infinite loop to get user's command
    while(1) {
        printf(">>> ");
        
        // Read all cmds
        batch_input = read_batch_cmd(batch_buffer);
        if (batch_input == NULL) continue;
        
        // Split into individual commands
        commands = split_commands(batch_input, &cmd_count);
        if (cmd_count == 0) {
            free(batch_input);
            continue;
        }
        
        printf("\n");
        // Execute each command
        for (int i = 0; i < cmd_count; i++) {
            parser_output = parse_cmd(commands[i]);
            
            // Check exit/quit
            if (parser_output->cmd_type == EXIT) {
                // Free current command first
                free(commands[i]);
                // Free all remaining commands
                for (int j = i + 1; j < cmd_count; j++) {
                    free(commands[j]);
                }
                free(commands);
                free(batch_input);
                free_query(&parser_output);
                exit_cmd = true;
                break; 
            }
            
            // Check invalid syntax
            if (parser_output->cmd_type == INVALID) {
                free_query(&parser_output);
                free(commands[i]);
                continue; 
            }
            
            // Execute commands
            switch (parser_output->cmd_type) {
            case SHOW:
                show(parser_output);
                break;
            case DESCRIBE:
                describe_table(parser_output);
                break;
            case CREATE:
                create_table(parser_output);
                break;
            case INSERT:
                insert(parser_output);
                break;
            case SELECT:
                select(parser_output);
                break;
            case DELETE:
                // Without WHERE clause - skip confirmation in batch mode if multiple commands
                if (!parser_output->params.delete_params.condition_column) {
                    if (cmd_count == 1) {
                        printf("Confirm deletion of all rows from '%s' table, press 'y' to proceed (cancel on default): ", 
                               parser_output->params.delete_params.table_name);
                        scanf(" %c", &confirm);
                        flush_extra();
                        
                        if (confirm == 'y') {
                            delete_from_table(parser_output);
                        } else {
                            printf("Execution of DELETE statement aborted.\n\n");
                        }
                    } else {
                        // In batch mode, auto-skip for safety
                        printf("Warning: DELETE without WHERE in batch mode - skipping for safety.\n");
                    }
                } else {
                    delete_from_table(parser_output);
                }
                break;
            case DROP:
                if (cmd_count == 1) {
                    printf("Confirm deletion of %d %s, press 'y' to proceed (cancel on default): ", 
                           parser_output->params.drop_params.table_count, 
                           parser_output->params.drop_params.table_count > 1 ? "tables" : "table");
                    scanf(" %c", &confirm);
                    flush_extra();
                    
                    if (confirm == 'y') {
                        drop_table(parser_output);
                    } else {
                        printf("Execution of DROP statement aborted.\n\n");
                    }
                } else {
                    // In batch mode, auto-skip for safety
                    printf("Warning: DROP in batch mode - skipping for safety.\n");
                }
                break;
            default:
                printf("Invalid command, please check the syntax.\n\n");
                break;
            }
            
            free_query(&parser_output);
            free(commands[i]);
        }
        
        if(!exit_cmd) {
            free(commands);
            free(batch_input);
        }
        
        if(exit_cmd) {
            break;
        }
    }

    // Prompt for db export
    do {
        printf("Do you want to export the current database, do it now or never (y/n) : ");
        scanf(" %c", &import_export_choice);
        flush_extra();
    } while (import_export_choice != 'y' && import_export_choice != 'Y' && 
             import_export_choice != 'n' && import_export_choice != 'N');

    if(import_export_choice == 'y' || import_export_choice == 'Y'){
        print_divider();
        do{
            printf("\nChoose a name for your file to be exported (without extension): ");
            export_name = read_file_name(file_buffer);
        } while(!export_name);
        
        printf("Exporting database from '%s'...\n", export_name);
        export_db(export_name, first_table);

        free(export_name);
        export_name = NULL;
        print_divider();
    }else {
        print_divider();
        printf("Database exportation aborted.\n");
        print_divider();
    }

    // free all db struct before exit
    free_db(first_table);
    first_table = NULL;

    printf("Goodbye !\n");
    exit(EXIT_SUCCESS);
}

