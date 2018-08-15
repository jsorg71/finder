
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fx.h>

#include "finder_gui.h"

struct app_data
{
    FXApp* app;
    FXMainWindow* mw;
    FXButton* but;
};

/*****************************************************************************/
void*
gui_create(int argc, char** argv)
{
    struct app_data* ap;

    ap = (struct app_data*)calloc(sizeof(struct app_data), 1);
    ap->app = new FXApp("Hello", "FoxTest");
    ap->app->init(argc, argv);
    ap->mw = new FXMainWindow(ap->app, "Hello", NULL, NULL, DECOR_ALL);
    ap->but = new FXButton(ap->mw, "&Hello, World!", NULL, ap->app, FXApp::ID_QUIT);
    ap->app->create();
    ap->mw->show(PLACEMENT_SCREEN);
    return ap;
}

/*****************************************************************************/
int
gui_main_loop(void* han)
{
    struct app_data* ap;

    ap = (struct app_data*)han;
    ap->app->run();
    return 0;
}

/*****************************************************************************/
int
gui_delete(void* han)
{
    struct app_data* ap;

    ap = (struct app_data*)han;
    delete ap->app;
    free(ap);
    return 0;
}

