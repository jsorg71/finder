
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fx.h>

#include "finder_gui.h"

class MsgObject;

struct app_data
{
    FXApp* app;
    FXMainWindow* mw;
    FXButton* but1;
    FXButton* but2;
    FXTabBar* tb;
    FXTabItem* ti1;
    FXTabItem* ti2;
    FXTabItem* ti3;
    MsgObject* mo;
};

class MsgObject : public FXObject
{
    FXDECLARE(MsgObject)
public:
    MsgObject();
    long onDefault(FXObject* obj, FXSelector sel, void* ptr);
    long onPress(FXObject* obj, FXSelector sel, void* ptr);
    long onTabChange(FXObject* obj, FXSelector sel, void* ptr);
    struct app_data* ap;
    enum _ids
    {
        ID_BUTTON = 0,
        ID_TABBAR,
        ID_LAST
    } ids;
};

/*****************************************************************************/
MsgObject::MsgObject()
{
}

/*****************************************************************************/
long
MsgObject::onDefault(FXObject* obj, FXSelector sel, void* ptr)
{
    return FXObject::onDefault(obj, sel, ptr);
}

/*****************************************************************************/
long
MsgObject::onPress(FXObject* obj, FXSelector sel, void* ptr)
{
    printf("onPress obj %p sel %d ptr %p\n", obj, sel, ptr);
    if (obj == ap->but1)
    {
        printf("but1\n");
    }
    else
    {
        printf("but2\n");
    }
    return 0; 
}

/*****************************************************************************/
long
MsgObject::onTabChange(FXObject* obj, FXSelector sel, void* ptr)
{
    printf("onTabChange obj %p sel %d ptr %p\n", obj, sel, ptr);
    return 0;
}

FXDEFMAP(MsgObject) MsgObjectMap[] =
{
    FXMAPFUNC(SEL_COMMAND, MsgObject::ID_BUTTON, MsgObject::onPress),
    FXMAPFUNC(SEL_COMMAND, MsgObject::ID_TABBAR, MsgObject::onTabChange)
};

FXIMPLEMENT(MsgObject, FXObject, MsgObjectMap, ARRAYNUMBER(MsgObjectMap))

/*****************************************************************************/
void*
gui_create(int argc, char** argv)
{
    struct app_data* ap;

    ap = (struct app_data*)calloc(sizeof(struct app_data), 1);
    ap->app = new FXApp("Find", "Find");
    ap->app->init(argc, argv);
    ap->mo = new MsgObject();
    ap->mo->ap = ap;
    ap->mw = new FXMainWindow(ap->app, "Find", NULL, NULL, DECOR_ALL, 0, 0, 640, 480);
    ap->tb = new FXTabBar(ap->mw, ap->mo, MsgObject::ID_TABBAR, TABBOOK_NORMAL);
    ap->ti1 = new FXTabItem(ap->tb, "Name & Location", 0, TAB_TOP_NORMAL);
    ap->ti2 = new FXTabItem(ap->tb, "Date Modified", 0, TAB_TOP_NORMAL);
    ap->ti3 = new FXTabItem(ap->tb, "Advanced", 0, TAB_TOP_NORMAL);
    ap->but1 = new FXButton(ap->mw, "but1", NULL, ap->mo, MsgObject::ID_BUTTON);
    ap->but2 = new FXButton(ap->mw, "but2", NULL, ap->mo, MsgObject::ID_BUTTON);
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
    delete ap->mo;
    free(ap);
    return 0;
}

