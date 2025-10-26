#ifndef LIST_H
#define LIST_H

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "utils.h"

typedef int ListErr;

struct ListStruct {
    size_t size;
    int*   data;
    int*   next;
    int*   prev;
    size_t head;
    size_t tail;
    size_t free;
};

ListStruct List_Ctor  (size_t size);
ListErr    List_Dtor  (ListStruct* list);
ListErr    List_Verify(ListStruct* list);

ListErr    List_Insert(ListStruct* list, size_t ind, int value);

#endif