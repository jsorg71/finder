
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fx.h>

#include "finder.h"
#include "finder_event.h"
#include "finder_gui.h"

struct app_data
{
    struct finder_info* fi;
    FXApp* app;
    FXMainWindow* mw;
    FXButton* but1;
    FXButton* but2;
    FXButton* but3;
    FXButton* but4;
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
    void* gui_event;
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
    long onCmdAbout(FXObject* obj, FXSelector sel, void* ptr);
    long onEvent1(FXObject* obj, FXSelector sel, void* ptr);
    struct app_data* ap;
    enum _ids
    {
        ID_BUTTON = 0,
        ID_TABBOOK,
        ID_FOLDINGLIST,
        ID_MAINWINDOW,
        ID_EXIT,
        ID_HELP,
        ID_ABOUT,
        ID_SOCKET,
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
    FXString str1;

    writeln(ap->fi, "onPress obj %p sel %d ptr %p", obj, sel, ptr);
    if (obj == ap->but1)
    {
        writeln(ap->fi, "but1");
        ap->fl->clearItems();
        str1 = ap->combo1->getText();
        snprintf(ap->fi->named, sizeof(ap->fi->named), "%s", str1.text());
        str1 = ap->combo2->getText();
        snprintf(ap->fi->look_in, sizeof(ap->fi->look_in), "%s", str1.text());
        ap->fi->include_subfolders = ap->cb1->getCheck();
        ap->fi->case_sensitive = ap->cb2->getCheck();
        ap->fi->show_hidden = ap->cb3->getCheck();
        start_find(ap->fi);
        ap->but1->disable();
        ap->but2->enable();
    }
    if (obj == ap->but2)
    {
        writeln(ap->fi, "but2");
        stop_find(ap->fi);
    }
    if (obj == ap->but3)
    {
        writeln(ap->fi, "but3");
        ap->app->stop(0);
    }
    if (obj == ap->but4)
    {
        writeln(ap->fi, "but4");
        str1 = ap->combo2->getText();
        str1 = FXDirDialog::getOpenDirectory(ap->mw, "Select Look In directory", str1);
        ap->combo2->setText(str1);
    }

