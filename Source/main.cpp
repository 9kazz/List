#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "LogFile.h"
#include "utils.h"
#include "list.h"

FILE* LogFile      = fopen("LogFile.log",      "w");
FILE* Logfile_html = fopen("Logfile_html.htm", "w");

void LogFile_close(void) {
    fclose(LogFile);
    fclose(Logfile_html);
}
 
int main() {
    assert(LogFile);
    assert(Logfile_html);

    // ListStruct list = List_Ctor(9);

    Unit_tests();

    // List_Insert_after(&list, 0, 10);
    // List_Insert_after(&list, 1, 20);
    // List_Insert_after(&list, 1, 15);
    // List_Delete (&list, 3);
    // List_Insert_before(&list, 2, 15);
    // List_Delete(&list, 2);

    // List_Dtor(&list);

/*ТОТ ЖЕ БЛОК КОДА В List_Dump ПОЧЕМУ-ТО НЕ ИСПОЛНЯЕТСЯ, ПРИШЛОСЬ НАПИСАТЬ В main -- ЗДЕСЬ РАБОТАЕТ???*/
    char create_image_cmd[256] = {0}; 
    for (size_t img_num = 1; img_num < 12; img_num++) {
        snprintf(create_image_cmd, 256, "dot dump_files/input_graphviz_%d.txt -T png -o dump_files/output_graphviz_%d.png", img_num, img_num);
        system(create_image_cmd);
    }
        
    atexit(LogFile_close);

    return 0;
}
