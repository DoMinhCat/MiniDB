/*
Date of creation : 16/10/2025
Description : Definition of functions parsing user's commands
Group 2 ESGI 2A3
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "../../include/parser.h"
#include "../../include/ini.h"

char* read_batch_cmd(char* batch_buffer) {
    char* batch_string = NULL;
    int total_len = 0;
    int c;
    
    // Read until semicolon or EOF
    while (total_len < MAX_BATCH_SIZE - 1) {
        c = getchar();
        
        if (c == EOF) {
            if (total_len == 0) return NULL;
            break;
        }
        
        batch_buffer[total_len++] = c;
        
        if (c == ';') {
            break;
        }
    }
    
    // Check if batch is too long
    if (total_len >= MAX_BATCH_SIZE - 1 && c != ';') {
        while ((c = getchar()) != ';' && c != EOF);
        printf("Batch command too long (max %d characters).\n", MAX_BATCH_SIZE-1);
        return NULL;
    }
    
    batch_buffer[total_len] = '\0';
    
    assert((batch_string = strdup(batch_buffer)) != NULL);
    return batch_string;
}

char** split_commands(char* batch, int* cmd_count) {
    char** commands = malloc(sizeof(char*) * 100); // Max 100 commands
    *cmd_count = 0;
    
    char* batch_copy = strdup(batch);
    char* line = strtok(batch_copy, "\n");
    
    while (line != NULL && *cmd_count < 100) {
        // Trim whitespace and check if line is not empty
        while (*line == ' ' || *line == '\t') line++;
        
        // Remove semicolon if present
        int len = strlen(line);
        if (len > 0 && line[len-1] == ';') {
            line[len-1] = '\0';
            len--;
        }
        
        // Remove trailing whitespace
        while (len > 0 && (line[len-1] == ' ' || line[len-1] == '\t')) {
            line[len-1] = '\0';
            len--;
        }
        
        if (len > 0) {
            commands[*cmd_count] = strdup(line);
            (*cmd_count)++;
        }
        
        line = strtok(NULL, "\n");
    }
    
    free(batch_copy);
    return commands;
}

char* read_file_name(char* file_buffer){ 
    int c;
    char* file_name = NULL;

    if(fgets(file_buffer, MAX_FILE_NAME, stdin) == NULL || strcmp(file_buffer, "\n") ==0) return NULL;

    // check if cmd is too long
    if(strlen(file_buffer) > 0 && file_buffer[strlen(file_buffer)-1] != '\n'){
        while ((c = getchar()) != '\n' && c != EOF); // flush the rest of cmd in stdin if cmd too long
        printf("Command allows %d characters at max.\n", MAX_FILE_NAME-1);
        return NULL;
    } 

    // remove trailing newline
    file_buffer[strcspn(file_buffer, "\n")] = '\0';

    assert((file_name = strdup(file_buffer))!=NULL);
    return file_name;
}

// char* read_cmd(char* cmd_buffer){ 
//     int c;
//     char* cmd_string = NULL;

//     if(fgets(cmd_buffer, MAX_CMD_SIZE, stdin) == NULL || strcmp(cmd_buffer, "\n") ==0) return NULL;

//     // check if cmd is too long
//     if(strlen(cmd_buffer) > 0 && cmd_buffer[strlen(cmd_buffer)-1] != '\n'){
//         while ((c = getchar()) != '\n' && c != EOF); // flush the rest of cmd in stdin if cmd too long
//         printf("Command allows %d characters at max.\n", MAX_CMD_SIZE-1);
//         return "long";
//     } 

//     // remove trailing newline
//     cmd_buffer[strcspn(cmd_buffer, "\n")] = '\0';

//     assert((cmd_string = strdup(cmd_buffer))!=NULL);
//     return cmd_string;
// }

Query* parse_cmd(char* cmd) {
    Query* query = init_query();

    char* token;

    token = strtok(cmd, " \t");

    // no need to handle null cmd, already handled in main.c
    if(strcasecmp(token, "DELETE") == 0) parse_delete(&query);
    else if(strcasecmp(token, "DROP") == 0) parse_drop(&query);
    else if(strcasecmp(token, "CREATE") == 0) parse_create(&query);
    else if(strcasecmp(token, "SELECT") == 0) parse_select(&query);
    else if(strcasecmp(token, "INSERT") == 0) parse_insert(&query);
    else if(strcasecmp(token, "SHOW") == 0) parse_show(&query);
    else if(strcasecmp(token, "DESCRIBE") == 0) parse_describe(&query);

    // exit/quit case 
    else if(strcasecmp(token, "EXIT") == 0 || strcasecmp(token, "QUIT") == 0) query->cmd_type = EXIT;
    
    // first word is not one of the accepted command (create, select, insert,...)
    else {
        query->cmd_type = INVALID;
        fprintf(stderr, "Syntax error: '%s' command not found, please check the syntax.\n\n", token);
    }

    return query;
}

