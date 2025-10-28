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

int List_Insert_after(ListStruct* list, size_t ind, int value) {

    ONDEBUG(list);

    List_Dump(list);

    size_t first_free_ind_data = list -> free;

    list -> data[first_free_ind_data] = value;
    list -> free = list -> next[first_free_ind_data];

    list -> prev[first_free_ind_data] = ind;
    list -> next[first_free_ind_data] = list -> next[ind];

    list -> prev[ list -> next[ind] ] = first_free_ind_data;

    list -> next[ind] = first_free_ind_data;

    list -> head = list -> next[0];
    list -> tail = list -> prev[0];

    ONDEBUG(list);

    List_Dump(list);

    return first_free_ind_data;
}


int List_Insert_before(ListStruct* list, size_t ind, int value) {

    ONDEBUG(list);

    List_Dump(list);

    size_t first_free_ind_data = list -> free;

    list -> data[first_free_ind_data] = value;
    list -> free = list -> next[first_free_ind_data];

    list -> next[first_free_ind_data] = ind;
    list -> prev[first_free_ind_data] = list -> prev[ind];

    list -> next[ list -> prev[ind] ] = first_free_ind_data;

    list -> prev[ind] = first_free_ind_data;

    list -> head = list -> next[0];
    list -> tail = list -> prev[0];

    ONDEBUG(list);

    List_Dump(list);

    return first_free_ind_data;
}

ListErr List_Delete(ListStruct* list, size_t ind) {

    ONDEBUG(list)

    List_Dump(list);

    if (ind == 0)
    {
        fprintf(stderr, "Attempt to delete null-element from the list\n");
        return DELETE_NULL_EL;
    }

    list -> next[ list -> prev[ind] ] = list -> next[ind];
    list -> prev[ list -> next[ind] ] = list -> prev[ind];

    list -> head = list -> next[0];
    list -> tail = list -> prev[0];   

    list -> next[ind]          = list -> free;
    list -> free               = ind;

    list -> data[ind] = POISON;
    list -> prev[ind] = 0;

    ONDEBUG(list);

    List_Dump(list);
    
    return NO_ERRORS;    
}