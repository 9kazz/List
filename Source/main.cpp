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

    Unit_tests();

/*ТОТ ЖЕ БЛОК КОДА В List_Dump ПОЧЕМУ-ТО НЕ ИСПОЛНЯЕТСЯ, ПРИШЛОСЬ НАПИСАТЬ В main -- ЗДЕСЬ РАБОТАЕТ???*/
    char create_image_cmd[256] = {0}; 
    for (size_t img_num = 1; img_num < 13; img_num++) {
        snprintf(create_image_cmd, 256, "dot dump_files/input_graphviz_%d.txt -T png -o dump_files/output_graphviz_%d.png", img_num, img_num);
        system(create_image_cmd);
    }
        
    atexit(LogFile_close);

    return 0;
}
