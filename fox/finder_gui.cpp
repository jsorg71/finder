
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fx.h>

#include "finder.h"
#include "finder_gui.h"

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
    FXFoldingList* fl;
    FXGroupBox* gb;
    FXTabBook* tab_book;
    FXVerticalFrame* tabframe;
    class MsgObject* mo;
    int width;
    int height;
};

class MsgObject : public FXObject
{
    FXDECLARE(MsgObject)
public:
    MsgObject();
    long onDefault(FXObject* obj, FXSelector sel, void* ptr);
    long onPress(FXObject* obj, FXSelector sel, void* ptr);
    long onTabChange(FXObject* obj, FXSelector sel, void* ptr);
    long onConfigure(FXObject* obj, FXSelector sel, void* ptr);
    long onResizeTimeout(FXObject* obj, FXSelector sel, void* ptr);
    struct app_data* ap;
    enum _ids
    {
        ID_BUTTON = 0,
        ID_TABBAR,
        ID_FOLDINGLIST,
        ID_MAINWINDOW,
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
    //static int i1;
    //printf("%d onDefault obj %p sel %d ptr %p\n", i1++, obj, sel, ptr);
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

/*****************************************************************************/
long
MsgObject::onConfigure(FXObject* obj, FXSelector sel, void* ptr)
{
    ap->app->addTimeout(ap->mo, MsgObject::ID_MAINWINDOW, 0, NULL);
    return 0;
}

/*****************************************************************************/
long
MsgObject::onResizeTimeout(FXObject* obj, FXSelector sel, void* ptr)
{
    FXint width;
    FXint height;

    width = ap->mw->getWidth();
    height = ap->mw->getHeight();
    if ((width != ap->width) || (height != ap->height))
    {
        printf("MsgObject::onResizeTimeout: resized to %dx%d, was %dx%d\n",
               width, height, ap->width, ap->height);
        ap->width = width;
        ap->height = height;

        ap->gb->move(10, 10);
        ap->gb->resize(width - 20, height - 20);

        ap->but1->move(width - 100, height - 100);
        ap->but1->resize(100, 100);

        ap->but2->move(width - 100, height - 200);
        ap->but2->resize(100, 100);

        ap->fl->move(width - 100, height - 300);
        ap->fl->resize(100, 100);

    }
    return 0;
}

FXDEFMAP(MsgObject) MsgObjectMap[] =
{
    FXMAPFUNC(SEL_COMMAND, MsgObject::ID_BUTTON, MsgObject::onPress),
    FXMAPFUNC(SEL_COMMAND, MsgObject::ID_TABBAR, MsgObject::onTabChange),
    FXMAPFUNC(SEL_CONFIGURE, MsgObject::ID_MAINWINDOW, MsgObject::onConfigure),
    FXMAPFUNC(SEL_TIMEOUT, MsgObject::ID_MAINWINDOW, MsgObject::onResizeTimeout)
};

FXIMPLEMENT(MsgObject, FXObject, MsgObjectMap, ARRAYNUMBER(MsgObjectMap))

/*****************************************************************************/
void*
gui_create(int argc, char** argv)
{
    struct app_data* ap;
    FXuint flags;
    FXSelector sel;

    ap = (struct app_data*)calloc(sizeof(struct app_data), 1);
    ap->app = new FXApp("Find", "Find");
    ap->app->init(argc, argv);
    ap->mo = new MsgObject();
    ap->mo->ap = ap;
    ap->mw = new FXMainWindow(ap->app, "Find", NULL, NULL, DECOR_ALL, 0, 0, 640, 480);
    ap->mw->setTarget(ap->mo);
    ap->mw->setSelector(MsgObject::ID_MAINWINDOW);

    flags = FRAME_GROOVE | LAYOUT_EXPLICIT;
    ap->gb = new FXGroupBox(ap->mw, "hi", flags);

    ap->tab_book = new FXTabBook(ap->gb,NULL,0,LAYOUT_FILL_X|LAYOUT_FILL_Y,0,0,0,0,0,0,0,0);
    ap->ti1 = new FXTabItem(ap->tab_book, "Name & Location1");
    ap->tabframe = new FXVerticalFrame(ap->tab_book,LAYOUT_FILL_X|LAYOUT_FILL_Y|FRAME_THICK|FRAME_RAISED);
    ap->ti1 = new FXTabItem(ap->tab_book, "Name & Location2");
    ap->tabframe = new FXVerticalFrame(ap->tab_book,LAYOUT_FILL_X|LAYOUT_FILL_Y);
    ap->ti1 = new FXTabItem(ap->tab_book, "Name & Location3");
    ap->tabframe = new FXVerticalFrame(ap->tab_book,LAYOUT_FILL_X|LAYOUT_FILL_Y|FRAME_THICK|FRAME_RAISED);
    //ap->ti2 = new FXTabItem(ap->tab_book, "Date Modified", 0, flags);
    //ap->ti3 = new FXTabItem(ap->tab_book, "Advanced", 0, flags);

#if 0
    sel = MsgObject::ID_TABBAR;
    flags = TABBOOK_NORMAL;
    ap->tb = new FXTabBar(ap->gb, ap->mo, sel, flags);
    //flags = TAB_TOP_NORMAL | LAYOUT_EXPLICIT;
    flags = TAB_TOP_NORMAL;
    ap->ti1 = new FXTabItem(ap->tb, "Name & Location", 0, flags);
    ap->ti2 = new FXTabItem(ap->tb, "Date Modified", 0, flags);
    ap->ti3 = new FXTabItem(ap->tb, "Advanced", 0, flags);
#endif
    sel = MsgObject::ID_BUTTON;
    flags = BUTTON_NORMAL | LAYOUT_EXPLICIT;
    ap->but1 = new FXButton(ap->mw, "but1", NULL, ap->mo, sel, flags);
    ap->but2 = new FXButton(ap->mw, "but2", NULL, ap->mo, sel, flags);

    sel = MsgObject::ID_FOLDINGLIST;
    flags = FOLDINGLIST_NORMAL | LAYOUT_EXPLICIT;
    ap->fl = new FXFoldingList(ap->mw, ap->mo, sel, flags);

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

