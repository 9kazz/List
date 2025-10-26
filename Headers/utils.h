#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "list.h"

const int POISON = 0xDEADFACE;

enum Errors_and_warnings {
    NO_ERRORS       =  0,
    NULLP_TO_STRUCT =  1,
    NULLP_TO_DATA   =  2,
    NULLP_TO_NEXT   =  4,
    NULLP_TO_PREV   =  8,
    HEAD_OUT_OF_BUF = 16,
    TAIL_OUT_OF_BUF = 32,
    FREE_OUT_OF_BUF = 64
};

#define SAFE_CALLOC(name, size_of_buf, el_type)                                     \
    el_type* temp_##name = (el_type*) calloc(size_of_buf, sizeof(el_type));         \
                                                                                    \
    if (temp_##name == NULL)                                                        \
        fprintf(stderr, "Allocation error of" #name);                               \
                                                                                    \
    el_type* name = temp_##name;


ListErr List_Dump_graphviz(ListStruct list, FILE* output_file);

#endif