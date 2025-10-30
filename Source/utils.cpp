#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "LogFile.h"
#include "utils.h"
#include "list.h"
#include "graphviz.h"

ListErr List_Dump(ListStruct* list, const char* dump_info) {
    assert(list);

    static size_t dump_num = 1;

    if(dump_num > MAX_FILE_COUNT) {
        fprintf(stderr, "List_Dump: free files for dump are run out\n");
        return NO_MORE_FILES;
    }

    static const size_t MAX_SIZE_FILE_NAME      =  64;
    static const size_t MAX_DOT_CMD_LEN         = 256;

    char input_graphviz_str[MAX_SIZE_FILE_NAME] = {0};
    char image_str         [MAX_SIZE_FILE_NAME] = {0};
    char create_image_cmd  [MAX_DOT_CMD_LEN]    = {0};

//---------------------------------------------------------------------------------------------------------------
    int snprintf_check = snprintf(input_graphviz_str, MAX_SIZE_FILE_NAME, "dump_files/input_graphviz_%d.txt", dump_num); 

    if (snprintf_check < 0) {
        fprintf(stderr, "List_dump: file name error\n");
        return FILE_NAME_ERR;
    }
//---------------------------------------------------------------------------------------------------------------
    snprintf_check = snprintf(image_str, MAX_SIZE_FILE_NAME, "dump_files/output_graphviz_%d.png", dump_num); 

    if (snprintf_check < 0) {
        fprintf(stderr, "List_dump: file name error\n");
        return FILE_NAME_ERR;
    }
//---------------------------------------------------------------------------------------------------------------

    SAFE_FOPEN(input_graphviz, input_graphviz_str, "w");

    List_Dump_graphviz(*list, input_graphviz);

    fclose(input_graphviz);

    snprintf_check = snprintf(create_image_cmd, MAX_DOT_CMD_LEN, "dot -Tpng dump_files/input_graphviz_%d.txt -o dump_files/output_graphviz_%d.png", dump_num, dump_num); 

    if (snprintf_check < 0) {
        fprintf(stderr, "List_dump: file name error\n");
        return FILE_NAME_ERR;
    }

    system(create_image_cmd);

    List_Dump_HTML(*list, image_str, dump_info, Logfile_html);

    ++dump_num;

    return NO_ERRORS;
}

