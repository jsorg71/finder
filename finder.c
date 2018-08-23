
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>

#include "finder.h"
#include "finder_gui.h"

/*****************************************************************************/
int
writeln(struct finder_info* fi, const char* format, ...)
{
    va_list ap;
    char* log_line;

    log_line = (char*)malloc(1024);
    va_start(ap, format);
    vsnprintf(log_line, 1024 - 2, format, ap);
    va_end(ap);
    printf("%s\n", log_line);
    free(log_line);
    return 0;
}

/*****************************************************************************/
int
start_find(struct finder_info* fi)
{
    printf("start_find:\n");
    printf("  named [%s]\n", fi->named);
    printf("  look_in [%s]\n", fi->look_in);
    printf("  include_subfolders %d\n", fi->include_subfolders);
    printf("  case_sensitive %d\n", fi->case_sensitive);
    printf("  show_hidden %d\n", fi->show_hidden);
    gui_set_event(fi);
    return 0;
}

/*****************************************************************************/
int
stop_find(struct finder_info* fi)
{
    printf("stop_find:\n");
    return 0; 
}

/*****************************************************************************/
int
event_callback(struct finder_info* fi)
{
    printf("event_callback:\n");
    return 0;
}

