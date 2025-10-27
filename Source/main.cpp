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

    ListStruct list = List_Ctor(8);

    List_Insert(&list, 0, 10);
    List_Insert(&list, 1, 20);
    List_Insert(&list, 2, 30);
    List_Insert(&list, 3, 40);
    List_Delete(&list, 2);

    List_Dtor(&list);

    /*ТОТ ЖЕ БЛОК КОДА В List_Dump ПОЧЕМУ */
    char create_image_cmd[256] = {0}; 
    for (size_t img_num = 1; img_num < list.file_counter; img_num++) {
        snprintf(create_image_cmd, 256, "dot input_graphviz_%d.txt -T png -o output_graphviz_%d.png", img_num, img_num);
        system(create_image_cmd);
    }
        
    atexit(LogFile_close);

    return 0;
}