ListErr List_Dump_graphviz(ListStruct list, FILE* output_file) {
    assert(output_file);

    ONDEBUG(&list)

    fprintf(output_file, "digraph List {\n");
    fprintf(output_file, "\trankdir = LR;\n");
    fprintf(output_file, "\tnode [" SHAPE STYLE " fillcolor = " BRIGHT_RED ", color = red];\n\n");

    //--------------------------------  D A T A -- N O D E S  -----------------------------------------------

    fprintf(output_file, "\tnode_0 [" SHAPE STYLE "fillcolor = " BRIGHT_YELLOW ", color = " YELLOW ", label = \"ind = 0 | POISON | { <tail> prev = %d | <head> next = %d } \"];\n", list.prev[0], list.next[0]);

    for (size_t el_ind = 1; el_ind < list.size; el_ind++) 
    {

        if (list.data[el_ind] == POISON && list.prev[el_ind] == 0) 
        {
            if (list.data[el_ind] == POISON)
                fprintf(output_file, "\tnode_%d [" SHAPE STYLE "fillcolor = " BRIGHT_GREEN ", color = green, label = \"ind = %d | POISON | { prev = %d | next = %d } \"];\n", el_ind, el_ind, list.prev[el_ind], list.next[el_ind]);

            else 
                fprintf(output_file, "\tnode_%d [" SHAPE STYLE "fillcolor = " BRIGHT_GREEN ", color = green, label = \"ind = %d | %d | { prev = %d | next = %d } \"];\n", el_ind, el_ind, list.data[el_ind], list.prev[el_ind], list.next[el_ind]); 
        } 
        
        else 
        {
            if (list.data[el_ind] == POISON)
                fprintf(output_file, "\tnode_%d [" SHAPE STYLE "fillcolor = " BRIGHT_BLUE ", color = blue, label = \"ind = %d | POISON | { prev = %d | next = %d } \"];\n", el_ind, el_ind, list.prev[el_ind], list.next[el_ind]);

            else 
                fprintf(output_file, "\tnode_%d [" SHAPE STYLE "fillcolor = " BRIGHT_BLUE ", color = blue, label = \"ind = %d | %d | { prev = %d | next = %d } \"];\n", el_ind, el_ind, list.data[el_ind], list.prev[el_ind], list.next[el_ind]);    
        }
    }

    fprintf(output_file, "\n");

    //------------------------------  O T H E R  -- N O D E S  ----------------------------------------------

    fprintf(output_file, "\tnode_head [" SHAPE " label = \"head | %d\"" STYLE "fillcolor = " BRIGHT_RED   " color = red];\n",   list.head);
    fprintf(output_file, "\tnode_tail [" SHAPE " label = \"tail | %d\"" STYLE "fillcolor = " BRIGHT_BLUE  ", color = blue];\n",  list.tail);
    fprintf(output_file, "\tnode_free [" SHAPE " label = \"free | %d\"" STYLE "fillcolor = " BRIGHT_GREEN ", color = green];\n", list.free);

    //--------------------------------- D A T A  --  E D G E  -----------------------------------------------

    fprintf(output_file, "\n\t{\n");
    fprintf(output_file, "\tedge [color = white, weight = 1000];\n\n");

    for (size_t el_ind = 0; el_ind < list.size - 1; el_ind++)
        fprintf(output_file, "\tnode_%d -> node_%d;\n", el_ind, el_ind + 1);

    fprintf(output_file, "\t}\n");  

    //--------------------------------  N E X T  --  E D G E  -----------------------------------------------

    fprintf(output_file, "\n\t{\n");
    fprintf(output_file, "\tedge [color = red, constraint = false];\n\n");

    for (size_t el_ind = list.head;  list.next[el_ind] != 0;  el_ind = list.next[el_ind])
        fprintf(output_file, "\tnode_%d -> node_%d;\n", el_ind, list.next[el_ind]);
    
    fprintf(output_file, "\tnode_%d -> node_0;\n", list.tail);

    fprintf(output_file, "\t}\n\n");


    //--------------------------------  P R E V  --  E D G E  -----------------------------------------------

    fprintf(output_file, "\n\t{\n");
    fprintf(output_file, "\tedge [color = blue, constraint = false];\n\n");

    for (size_t el_ind = list.tail;  list.prev[el_ind] != 0;  el_ind = list.prev[el_ind])
        fprintf(output_file, "\tnode_%d -> node_%d;\n", el_ind, list.prev[el_ind]);

    fprintf(output_file, "\tnode_%d -> node_0;\n", list.head);

    fprintf(output_file, "\t}\n\n");

    //--------------------------------  F R E E  --  E D G E  -----------------------------------------------

    fprintf(output_file, "\n\t{\n");
    fprintf(output_file, "\tedge [color = green, constraint = false];\n\n");

    size_t el_ind = list.free;

    while (list.next[el_ind] != 0) {
        fprintf(output_file, "\tnode_%d -> node_%d;\n", el_ind, list.next[el_ind]);
        el_ind = list.next[el_ind];
    }

    fprintf(output_file, "\tnode_%d -> node_0;\n", el_ind);

    fprintf(output_file, "\t}\n\n");

    //--------------------------------  O T H E R  --  E D G E  ---------------------------------------------

    fprintf(output_file, "\n\t{\n");

    fprintf(output_file, "\tnode_0: <head> -> node_head [color = red];\n");
    fprintf(output_file, "\tnode_0: <tail> -> node_tail [color = blue];\n");

    fprintf(output_file, "\tnode_head -> node_%d [color = red];\n",   list.head);
    fprintf(output_file, "\tnode_tail -> node_%d [color = blue];\n",  list.tail);
    fprintf(output_file, "\tnode_free -> node_%d [color = green];\n", list.free);

    fprintf(output_file, "\t}\n\n");

    fprintf(output_file, "}\n");

    ONDEBUG(&list)

    return NO_ERRORS;
}

