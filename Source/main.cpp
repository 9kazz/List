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
        
    atexit(LogFile_close);

    return 0;
}
