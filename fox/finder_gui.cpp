
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fx.h>
#include "fxkeys.h"

#include "finder.h"
#include "finder_gui.h"

struct app_data
{
    FXApp* app;
    FXMainWindow* mw;
    FXButton* but1;
    FXButton* but2;
    FXButton* but3;
    FXTabItem* ti1;
    FXTabItem* ti2;
    FXTabItem* ti3;
    FXGroupBox* gb2;
    FXFoldingList* fl;
    FXGroupBox* gb1;
    FXTabBook* tab_book;
    FXGroupBox* tabframe1;
    FXGroupBox* tabframe2;
    FXGroupBox* tabframe3;
    FXLabel* label1;
    FXLabel* label2;
    FXCheckButton* cb1;
    FXCheckButton* cb2;
    FXCheckButton* cb3;
    FXComboBox* combo1;
    FXComboBox* combo2;
    FXTextField* text1;
    FXDockSite* topdock;
    FXToolBarShell* tbs;
    FXMenuBar* mb;
    FXMenuPane* filemenu;
    FXMenuPane* helpmenu;
    FXStatusBar* sb;
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
    long onCmdExit(FXObject* obj, FXSelector sel, void* ptr);
    long onCmdHelp(FXObject* obj, FXSelector sel, void* ptr);
    struct app_data* ap;
    enum _ids
    {
        ID_BUTTON = 0,
        ID_TABBOOK,
        ID_FOLDINGLIST,
        ID_MAINWINDOW,
        ID_EXIT,
        ID_HELP,
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
    //printf("%d onDefault obj %p sel 0x%8.8x ptr %p\n", i1++, obj, sel, ptr);
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
        ap->fl->clearItems();
    }
    if (obj == ap->but2)
    {
        printf("but2\n");
    }
    if (obj == ap->but3)
    {
        printf("but3\n");
        ap->app->stop(0);
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

        ap->gb1->move(0, 22);
        ap->gb1->resize(width - 120, 180);

        ap->gb2->move(0, 200);
        ap->gb2->resize(width, height - 225);

        ap->but1->move(width - 110, 32);
        ap->but1->resize(100, 30);

        ap->but2->move(width - 110, 72);
        ap->but2->resize(100, 30);

        ap->but3->move(width - 110, 165);
        ap->but3->resize(100, 30);

        ap->label1->move(8, 8);
        ap->label1->resize(100, 24);

        ap->combo1->move(85, 8);
        ap->combo1->resize(400, 24);

        ap->label2->move(8, 41);
        ap->label2->resize(100, 24);

        ap->combo2->move(85, 41);
        ap->combo2->resize(400, 24);

        ap->cb1->move(8, 74);
        ap->cb1->resize(200, 24);

        ap->cb2->move(8, 107);
        ap->cb2->resize(200, 24);

        ap->cb3->move(216, 74);
        ap->cb3->resize(200, 24);

        ap->text1->move(85, 8);
        ap->text1->resize(400, 24);

    }
    return 0;
}

/*****************************************************************************/
long
MsgObject::onCmdExit(FXObject* obj, FXSelector sel, void* ptr)
{
    printf("MsgObject::onCmdExit:\n");
    ap->app->stop(0);
    return 0;
}

/*****************************************************************************/
long
MsgObject::onCmdHelp(FXObject* obj, FXSelector sel, void* ptr)
{
    printf("MsgObject::onCmdHelp:\n");
    return 0;
}