ListErr List_Dump_HTML(ListStruct list, const char* image, const char* dump_info, FILE* output_file) {
    assert(output_file);

    ONDEBUG(&list)

    fprintf(output_file, "<pre>\n");
    fprintf(output_file, "\t<h2> Dump of list (%s): </h2>\n", dump_info);

    fprintf(output_file, "<h4>\tsize: %d\n", list.size);
    fprintf(output_file, "\thead: %d\n", list.head);
    fprintf(output_file, "\ttail: %d\n", list.tail);
    fprintf(output_file, "\tfree: %d </h4>\n", list.free);

    //--------------------------------------------------------------------------------------------

    fprintf(output_file, "<h4>\tdata </h4>\n\t{\n");

    for (size_t el_ind = 0; el_ind < list.size; el_ind++) {
        if (list.data[el_ind] == POISON)
            fprintf(output_file, "\t\t[%d]\tPOISON\n", el_ind);
        else
            fprintf(output_file, "\t\t[%d]\t%d\n", el_ind, list.data[el_ind]);
    }
    
    fprintf(output_file, "\t}\n");

    //--------------------------------------------------------------------------------------------

    fprintf(output_file, "<h4>\tnext </h4>\n\t{\n");

    for (size_t el_ind = 0; el_ind < list.size; el_ind++)
        fprintf(output_file, "\t\t[%d]\t%d\n", el_ind, list.next[el_ind]);
    
    fprintf(output_file, "\t}\n");

    //--------------------------------------------------------------------------------------------

    fprintf(output_file, "<h4>\tprev </h4>\n\t{\n");

    for (size_t el_ind = 0; el_ind < list.size; el_ind++)
        fprintf(output_file, "\t\t[%d]\t%d\n", el_ind, list.prev[el_ind]);
    
    fprintf(output_file, "\t}\n");

    //--------------------------------------------------------------------------------------------

    fprintf(output_file, "<h4>\tImage:</h4>\n");
    fprintf(output_file, "\t<img src = %s width = \"1000\" height = \"300\">\n\n", image);

    fprintf(output_file, "</pre>");

    ONDEBUG(&list)

    return NO_ERRORS;
}

