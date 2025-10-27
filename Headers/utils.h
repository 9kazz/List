#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "list.h"

const int POISON = 0xDEADFACE;
const int MAX_FILE_COUNT = 32;

enum Errors_and_warnings {
    NO_ERRORS         =     0,
    DELETE_EMPTY_LIST =     1,  
    NO_MORE_FILES     =     2, 
    FILE_NAME_ERR     =     4,
    MIN_CRITICAL_ERR  =    16,     // Isn`t error. It`s just constant, after which follows not critical errors (warnings)
    NULLP_TO_STRUCT   =    16,
    NULLP_TO_DATA     =    32,
    NULLP_TO_NEXT     =    64,
    NULLP_TO_PREV     =   128,
    HEAD_OUT_OF_BUF   =   256,
    TAIL_OUT_OF_BUF   =   512,
    FREE_OUT_OF_BUF   =  1024,
};

#define SAFE_CALLOC(name, size_of_buf, el_type)                                     \
    el_type* temp_##name = (el_type*) calloc(size_of_buf, sizeof(el_type));         \
                                                                                    \
    if (temp_##name == NULL)                                                        \
        fprintf(stderr, "Allocation error of" #name "\n");                          \
                                                                                    \
    el_type* name = temp_##name;    
    
#define SAFE_FOPEN(name, file, mode)                                                \
    FILE* temp_##name = fopen(file, mode);                                          \
                                                                                    \
    if (temp_##name == NULL)                                                        \
        fprintf(stderr, "File opening error (" #name ")\n");                        \
                                                                                    \
    FILE* name = temp_##name; 

#ifdef DEBUG    
    #define ONDEBUG(list)                       \
        {                                       \
        ListErr error = List_Verify(list);      \
                                                \
        if (error >= MIN_CRITICAL_ERR)          \
            return error;                       \
        }                                   
#else
    #define ONDEBUG(list)
#endif
    
    ListErr List_Dump         (ListStruct* list);
    ListErr List_Dump_graphviz(ListStruct  list, FILE* output_file);
    ListErr List_Dump_HTML    (ListStruct  list, const char* image, FILE* output_file);

#endif