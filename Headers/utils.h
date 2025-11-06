#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "list.h"

const int POISON = 0xDEADFACE;

enum Utils_Constants {
    MAX_FILE_COUNT    = 32,
    MAX_DUMP_INFO_LEN = 128,
    NO_INDEX          = -1
};

enum Successful_ending {
    UNIT_TEST_PASSED = 0,
    IS_FREE          = 1,
    NOT_FREE         = 0
};

enum Errors_and_warnings {
    NO_ERRORS         = 0,
    DELETE_NULL_EL    = 1,  
    NO_MORE_FILES     = 1 << 1, 
    FILE_NAME_ERR     = 1 << 2, 
    UNIT_TEST_FAILED  = 1 << 3, 
    INCOR_ACTION_DUMP = 1 << 4, 
    MIN_CRITICAL_ERR  = 1 << 8,     // Isn`t error. It`s just constant, after which follows not critical errors (warnings)
    NULLP_TO_STRUCT   = 1 << 8, 
    NULLP_TO_DATA     = 1 << 9, 
    NULLP_TO_NEXT     = 1 << 10, 
    NULLP_TO_PREV     = 1 << 11, 
    HEAD_OUT_OF_BUF   = 1 << 12, 
    TAIL_OUT_OF_BUF   = 1 << 13, 
    FREE_OUT_OF_BUF   = 1 << 14, 
    INCOR_INDEX       = 1 << 15,
    REALLOC_ERR       = 1 << 16, 
    CYCLE_ERR         = 1 << 17
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
    #define ONDEBUG(list, index)                                                                                                                     \
        {                                                                                                                                            \
        static char dump_info_str[MAX_DUMP_INFO_LEN] = {0};                                                                                          \
        snprintf(dump_info_str, MAX_DUMP_INFO_LEN, "Before <span style = \"color: blue;\"> %s </span> in %s:%d", __func__, __FILE__, __LINE__);      \
                                                                                                                                                     \
        ListErr error = List_Verify(list, index, dump_info_str);                                                                                     \
                                                                                                                                                     \
        if (error >= MIN_CRITICAL_ERR)                                                                                                               \
            return error;                                                                                                                            \
        }                                   

    #define LIST_DUMP_BEFORE(list, dump_info_str)                                                                                                    \
        {                                                                                                                                            \
        static char dump_info_str[MAX_DUMP_INFO_LEN] = {0};                                                                                          \
        snprintf(dump_info_str, MAX_DUMP_INFO_LEN, "Before <span style = \"color: blue;\"> %s </span> in %s:%d", __func__, __FILE__, __LINE__);      \
        List_Dump(list, dump_info_str);                                                                                                              \
        }
        
    #define LIST_DUMP_AFTER(list, dump_info_str)                                                                                                     \
        {                                                                                                                                            \
        static char dump_info_str[MAX_DUMP_INFO_LEN] = {0};                                                                                          \
        snprintf(dump_info_str, MAX_DUMP_INFO_LEN, "AFTER <span style = \"color: blue;\"> %s </span> in %s:%d", __func__, __FILE__, __LINE__);       \
        List_Dump(list, dump_info_str);                                                                                                              \
        }
    
#else
    #define ONDEBUG(list, index)
    #define LIST_DUMP_BEFORE(list, dump_info_str) 
    #define LIST_DUMP_AFTER(list, dump_info_str) 
#endif

    ListErr List_Dump         (ListStruct* list, const char* dump_info);
    ListErr List_Dump_graphviz(ListStruct* list, FILE* output_file);
    ListErr List_Dump_HTML    (ListStruct* list, const char* image, const char* dump_info, FILE* output_file);

    ListErr One_test_check    (ListStruct* list, int* correct_data, int* correct_next, int* correct_prev);
    ListErr Unit_tests        (void);

    int     is_free (ListStruct* list, size_t idx);

#endif