ListErr Unit_tests(void) {
    ONDEBUG(list)

    const int SIZE_OF_LIST = 9;

    int test_result = UNIT_TEST_PASSED;

    int correct_data[SIZE_OF_LIST + 1] = {0};
    int correct_next[SIZE_OF_LIST + 1] = {0};
    int correct_prev[SIZE_OF_LIST + 1] = {0};

    ListStruct list = List_Ctor(SIZE_OF_LIST);

    #define ONE_TEST_CHECK                                                                 \
        test_result = One_test_check(list, correct_data, correct_next, correct_prev);      \
        if (test_result == UNIT_TEST_FAILED) {                                             \
            List_Dtor(&list);                                                              \
            return UNIT_TEST_FAILED;                                                       \
        }

    List_Insert_after(&list, 0, 10); // TODO: rename to POISON

        correct_data[0] =     POISON;       correct_next[0] = 1;      correct_prev[0] = 1;
		correct_data[1] =         10;       correct_next[1] = 0;      correct_prev[1] = 0;
		correct_data[2] =     POISON;       correct_next[2] = 3;      correct_prev[2] = 0;
		correct_data[3] =     POISON;       correct_next[3] = 4;      correct_prev[3] = 0;
		correct_data[4] =     POISON;       correct_next[4] = 5;      correct_prev[4] = 0;
		correct_data[5] =     POISON;       correct_next[5] = 6;      correct_prev[5] = 0;
		correct_data[6] =     POISON;       correct_next[6] = 7;      correct_prev[6] = 0;
		correct_data[7] =     POISON;       correct_next[7] = 8;      correct_prev[7] = 0;
		correct_data[8] =     POISON;       correct_next[8] = 9;      correct_prev[8] = 0;
        correct_data[9] =     POISON;       correct_next[9] = 0;      correct_prev[9] = 0;

    ONE_TEST_CHECK

    List_Insert_after(&list, 1, 20);

        correct_data[0] =     POISON;       correct_next[0] = 1;      correct_prev[0] = 2;
		correct_data[1] =         10;       correct_next[1] = 2;      correct_prev[1] = 0;
		correct_data[2] =         20;       correct_next[2] = 0;      correct_prev[2] = 1;
		correct_data[3] =     POISON;       correct_next[3] = 4;      correct_prev[3] = 0;
		correct_data[4] =     POISON;       correct_next[4] = 5;      correct_prev[4] = 0;
		correct_data[5] =     POISON;       correct_next[5] = 6;      correct_prev[5] = 0;
		correct_data[6] =     POISON;       correct_next[6] = 7;      correct_prev[6] = 0;
		correct_data[7] =     POISON;       correct_next[7] = 8;      correct_prev[7] = 0;
		correct_data[8] =     POISON;       correct_next[8] = 9;      correct_prev[8] = 0;
        correct_data[9] =     POISON;       correct_next[9] = 0;      correct_prev[9] = 0;

    ONE_TEST_CHECK

    List_Insert_after(&list, 1, 15);

        correct_data[0] =     POISON;       correct_next[0] = 1;      correct_prev[0] = 2;
		correct_data[1] =         10;       correct_next[1] = 3;      correct_prev[1] = 0;
		correct_data[2] =         20;       correct_next[2] = 0;      correct_prev[2] = 3;
		correct_data[3] =         15;       correct_next[3] = 2;      correct_prev[3] = 1;
		correct_data[4] =     POISON;       correct_next[4] = 5;      correct_prev[4] = 0;
		correct_data[5] =     POISON;       correct_next[5] = 6;      correct_prev[5] = 0;
		correct_data[6] =     POISON;       correct_next[6] = 7;      correct_prev[6] = 0;
		correct_data[7] =     POISON;       correct_next[7] = 8;      correct_prev[7] = 0;
		correct_data[8] =     POISON;       correct_next[8] = 9;      correct_prev[8] = 0;
        correct_data[9] =     POISON;       correct_next[9] = 0;      correct_prev[9] = 0;
    
    ONE_TEST_CHECK

    List_Delete (&list, 3);

        correct_data[0] =     POISON;       correct_next[0] = 1;      correct_prev[0] = 2;
		correct_data[1] =         10;       correct_next[1] = 2;      correct_prev[1] = 0;
		correct_data[2] =         20;       correct_next[2] = 0;      correct_prev[2] = 1;
		correct_data[3] =     POISON;       correct_next[3] = 4;      correct_prev[3] = 0;
		correct_data[4] =     POISON;       correct_next[4] = 5;      correct_prev[4] = 0;
		correct_data[5] =     POISON;       correct_next[5] = 6;      correct_prev[5] = 0;
		correct_data[6] =     POISON;       correct_next[6] = 7;      correct_prev[6] = 0;
		correct_data[7] =     POISON;       correct_next[7] = 8;      correct_prev[7] = 0;
		correct_data[8] =     POISON;       correct_next[8] = 9;      correct_prev[8] = 0;
        correct_data[9] =     POISON;       correct_next[9] = 0;      correct_prev[9] = 0;

    ONE_TEST_CHECK

    List_Insert_before(&list, 2, 15);

        correct_data[0] =     POISON;       correct_next[0] = 1;      correct_prev[0] = 2;
		correct_data[1] =         10;       correct_next[1] = 3;      correct_prev[1] = 0;
		correct_data[2] =         20;       correct_next[2] = 0;      correct_prev[2] = 3;
		correct_data[3] =         15;       correct_next[3] = 2;      correct_prev[3] = 1;
		correct_data[4] =     POISON;       correct_next[4] = 5;      correct_prev[4] = 0;
		correct_data[5] =     POISON;       correct_next[5] = 6;      correct_prev[5] = 0;
		correct_data[6] =     POISON;       correct_next[6] = 7;      correct_prev[6] = 0;
		correct_data[7] =     POISON;       correct_next[7] = 8;      correct_prev[7] = 0;
		correct_data[8] =     POISON;       correct_next[8] = 9;      correct_prev[8] = 0;
        correct_data[9] =     POISON;       correct_next[9] = 0;      correct_prev[9] = 0;

    ONE_TEST_CHECK

    List_Delete(&list, 2);

        correct_data[0] =     POISON;       correct_next[0] = 1;      correct_prev[0] = 3;
		correct_data[1] =         10;       correct_next[1] = 3;      correct_prev[1] = 0;
		correct_data[2] =     POISON;       correct_next[2] = 4;      correct_prev[2] = 0;
		correct_data[3] =         15;       correct_next[3] = 0;      correct_prev[3] = 1;
		correct_data[4] =     POISON;       correct_next[4] = 5;      correct_prev[4] = 0;
		correct_data[5] =     POISON;       correct_next[5] = 6;      correct_prev[5] = 0;
		correct_data[6] =     POISON;       correct_next[6] = 7;      correct_prev[6] = 0;
		correct_data[7] =     POISON;       correct_next[7] = 8;      correct_prev[7] = 0;
		correct_data[8] =     POISON;       correct_next[8] = 9;      correct_prev[8] = 0;
        correct_data[9] =     POISON;       correct_next[9] = 0;      correct_prev[9] = 0;

    ONE_TEST_CHECK

    #undef ONE_TEST_CHECK

    fprintf(LogFile, "Unit test passed successfully\n");
    List_Dtor(&list);
    return UNIT_TEST_PASSED;
}