    return 0;
}

/*****************************************************************************/
long
MsgObject::onTabChange(FXObject* obj, FXSelector sel, void* ptr)
{
    writeln(ap->fi, "onTabChange obj %p sel %d ptr %p", obj, sel, ptr);
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
        //writeln(ap->fi, "MsgObject::onResizeTimeout: resized to %dx%d, was %dx%d",
        //        width, height, ap->width, ap->height);
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
        ap->combo2->resize(340, 24);

        ap->but4->move(340 + 85 + 2, 41);
        ap->but4->resize(60, 24);

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
    writeln(ap->fi, "MsgObject::onCmdExit:");
    ap->app->stop(0);
    return 0;
}

/*****************************************************************************/
long
MsgObject::onCmdHelp(FXObject* obj, FXSelector sel, void* ptr)
{
    writeln(ap->fi, "MsgObject::onCmdHelp:");
    return 0;
}

/*****************************************************************************/
long
MsgObject::onCmdAbout(FXObject* obj, FXSelector sel, void* ptr)
{
    writeln(ap->fi, "MsgObject::onCmdAbout:");
    return 0;
}

/*****************************************************************************/
long
MsgObject::onEvent1(FXObject* obj, FXSelector sel, void* ptr)
{
    finder_event_clear(ap->gui_event);
    event_callback(ap->fi);
    ap->app->flush(1);
    ap->app->runWhileEvents();
    return 0;
}

FXDEFMAP(MsgObject) MsgObjectMap[] =
{
    FXMAPFUNC(SEL_COMMAND, MsgObject::ID_BUTTON, MsgObject::onPress),
    FXMAPFUNC(SEL_COMMAND, MsgObject::ID_TABBOOK, MsgObject::onTabChange),
    FXMAPFUNC(SEL_CONFIGURE, MsgObject::ID_MAINWINDOW, MsgObject::onConfigure),
    FXMAPFUNC(SEL_TIMEOUT, MsgObject::ID_MAINWINDOW, MsgObject::onResizeTimeout),
    FXMAPFUNC(SEL_COMMAND, MsgObject::ID_EXIT, MsgObject::onCmdExit),
    FXMAPFUNC(SEL_COMMAND, MsgObject::ID_HELP, MsgObject::onCmdHelp),
    FXMAPFUNC(SEL_COMMAND, MsgObject::ID_ABOUT, MsgObject::onCmdAbout),
    FXMAPFUNC(SEL_IO_READ, MsgObject::ID_SOCKET, MsgObject::onEvent1)
};

FXIMPLEMENT(MsgObject, FXObject, MsgObjectMap, ARRAYNUMBER(MsgObjectMap))

/*****************************************************************************/
static int
gui_create(int argc, char** argv, struct finder_info** fi)
{
    struct app_data* ap;
    FXuint flags;
    FXSelector sel;
    FXCursor* cur;
    FXInputHandle ih;

    *fi = (struct finder_info*)calloc(1, sizeof(struct finder_info));
    ap = (struct app_data*)calloc(1, sizeof(struct app_data));
    (*fi)->gui_obj = ap;
    ap->mo = new MsgObject();
    ap->mo->ap = ap;
    ap->fi = *fi;
    ap->fi->gui_obj = ap;
    ap->app = new FXApp("Finder", "Finder");
    cur = new FXCursor(ap->app, FX::CURSOR_ARROW);
    ap->app->setDefaultCursor(DEF_RARROW_CURSOR, cur);
    ap->app->init(argc, argv);
    ap->mw = new FXMainWindow(ap->app, "Finder", NULL, NULL, DECOR_ALL, 0, 0, 640, 480);
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

    sel = MsgObject::ID_BUTTON;
    flags = BUTTON_NORMAL | LAYOUT_EXPLICIT;
    ap->but4 = new FXButton(ap->tabframe1, "&Browse", NULL, ap->mo, sel, flags);

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
    sel = MsgObject::ID_ABOUT;
    new FXMenuCommand(ap->helpmenu, "&About\t\tDisplay version information.", NULL, ap->mo, sel);

    flags = LAYOUT_SIDE_BOTTOM | LAYOUT_FILL_X | STATUSBAR_WITH_DRAGCORNER | FRAME_RAISED;
    ap->sb = new FXStatusBar(ap->mw, flags);

    ap->app->create();
    ap->mw->show(PLACEMENT_SCREEN);

    finder_event_create(&(ap->gui_event));
    ih = (FXInputHandle)finder_event_get_wait_obj(ap->gui_event);
    ap->app->addInput(ih, INPUT_READ, ap->mo, MsgObject::ID_SOCKET);

    return 0;
}

/*****************************************************************************/
static int
gui_main_loop(struct finder_info* fi)
{
    struct app_data* ap;

    ap = (struct app_data*)(fi->gui_obj);
    writeln(ap->fi, "gui_main_loop");
    gui_init(ap->fi);
    ap->app->run();
    gui_deinit(ap->fi);
    return 0;
}

/*****************************************************************************/
static int
gui_delete(struct finder_info* fi)
{
    struct app_data* ap;

    ap = (struct app_data*)(fi->gui_obj);
    writeln(ap->fi, "gui_delete");
    delete ap->app;
    delete ap->mo;
    finder_event_delete(ap->gui_event);
    free(ap);
    free(fi);
    return 0;
}

/*****************************************************************************/
int
gui_set_event(struct finder_info* fi)
{
    struct app_data* ap;

    //writeln(fi, "gui_set_event");
    ap = (struct app_data*)(fi->gui_obj);
    finder_event_set(ap->gui_event);
    return 0;
}

/*****************************************************************************/
int
main(int argc, char** argv)
{
    struct finder_info* fi;

    gui_create(argc, argv, &fi);
    gui_main_loop(fi);
    gui_delete(fi);
    return 0;
}

/*****************************************************************************/
int
gui_find_done(struct finder_info* fi)
{
    struct app_data* ap;

    writeln(fi, "gui_find_done");
    ap = (struct app_data*)(fi->gui_obj);
    ap->but1->enable();
    ap->but2->disable();
    return 0;
}

/*****************************************************************************/
int
gui_add_one(struct finder_info* fi, const char* filename)
{
    struct app_data* ap;
    FXFoldingItem* folding_item;

    //writeln(fi, "gui_add_one");
    ap = (struct app_data*)(fi->gui_obj);
    folding_item = new FXFoldingItem(filename);
    ap->fl->appendItem(NULL, folding_item);
    return 0;
}
