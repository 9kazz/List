#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "LogFile.h"
#include "utils.h"
#include "list.h"

ListStruct List_Ctor(size_t size) {

    SAFE_CALLOC(data, size + 1, int)
    SAFE_CALLOC(next, size + 1, int)
    SAFE_CALLOC(prev, size + 1, int)

    for (size_t ind = 0; ind <= size; ind++) {
        next[ind] = ind + 1;
        data[ind] = POISON;
    }

    next[0]    = 0;
    next[size] = 0;

    ListStruct list;
        list.size = size + 1;
        list.data = data;
        list.next = next;
        list.prev = prev;
        list.head = 0;
        list.tail = 0;
        list.free = 1;

    return list;
}

ListErr List_Dtor(ListStruct* list) {

    ONDEBUG(list);

    free(list -> data);
    free(list -> next);
    free(list -> prev);

    return NO_ERRORS;
}

ListErr List_Verify(ListStruct* list) {

    ListErr Error_code = 0;

    #define IF_NULL_POINTER(pointer, error)                                                                     \
        if (pointer == NULL) {                                                                                  \
                                                                                                                \
            fprintf(LogFile, "List_Verify: NULL pointer to" "\"" #pointer "\"\n");                              \
            fprintf(LogFile, "Error code: %d (" #error ")\n\n", error);                                         \
                                                                                                                \
            Error_code = Error_code | error;                                                                    \
                                                                                                                \
            fprintf(LogFile, "List_Verify: verification ended with critical error: %d\n", Error_code);          \
            return Error_code;                                                                                  \
        }

    IF_NULL_POINTER(list,       NULLP_TO_STRUCT)
    IF_NULL_POINTER(list->data, NULLP_TO_DATA)
    IF_NULL_POINTER(list->next, NULLP_TO_NEXT)
    IF_NULL_POINTER(list->prev, NULLP_TO_PREV)

    #undef IF_NULL_POINTER

    #define IF_INDEX_OUT_OF_BUF(index, error)                                                                   \
        if (index < 0 ||                                                                                        \
            index > list -> size - 1)                                                                           \
        {                                                                                                       \
                                                                                                                \
            fprintf(LogFile, "List_Verify: index out of buffer (" #index ")\n");                                \
            fprintf(LogFile, "Error code: %d (" #error ")\n\n", error);                                         \
                                                                                                                \
            Error_code = Error_code | error;                                                                    \
                                                                                                                \
            fprintf(LogFile, "List_Verify: verification ended with critical error: %d\n", Error_code);          \
            return Error_code;                                                                                  \
        }        

    IF_INDEX_OUT_OF_BUF(list->head, HEAD_OUT_OF_BUF)
    IF_INDEX_OUT_OF_BUF(list->tail, TAIL_OUT_OF_BUF)
    IF_INDEX_OUT_OF_BUF(list->free, FREE_OUT_OF_BUF)

    #undef IF_INDEX_OUT_OF_BUF

    if (Error_code == 0)
        fprintf(LogFile, "List_Verify: verification is success (Error_code = 0) \n");

    else
        fprintf(LogFile, "List_Verify: verification ended with warning: %d\n", Error_code);   

    return Error_code;                                                                      
}

 ListErr List_Insert(ListStruct* list, size_t ind, int value) {

    ONDEBUG(list);

    size_t first_free_ind_data = list -> free;

    list -> data[first_free_ind_data] = value;

    list -> free = list -> next[first_free_ind_data];

    if (list -> head == 0 &&
        list -> tail == 0)
    {
        list -> prev[first_free_ind_data] = 0;
        list -> next[first_free_ind_data] = 0;

        list -> head = first_free_ind_data;
        list -> tail = first_free_ind_data;
    }

    else if (ind == list -> tail)
    {
        list -> prev[first_free_ind_data] = list -> tail;
        list -> next[first_free_ind_data] = 0;

        list -> next[ind] = first_free_ind_data;

        list -> tail = first_free_ind_data;
    }

    else if (ind == 0) 
    {
        list -> prev[first_free_ind_data] = 0;
        list -> next[first_free_ind_data] = list -> head;

        list -> prev[ind] = first_free_ind_data;

        list -> head = first_free_ind_data;
    }

    else 
    {
        list -> prev[first_free_ind_data] = ind;
        list -> next[first_free_ind_data] = list -> next[ind];

        list -> prev[ list -> next[ind] ] = first_free_ind_data;

        list -> next[ind] = first_free_ind_data;
        
        list -> next[ind] = first_free_ind_data;
    }   

    ONDEBUG(list);

    return NO_ERRORS;
 }

ListErr List_Delete(ListStruct* list, size_t ind) {

    ONDEBUG(list)

    if (list -> head == 0 &&
        list -> tail == 0)
    {
        fprintf(stderr, "Attempt to delete element from empty list\n");
        return DELETE_EMPTY_LIST;
    }

    else if (ind == list -> tail)
    {
        size_t Tail = list -> tail;

        list -> next[ list -> prev[Tail] ] = 0;

        list -> tail = list -> prev[Tail];
    }
    
    else if (ind == list -> head) 
    {
        size_t Head = list -> head;

        list -> prev[ list -> next[Head] ] = 0;

        list -> head = list -> next[Head];
    }

    else 
    {
        list -> next[ list -> prev[ind] ] = list -> next[ind];
        list -> prev[ list -> next[ind] ] = list -> prev[ind];
    }   

    list -> next[ind]          = list -> free;
    list -> prev[list -> free] = ind;
    list -> free               = ind;

    list -> data[ind] = POISON;
    list -> prev[ind] = 0;

    ONDEBUG(list);

    return NO_ERRORS;    
}