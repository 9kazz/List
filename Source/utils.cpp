#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "LogFile.h"
#include "utils.h"
#include "list.h"

ListErr List_Dump_graphviz(ListStruct list, FILE* output_file) {
    // List_Verify(list);
    
    fprintf(output_file, "digraph List {\n");
    fprintf(output_file, "\trankdir = LR;\n");
    fprintf(output_file, "\tnode [shape = Mrecord];\n\n");

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
    
    // //--------------------------------------  P R E V  ---------------------------------------------------

    // fprintf(output_file, "\n\t{\n");

    // for (size_t el_ind = 0; el_ind < list.size; el_ind++)
    //     fprintf(output_file, "\tnode_%d -> node_%d;\n", el_ind, list.prev[el_ind]);

    // fprintf(output_file, "\t}\n");


    fprintf(output_file, "}\n");

    return 0;
}