FXDEFMAP(MsgObject) MsgObjectMap[] =
{
    FXMAPFUNC(SEL_COMMAND, MsgObject::ID_BUTTON, MsgObject::onPress),
    FXMAPFUNC(SEL_COMMAND, MsgObject::ID_TABBOOK, MsgObject::onTabChange),
    FXMAPFUNC(SEL_CONFIGURE, MsgObject::ID_MAINWINDOW, MsgObject::onConfigure),
    FXMAPFUNC(SEL_TIMEOUT, MsgObject::ID_MAINWINDOW, MsgObject::onResizeTimeout),
    FXMAPFUNC(SEL_COMMAND, MsgObject::ID_EXIT, MsgObject::onCmdExit),
    FXMAPFUNC(SEL_COMMAND, MsgObject::ID_HELP, MsgObject::onCmdHelp)
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

    flags = LAYOUT_EXPLICIT;
    ap->gb1 = new FXGroupBox(ap->mw, "", flags);

    sel = MsgObject::ID_TABBOOK;
    flags = LAYOUT_FILL_X | LAYOUT_FILL_Y;
    ap->tab_book = new FXTabBook(ap->gb1, ap->mo, sel, flags);
    ap->ti1 = new FXTabItem(ap->tab_book, "Name/&Location");
    flags = LAYOUT_FILL_X | LAYOUT_FILL_Y | FRAME_THICK | FRAME_RAISED;
    ap->tabframe1 = new FXGroupBox(ap->tab_book, "", flags);
    ap->ti2 = new FXTabItem(ap->tab_book, "&Date Modified");
    ap->tabframe2 = new FXGroupBox(ap->tab_book, "", flags);
    ap->ti3 = new FXTabItem(ap->tab_book, "&Advanced");
    ap->tabframe3 = new FXGroupBox(ap->tab_book, "", flags);

    flags = LABEL_NORMAL | LAYOUT_EXPLICIT | JUSTIFY_LEFT;
    ap->label1 = new FXLabel(ap->tabframe1, "&Named:", NULL, flags);

    flags = FRAME_SUNKEN | FRAME_THICK | LAYOUT_EXPLICIT;
    ap->combo1 = new FXComboBox(ap->tabframe1, 0, NULL, 0, flags);

    flags = LABEL_NORMAL | LAYOUT_EXPLICIT | JUSTIFY_LEFT;
    ap->label2 = new FXLabel(ap->tabframe1, "Look &in:", NULL, flags);

    flags = FRAME_SUNKEN | FRAME_THICK | LAYOUT_EXPLICIT;
    ap->combo2 = new FXComboBox(ap->tabframe1, 0, NULL, 0, flags);

    flags = CHECKBUTTON_NORMAL | LAYOUT_EXPLICIT | JUSTIFY_LEFT;
    ap->cb1 = new FXCheckButton(ap->tabframe1, "Include subfolders", NULL, 0, flags);
    ap->cb1->setCheck(TRUE);
    ap->cb2 = new FXCheckButton(ap->tabframe1, "Case sensitive search", NULL, 0, flags);
    ap->cb3 = new FXCheckButton(ap->tabframe1, "Show hidden files", NULL, 0, flags);

    flags = TEXTFIELD_NORMAL;
    ap->text1 = new FXTextField(ap->tabframe2, 0, NULL, 0, flags);

    sel = MsgObject::ID_BUTTON;
    flags = BUTTON_NORMAL | LAYOUT_EXPLICIT;
    ap->but1 = new FXButton(ap->mw, "&Find", NULL, ap->mo, sel, flags);
    ap->but2 = new FXButton(ap->mw, "&Stop", NULL, ap->mo, sel, flags);
    ap->but2->disable();
    ap->but3 = new FXButton(ap->mw, "Exit", NULL, ap->mo, sel, flags);

    flags = LAYOUT_EXPLICIT;
    ap->gb2 = new FXGroupBox(ap->mw, "", flags);

    sel = MsgObject::ID_FOLDINGLIST;
    flags = FOLDINGLIST_NORMAL | LAYOUT_FILL_X | LAYOUT_FILL_Y;
    ap->fl = new FXFoldingList(ap->gb2, ap->mo, sel, flags);
    ap->fl->appendHeader("Name", 0, 100);
    ap->fl->appendHeader("In Subfolder", 0, 100);
    ap->fl->appendHeader("Size", 0, 100);
    ap->fl->appendHeader("Modified", 0, 100);

    flags = LAYOUT_SIDE_TOP | LAYOUT_FILL_X;
    ap->topdock = new FXDockSite(ap->mw, flags);

    flags = FRAME_RAISED;
    ap->tbs = new FXToolBarShell(ap->mw, flags);

    flags = LAYOUT_DOCK_NEXT | LAYOUT_SIDE_TOP | LAYOUT_FILL_X | FRAME_RAISED;
    ap->mb = new FXMenuBar(ap->topdock, ap->tbs, flags);

    ap->filemenu = new FXMenuPane(ap->mw);
    new FXMenuTitle(ap->mb, "&File", NULL, ap->filemenu);
    sel = MsgObject::ID_EXIT;
    new FXMenuCommand(ap->filemenu, "&Exit\t\tExit the application.", NULL, ap->mo, sel);

    ap->helpmenu = new FXMenuPane(ap->mw);
    new FXMenuTitle(ap->mb, "&Help", NULL, ap->helpmenu);
    sel = MsgObject::ID_HELP;
    new FXMenuCommand(ap->helpmenu, "&Help...\t\tDisplay help information.", NULL, ap->mo, sel);

    flags = LAYOUT_SIDE_BOTTOM | LAYOUT_FILL_X | STATUSBAR_WITH_DRAGCORNER |
            FRAME_RAISED;
    ap->sb = new FXStatusBar(ap->mw, flags);

    ap->app->create();
    ap->mw->show(PLACEMENT_SCREEN);
    return ap;
}

/*****************************************************************************/
int
gui_main_loop(void* han)
{
    struct app_data* ap;

    printf("gui_main_loop\n");
    ap = (struct app_data*)han;
    ap->app->run();
    return 0;
}

/*****************************************************************************/
int
gui_delete(void* han)
{
    struct app_data* ap;

    printf("gui_delete\n");
    ap = (struct app_data*)han;
    delete ap->app;
    delete ap->mo;
    free(ap);
    return 0;
}

