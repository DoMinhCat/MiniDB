/*
Date of creation : 26/10/2025
Description : Clean initialisation of Query structure
Group 2 ESGI 2A3
*/

#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "../../include/parser.h"
#include "../../include/ini.h"

 
Query* init_query(){
    Query* query = NULL;
    assert((query = (Query*)malloc(sizeof(Query))) != NULL);
    
    query->cmd_type = INVALID;
    memset(&query->params, 0, sizeof(query->params));

    return query;
}
