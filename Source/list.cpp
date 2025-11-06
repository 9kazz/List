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
        list.free = 1;

    return list;
}

ListErr List_Dtor(ListStruct* list) {

    // ONDEBUG(list, NO_INDEX)

    free(list -> data);
    free(list -> next);
    free(list -> prev);

    return NO_ERRORS;
}

ListErr List_Verify(ListStruct* list, size_t index, const char* dump_info) {
    
    ListErr Error_code = 0;

    #define IF_NULL_POINTER(pointer, error)                                                                                                         \
        if (pointer == NULL)                                                                                                                        \
        {                                                                                                                                           \
            fprintf(Logfile_html, "<h3 style = \"color: red;\"> List_Verify %s: NULL pointer to" "\"" #pointer "\"</h3>\n", dump_info);             \
            fprintf(Logfile_html, "<h3 style = \"color: red;\"> Error code: %d (" #error ")</h3>\n\n", error);                                      \
                                                                                                                                                    \
            Error_code = Error_code | error;                                                                                                        \
                                                                                                                                                    \
            fprintf(Logfile_html, "<h3 style = \"color: red;\"> List_Verify: verification ended with critical error: %d\n </h3>", Error_code);      \
            return Error_code;                                                                                                                      \
        }

    IF_NULL_POINTER(list,       NULLP_TO_STRUCT)
    IF_NULL_POINTER(list->data, NULLP_TO_DATA)
    IF_NULL_POINTER(list->next, NULLP_TO_NEXT)
    IF_NULL_POINTER(list->prev, NULLP_TO_PREV)

    #undef IF_NULL_POINTER

//---------------------------------------------------------------------------------------------------------------------------------------------------------

    #define IF_INDEX_OUT_OF_BUF(index, error)                                                                                                       \
        if (index < 0 ||                                                                                                                            \
            index > list -> size - 1)                                                                                                               \
        {                                                                                                                                           \
            fprintf(Logfile_html, "<h3 style = \"color: red;\"> List_Verify %s: index out of buffer (" #index ")</h3>\n", dump_info);               \
            fprintf(Logfile_html, "<h3 style = \"color: red;\"> Error code: %d (" #error ")</h3>\n\n", error);                                      \
                                                                                                                                                    \
            Error_code = Error_code | error;                                                                                                        \
                                                                                                                                                    \
            fprintf(Logfile_html, "<h3 style = \"color: red;\"> List_Verify: verification ended with critical error: %d\n </h3>", Error_code);      \
            return Error_code;                                                                                                                      \
        }        

    if (index != NO_INDEX) {
        IF_INDEX_OUT_OF_BUF(get_head(list), HEAD_OUT_OF_BUF)
        IF_INDEX_OUT_OF_BUF(get_tail(list), TAIL_OUT_OF_BUF)
        IF_INDEX_OUT_OF_BUF(list->free, FREE_OUT_OF_BUF)
        IF_INDEX_OUT_OF_BUF(index, INCOR_INDEX)
    }

//---------------------------------------------------------------------------------------------------------------------------------------------------------

    #define FREE_CYCLE_ERR                                                    \
        ( is_free(list, idx) && size_counter == list->size )

    #define NEXT_PREV_CYCLE_ERR                                               \
        ( ! is_free(list, idx) &&                                             \
        ( list->next[ list->prev[idx] ] != idx ||                             \
          list->prev[ list->next[idx] ] != idx ||                             \
          size_counter == list->size ) )

    #define IS_CYCLE(array, start, error)                                                                                                                                   \
        for (size_t idx = start, size_counter = 0; idx != 0; idx = array[idx], size_counter++) {                                              \
                                                                                                                                                                            \
            IF_INDEX_OUT_OF_BUF(idx,             INCOR_INDEX)                                                                                                               \
            IF_INDEX_OUT_OF_BUF(list->prev[idx], INCOR_INDEX)                                                                                                               \
            IF_INDEX_OUT_OF_BUF(list->next[idx], INCOR_INDEX)                                                                                                               \
                                                                                                                                                                            \
            if ( ( ! NEXT_PREV_CYCLE_ERR &&   FREE_CYCLE_ERR) ||                                                                                                            \
                 (   NEXT_PREV_CYCLE_ERR && ! FREE_CYCLE_ERR)  )                                                                                                            \
            {                                                                                                                                                               \
                fprintf(Logfile_html, "<h3 style = \"color: red;\"> List_Verify %s: " #array " (start from " #start ") isn`t cycled at %d </h3>\n", dump_info, idx);        \
                fprintf(Logfile_html, "<h3 style = \"color: red;\"> Error code: %d (" #error ")</h3>\n\n", error);                                                          \
                                                                                                                                                                            \
                Error_code = Error_code | error;                                                                                                                            \
                                                                                                                                                                            \
                fprintf(Logfile_html, "<h3 style = \"color: red;\"> Cycle error of " #array "</h3>\n");                                                                     \
                return Error_code;                                                                                                                                          \
            }                                                                                                                                                               \
        }

    IS_CYCLE(list->next, get_head(list), CYCLE_ERR)
    // IS_CYCLE(list->next, get_free(list), CYCLE_ERR)
    IS_CYCLE(list->prev, get_tail(list), CYCLE_ERR)
    
    #undef IF_INDEX_OUT_OF_BUF
    #undef NEXT_PREV_CYCLE_ERR
    #undef FREE_CYCLE_ERR
    #undef IS_CYCLE

//---------------------------------------------------------------------------------------------------------------------------------------------------------

    // if (Error_code == 0)
        // fprintf(LogFile, "List_Verify: verification is success (Error_code = 0) \n");

    if (Error_code != 0)
        fprintf(Logfile_html, "List_Verify: verification ended with warning: %d\n", Error_code);   

    return Error_code;                                                                      
}

int List_Insert_after(ListStruct* list, size_t ind, int value) {

    ONDEBUG(list, ind)

    LIST_DUMP_BEFORE(list, dump_info_str)

    size_t first_free_ind_data = get_free(list);

    DATA(first_free_ind_data) = value;

    if (get_free(list) != 0) 
        list -> free = get_next(list, first_free_ind_data);

    PREV(first_free_ind_data) = ind;
    NEXT(first_free_ind_data) = get_next(list, ind);

    PREV(NEXT(ind)) = first_free_ind_data;

    NEXT(ind) = first_free_ind_data;

    ONDEBUG(list, ind)

    LIST_DUMP_AFTER(list, dump_info_str)

    return first_free_ind_data;
}


int List_Insert_before(ListStruct* list, size_t ind, int value) {

    ONDEBUG(list, ind)

    LIST_DUMP_BEFORE(list, dump_info_str)

    size_t first_free_ind_data = get_free(list);

    DATA(first_free_ind_data) = value;

    if (get_free(list) != 0) 
        list -> free = get_next(list, first_free_ind_data);

    NEXT(first_free_ind_data) = ind;
    PREV(first_free_ind_data) = get_prev(list, ind);

    NEXT(PREV(ind)) = first_free_ind_data;

    PREV(ind) = first_free_ind_data;

    ONDEBUG(list, ind)

    LIST_DUMP_AFTER(list, dump_info_str)

    return first_free_ind_data;
}

int List_Insert_Head(ListStruct* list, int value) {
    
    ONDEBUG(list, NO_INDEX)

    size_t first_free_ind_data = get_free(list);

    DATA(first_free_ind_data) = value;

    if (get_free(list) != 0) 
        list -> free = get_next(list, first_free_ind_data);

    PREV(first_free_ind_data) = 0;
    NEXT(first_free_ind_data) = get_next(list, 0);

    PREV(NEXT(0)) = first_free_ind_data;

    NEXT(0) = first_free_ind_data;

    LIST_DUMP_BEFORE(list, dump_info_str)

    ONDEBUG(list, NO_INDEX)

    LIST_DUMP_AFTER(list, dump_info_str)

    return first_free_ind_data;
}

int List_Insert_Tail (ListStruct* list, int value) {

    ONDEBUG(list, NO_INDEX)

    LIST_DUMP_BEFORE(list, dump_info_str)

    size_t first_free_ind_data = get_free(list);

    DATA(first_free_ind_data) = value;

    if (get_free(list) != 0) 
        list -> free = get_next(list, first_free_ind_data);

    NEXT(first_free_ind_data) = 0;
    PREV(first_free_ind_data) = get_prev(list, 0);

    NEXT(PREV(0)) = first_free_ind_data;

    PREV(0) = first_free_ind_data;

    ONDEBUG(list, NO_INDEX)

    LIST_DUMP_AFTER(list, dump_info_str)

    return first_free_ind_data;
}

ListErr List_Delete (ListStruct* list, size_t ind) {

    ONDEBUG(list, NO_INDEX)

    LIST_DUMP_BEFORE(list, dump_info_str)

    if (ind == 0)
    {
        fprintf(stderr, "Attempt to delete null-element from the list\n");
        return DELETE_NULL_EL;
    }

    NEXT(PREV(ind)) = get_next(list, ind);
    PREV(NEXT(ind)) = get_prev(list, ind); 

    NEXT(ind)    = get_free(list);
    list -> free = ind;

    DATA(ind) = POISON;
    PREV(ind) = 0;

    ONDEBUG(list, NO_INDEX)

    LIST_DUMP_AFTER(list, dump_info_str)
    
    return NO_ERRORS;    
}

ListStruct* List_Realloc (ListStruct* list) {

    #define SAFE_REALLOC(array)                                                                          \
        int* temp_##array = (int*) realloc( (int*) list -> array, new_size * sizeof(temp_##array[0]));   \
                                                                                                         \
        if (temp_##array == NULL) {                                                                      \
            fprintf(stderr, "Realloc error\n");                                                          \
            return NULL;                                                                                 \
        }

    LIST_DUMP_BEFORE(list, dump_info_str)

    size_t list_size = list -> size;
    size_t new_size  = 2 * list_size - 1;

    SAFE_REALLOC(data)
    SAFE_REALLOC(next)
    SAFE_REALLOC(prev)

    list -> free = list_size;

    for (size_t idx = list_size; idx < new_size; idx++) {
        temp_prev[idx] = 0;
        temp_data[idx] = POISON;
        temp_next[idx] = idx + 1;
    }

    temp_next[new_size - 1] = 0;

    list -> size = new_size;
    list -> data = temp_data;
    list -> prev = temp_prev;
    list -> next = temp_next;

    LIST_DUMP_AFTER(list, dump_info_str)

    #undef SAFE_REALLOC

    return list;
}

size_t get_head (ListStruct* list) {
    return list -> next[0];
}

size_t get_tail (ListStruct* list) {
    return list -> prev[0];
}

size_t get_free (ListStruct* list) {
    return list -> free;
}

int get_data (ListStruct* list, size_t ind) {
    return list -> data[ind];
}

int get_next (ListStruct* list, size_t ind) {
    return list -> next[ind];
}

int get_prev (ListStruct* list, size_t ind) {
    return list -> prev[ind];
}

ListStruct List_Linearize (ListStruct* list) {

    LIST_DUMP_BEFORE(list, dump_info_str)

    ListStruct sorted_list = List_Ctor(list -> size - 1);
    
    size_t orig_idx = get_head(list);
    size_t sort_idx = 1;

    while (list->next[orig_idx] != 0 && sort_idx < list->size)
    {
        sorted_list.data[sort_idx] = list -> data[orig_idx];
        sorted_list.next[sort_idx] = sort_idx + 1;
        sorted_list.prev[sort_idx] = sort_idx - 1;

        orig_idx = list -> next[orig_idx]; 
        ++sort_idx;
    }

    sorted_list.data[sort_idx] = list -> data[orig_idx];
    sorted_list.next[sort_idx] = 0;
    sorted_list.prev[sort_idx] = sort_idx - 1;

    sorted_list.prev[0] = sort_idx;
    sorted_list.next[0] = 1;
    sorted_list.free    = sort_idx + 1;

    LIST_DUMP_AFTER(&sorted_list, dump_info_str)

    return sorted_list;
}