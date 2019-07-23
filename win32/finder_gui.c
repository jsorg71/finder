
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#include "finder.h"

struct finder_info g_fi;

/*****************************************************************************/
int
gui_set_event(struct finder_info* fi)
{
    return 0;
}

/*****************************************************************************/
int
gui_find_done(struct finder_info* fi)
{
    return 0;
}

/*****************************************************************************/
int
gui_add_one(struct finder_info* fi, const char* filename,
            const char* in_subfolder, FINDER_I64 size,
            const char* modified)
{
    return 0;
}

/*****************************************************************************/
int WINAPI
WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
        LPSTR lpCmdLine, int nCmdShow)
{
    memset(&g_fi, 0, sizeof(g_fi));
    if (gui_init(&g_fi) != 0)
    {
        MessageBox(NULL, "WinMain: gui_init failed", "finder", MB_OK);
    }
    writeln(&g_fi, "hello %d", 45);
    MessageBox(NULL, lpCmdLine, "finder", 0);
    return 0;
}