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
    size_t free;
};

ListStruct List_Ctor  (size_t size);
ListErr    List_Dtor  (ListStruct* list);
ListErr    List_Verify(ListStruct* list, size_t index, const char* dump_info);

int            List_Insert_after (ListStruct* list, size_t ind, int value);
int            List_Insert_before(ListStruct* list, size_t ind, int value);
int            List_Insert_Head  (ListStruct* list, int value);
int            List_Insert_Tail  (ListStruct* list, int value);
ListErr        List_Delete       (ListStruct* list, size_t ind);
ListStruct*    List_Realloc      (ListStruct* list);

ListStruct     List_Linearize    (ListStruct* list);

size_t     get_head (ListStruct* list);
size_t     get_tail (ListStruct* list);
size_t     get_free (ListStruct* list);
int        get_data (ListStruct* list, size_t ind);
int        get_next (ListStruct* list, size_t ind);
int        get_prev (ListStruct* list, size_t ind);


#define DATA(inx)   list -> data[inx]

#define NEXT(inx)   list -> next[inx]

#define PREV(inx)   list -> prev[inx]

#endif