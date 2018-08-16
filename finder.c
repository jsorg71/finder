
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "finder.h"
#include "finder_gui.h"

/*****************************************************************************/
int
main(int argc, char** argv)
{
    void* han;

    han = gui_create(argc, argv);
    gui_main_loop(han);
    gui_delete(han);
    return 0;
}