ListErr One_test_check(ListStruct list, int* correct_data, int* correct_next, int* correct_prev) {
    ONDEBUG(&list)

    for (size_t el_ind = 0; el_ind < list.size - 1; el_ind++) {

        if (list.data[el_ind] != correct_data[el_ind] || 
            list.next[el_ind] != correct_next[el_ind] ||
            list.prev[el_ind] != correct_prev[el_ind]  )
        {
            fprintf(LogFile, "Unit test is failed at index [%d]\n", el_ind);
            
            //---------------------------------------------------------------------

            fprintf(LogFile, "data:\n");

            for (size_t el_num = 0; el_num < list.size; el_num++)
                fprintf(LogFile, "\t\t[%d]\t%d\n", el_num, list.data[el_num]);

            fprintf(LogFile, "Should be:\n");

            for (size_t el_num = 0; el_num < list.size; el_num++)
                fprintf(LogFile, "\t\t[%d]\t%d\n", el_num, correct_data[el_num]);

            //---------------------------------------------------------------------

            fprintf(LogFile, "next:\n");

            for (size_t el_num = 0; el_num < list.size; el_num++)
                fprintf(LogFile, "\t\t[%d]\t%d\n", el_num, list.next[el_num]);

            fprintf(LogFile, "Should be:\n");

            for (size_t el_num = 0; el_num < list.size; el_num++)
                fprintf(LogFile, "\t\t[%d]\t%d\n", el_num, correct_next[el_num]);

            //---------------------------------------------------------------------

            fprintf(LogFile, "prev:\n");

            for (size_t el_num = 0; el_num < list.size; el_num++)
                fprintf(LogFile, "\t\t[%d]\t%d\n", el_num, list.prev[el_num]);

            fprintf(LogFile, "Should be:\n");

            for (size_t el_num = 0; el_num < list.size; el_num++)
                fprintf(LogFile, "\t\t[%d]\t%d\n", el_num, correct_prev[el_num]);

            return UNIT_TEST_FAILED;
        }

    }

    ONDEBUG(&list)

    return UNIT_TEST_PASSED;
}