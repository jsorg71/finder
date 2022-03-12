
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <gtk/gtk.h>

#include "finder.h"
#include "finder_event.h"
#include "finder_gui.h"
#include "finder_gui_about.h"
#include "finder_mainwindow.h"

/*****************************************************************************/
int
main(int argc, char** argv)
{
    gtk_init(&argc, &argv);
    gtk_main();
    return 0;
}

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
int
gui_writeln(struct finder_info* fi, const char* msg)
{
    return 0;
}
