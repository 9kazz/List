#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "LogFile.h"
#include "utils.h"
#include "list.h"

ListErr List_Dump_graphviz(ListStruct list, FILE* output_file) {
    assert(output_file);

    ONDEBUG(&list)
    
    fprintf(output_file, "digraph List {\n");
    fprintf(output_file, "\trankdir = LR;\n");
    fprintf(output_file, "\tnode [shape = Mrecord, style = \"filled\", fillcolor = \"#e9e6f2ff\"];\n\n");

    //--------------------------------  D A T A -- N O D E S  -----------------------------------------------

    for (size_t el_ind = 0; el_ind < list.size; el_ind++) 
    {
        if (list.data[el_ind] == POISON)
            fprintf(output_file, "\tnode_%d [shape = Mrecord, label = \"ind = %d | POISON | { prev = %d | next = %d } \"];\n", el_ind, el_ind, list.prev[el_ind], list.next[el_ind]);

        else 
            fprintf(output_file, "\tnode_%d [shape = Mrecord, label = \"ind = %d | %d | { prev = %d | next = %d } \"];\n", el_ind, el_ind, list.data[el_ind], list.prev[el_ind], list.next[el_ind]);
    }

    fprintf(output_file, "\n");

    //------------------------------  O T H E R  -- N O D E S  ----------------------------------------------

    fprintf(output_file, "\tnode_head [shape = Mrecord, label = \"head | %d\", style = \"filled\", fillcolor = \"#FFC0C0\", color = red];\n",   list.head);
    fprintf(output_file, "\tnode_tail [shape = Mrecord, label = \"tail | %d\", style = \"filled\", fillcolor = \"#C0C0FF\", color = blue];\n",  list.tail);
    fprintf(output_file, "\tnode_free [shape = Mrecord, label = \"free | %d\", style = \"filled\", fillcolor = \"#C0FFC0\", color = green];\n", list.free);

    //--------------------------------- D A T A  --  E D G E  -----------------------------------------------

    fprintf(output_file, "\n\t{\n");
    fprintf(output_file, "\tedge [color = white, weight = 1000];\n\n");

    for (size_t el_ind = 0; el_ind < list.size - 1; el_ind++)
        fprintf(output_file, "\tnode_%d -> node_%d;\n", el_ind, el_ind + 1);

    fprintf(output_file, "\t}\n");  

    //--------------------------------  N E X T  --  E D G E  -----------------------------------------------

    fprintf(output_file, "\n\t{\n");
    fprintf(output_file, "\tedge [color = red, weight = 1];\n\n");

    for (size_t el_ind = list.head;  list.next[el_ind] != 0;  el_ind = list.next[el_ind])
        fprintf(output_file, "\tnode_%d -> node_%d;\n", el_ind, list.next[el_ind]);
    
    fprintf(output_file, "\tnode_%d -> node_0;\n", list.tail);

    fprintf(output_file, "\t}\n\n");


    //--------------------------------  P R E V  --  E D G E  -----------------------------------------------

    fprintf(output_file, "\n\t{\n");
    fprintf(output_file, "\tedge [color = blue, weight = 1];\n\n");

    for (size_t el_ind = list.tail;  list.prev[el_ind] != 0;  el_ind = list.prev[el_ind])
        fprintf(output_file, "\tnode_%d -> node_%d;\n", el_ind, list.prev[el_ind]);

    fprintf(output_file, "\tnode_%d -> node_0;\n", list.head);

    fprintf(output_file, "\t}\n\n");

    //--------------------------------  F R E E  --  E D G E  -----------------------------------------------

    fprintf(output_file, "\n\t{\n");
    fprintf(output_file, "\tedge [color = green, weight = 1];\n\n");

    size_t el_ind = list.free;

    while (list.next[el_ind] != 0) {
        fprintf(output_file, "\tnode_%d -> node_%d;\n", el_ind, list.next[el_ind]);
        el_ind = list.next[el_ind];
    }

    fprintf(output_file, "\tnode_%d -> node_0;\n", el_ind);

    fprintf(output_file, "\t}\n\n");

    //--------------------------------  O T H E R  --  E D G E  ---------------------------------------------

    fprintf(output_file, "\n\t{\n");

    fprintf(output_file, "\tnode_head -> node_%d [color = red];\n",   list.head);
    fprintf(output_file, "\tnode_tail -> node_%d [color = blue];\n",  list.tail);
    fprintf(output_file, "\tnode_free -> node_%d [color = green];\n", list.free);

    fprintf(output_file, "\t}\n\n");

    fprintf(output_file, "}\n");

    ONDEBUG(&list)

    return NO_ERRORS;
}

ListErr List_Dump_HTML(ListStruct list, const char* image, FILE* output_file) {
    assert(output_file);

    ONDEBUG(&list)

    fprintf(output_file, "<pre>\n");
    fprintf(output_file, "\t<h2> Dump of list: </h2>\n");

    fprintf(output_file, "<h4>\tsize: %d\n", list.size);
    fprintf(output_file, "\thead: %d\n", list.head);
    fprintf(output_file, "\ttail: %d\n", list.tail);
    fprintf(output_file, "\tfree: %d </h4>\n", list.free);

    //--------------------------------------------------------------------------------------------

    fprintf(output_file, "<h4>\tdata </h4>\n\t{\n");

    for (size_t el_ind = 0; el_ind < list.size; el_ind++)
        fprintf(output_file, "\t\t[%d]\t%d\n", el_ind, list.data[el_ind]);
    
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

ListErr List_Dump(ListStruct* list) {
    assert(list);

    size_t dump_num = list -> file_counter;

    if(dump_num > MAX_FILE_COUNT) {
        fprintf(stderr, "List_Dump: free files for dump are run out\n");
        return NO_MORE_FILES;
    }

    static const size_t MAX_SIZE_FILE_NAME      =  32;
    static const size_t MAX_DOT_CMD_LEN         = 256;

    char input_graphviz_str[MAX_SIZE_FILE_NAME] = {0};
    char image_str         [MAX_SIZE_FILE_NAME] = {0};
    char create_image_cmd  [MAX_DOT_CMD_LEN]    = {0};

//---------------------------------------------------------------------------------------------------------------
    int snprintf_check = snprintf(input_graphviz_str, MAX_SIZE_FILE_NAME, "input_graphviz_%d.txt", dump_num); 

    if (snprintf_check < 0) {
        fprintf(stderr, "List_dump: file name error\n");
        return FILE_NAME_ERR;
    }
//---------------------------------------------------------------------------------------------------------------
    snprintf_check = snprintf(image_str, MAX_SIZE_FILE_NAME, "output_graphviz_%d.png", dump_num); 

    if (snprintf_check < 0) {
        fprintf(stderr, "List_dump: file name error\n");
        return FILE_NAME_ERR;
    }
//---------------------------------------------------------------------------------------------------------------

    SAFE_FOPEN(input_graphviz, input_graphviz_str, "w");

    List_Dump_graphviz(*list, input_graphviz);

    snprintf_check = snprintf(create_image_cmd, MAX_DOT_CMD_LEN, "dot input_graphviz_%d.txt -T png -o output_graphviz_%d.png", dump_num, dump_num); 

    if (snprintf_check < 0) {
        fprintf(stderr, "List_dump: file name error\n");
        return FILE_NAME_ERR;
    }

    system(create_image_cmd);

    List_Dump_HTML(*list, image_str, Logfile_html);

    fclose(input_graphviz);

    ++(list -> file_counter);

    return NO_ERRORS;
}