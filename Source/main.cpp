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

    ListStruct list = List_Ctor(10);
    
    List_Insert_after(&list, 0, 10); 
    List_Insert_after(&list, 1, 20);
    List_Insert_after(&list, 1, 15);
    
    ListStruct sorted_list = List_Linearize(&list);

    // List_Delete (&list, 3);
    // List_Insert_before(&list, 2, 15);
    // List_Delete(&list, 2);

    List_Dtor(&list);
    List_Dtor(&sorted_list);
    
    // Unit_tests();
        
    atexit(LogFile_close);

    return 0;
}
