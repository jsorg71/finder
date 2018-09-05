
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "finder_file.h"

#define FINDER_FILE_MAX_LINE_BYTES 2048

/*****************************************************************************/
int
finder_file_read_sections(const char* file_name, void* names_list)
{
    return 0;
}

/*****************************************************************************/
int
finder_file_read_section(const char* file_name, const char* section,
                         void* names_list, void* values_list)
{
    return 0;
}

/*****************************************************************************/
int
finder_file_write_section(const char* file_name, const char* section,
                          void* names_list, void* values_list)
{
    return 0;
}

