#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "LogFile.h"
#include "utils.h"
#include "list.h"

FILE* LogFile = fopen("LogFile.log", "w");

void LogFile_close(void) {
    fclose(LogFile);
}

int main() {
    assert(LogFile);

    FILE* output_graphviz = fopen("input_graphviz.txt", "w");
    assert(output_graphviz);

    ListStruct list = List_Ctor(8);

    List_Insert(&list, 0, 10);
    List_Insert(&list, 1, 20);
    List_Insert(&list, 2, 30);
    List_Insert(&list, 3, 40);

    List_Dump_graphviz(list, output_graphviz);

    List_Dtor(&list);

    fclose(output_graphviz);

    atexit(LogFile_close);

    return 0;
}
