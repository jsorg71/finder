/**
 * finder: A file finder GUI application.
 *
 * Copyright (C) Jay Sorg 2018-2022
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fx.h>

#include "finder.h"
#include "finder_event.h"
#include "finder_gui.h"
#include "finder_gui_about.h"
#include "finder_mainwindow.h"

class ItemObject : public FXObject
{
    FXDECLARE(ItemObject)
public:
    FXString filename;
    FXString in_subfolder;
    FINDER_I64 size;
    FXString size_text;
    FXString modified;
};

FXDEFMAP(ItemObject) ItemObjectMap[] =
{
};

FXIMPLEMENT(ItemObject, FXObject, ItemObjectMap, ARRAYNUMBER(ItemObjectMap))

class GUIObject : public FXObject
{
    FXDECLARE(GUIObject)
public:
    GUIObject();
    GUIObject(int argc, char** argv, struct finder_info* fi);
    virtual ~GUIObject();
    int findDone();
    int setEvent();
    int mainLoop();
    int loadCombo(FXComboBox* cb, const char* section, const char* key_prefix);
    int loadCheckbox(FXCheckButton* cb, const char* section, const char* key, FXbool default_checked);
    int loadFromReg();
    int saveCombo(FXComboBox* cb, const char* section, const char* key_prefix);
    int saveCheckbox(FXCheckButton* cb, const char* section, const char* key, FXbool default_checked);
    int saveToReg();
    int addOne(const char* filename, const char* in_subfolder,
               FINDER_I64 size, const char* modified);
public:
    long onDefault(FXObject* obj, FXSelector sel, void* ptr);
    long onPress(FXObject* obj, FXSelector sel, void* ptr);
    long onTabChange(FXObject* obj, FXSelector sel, void* ptr);
    long onCmdConfigure(FXObject* obj, FXSelector sel, void* ptr);
    long onCmdExit(FXObject* obj, FXSelector sel, void* ptr);
    long onCmdHelp(FXObject* obj, FXSelector sel, void* ptr);
    long onCmdAbout(FXObject* obj, FXSelector sel, void* ptr);
    long onEvent1(FXObject* obj, FXSelector sel, void* ptr);
    long onEventTimeout(FXObject* obj, FXSelector sel, void* ptr);
    long onFoldingListHeader(FXObject* obj, FXSelector sel, void* ptr);
    long onFoldingListHeaderClick(FXObject* obj, FXSelector sel, void* ptr);
    long onFoldingListItemDelete(FXObject* obj, FXSelector sel, void* ptr);
    long onFLRightMouseUp(FXObject* obj, FXSelector sel, void* ptr);
    long onCopyFilename(FXObject* obj, FXSelector sel, void* ptr);
    long onCopyFullPath(FXObject* obj, FXSelector sel, void* ptr);
    long onClose(FXObject* obj, FXSelector sel, void* ptr);
    long onClipboardLost(FXObject* obj, FXSelector sel, void* ptr);
    long onClipboardGained(FXObject* obj, FXSelector sel, void* ptr);
    long onClipboardRequest(FXObject* obj, FXSelector sel, void* ptr);
    long onBeginDrag(FXObject* obj, FXSelector sel, void* ptr);
    long onEndDrag(FXObject* obj, FXSelector sel, void* ptr);
    long onDragged(FXObject* obj, FXSelector sel, void* ptr);
    long onDNDRequest(FXObject* obj, FXSelector sel, void* ptr);
    long onRadioButton(FXObject* obj, FXSelector sel, void* ptr);
    enum _ids
    {
        ID_BUTTON = 1,
        ID_TABBOOK,
        ID_FOLDINGLIST,
        ID_MAINWINDOW,
        ID_MAINWINDOW1,
        ID_EXIT,
        ID_HELP,
        ID_ABOUT,
        ID_SOCKET,
        ID_FOLDINGLISTHEADER,
        ID_COPY_FILENAME,
        ID_COPY_FULL_PATH,
        ID_RADIOBUTTON,
        ID_LAST
    };
public:
    struct finder_info* m_fi;
    FXApp* m_app;
    FXMutex* m_mutex1;
    FXFinderMainWindow* m_mw;
    FXButton* m_but1;
    FXButton* m_but2;
    FXButton* m_but3;
    FXTabItem* m_ti1;
    FXTabItem* m_ti2;
    FXTabItem* m_ti3;
    FXGroupBox* m_gb1;
    FXGroupBox* m_gb2;
    FXFoldingList* m_fl;
    FXHeader* m_flh;
    FXTabBook* m_tab_book;
    FXGroupBox* m_tabframe1;
    FXGroupBox* m_tabframe2;
    FXGroupBox* m_tabframe3;
    FXDockSite* m_topdock;
    FXToolBarShell* m_tbs;
    FXMenuBar* m_mb;
    FXMenuPane* m_filemenu;
    FXMenuPane* m_helpmenu;
    FXStatusBar* m_sb;
    FXLabel* m_sbl1;
    FXMenuPane* m_fl_popup;
    struct _m_name_tab
    {
        FXLabel* m_label1;
        FXComboBox* m_combo1;
        FXLabel* m_label2;
        FXComboBox* m_combo2;
        FXButton* m_but1;
        FXCheckButton* m_cb1;
        FXCheckButton* m_cb2;
        FXCheckButton* m_cb3;
    } m_name_tab;
    struct _m_date_tab
    {
        FXRadioButton* m_rb1;
        FXRadioButton* m_rb2;
        FXComboBox* m_combo1;
        FXRadioButton* m_rb3;
        FXRadioButton* m_rb4;
        FXRadioButton* m_rb5;
        FXSpinner* m_spinner1;
        FXSpinner* m_spinner2;
        FXLabel* m_label1;
        FXLabel* m_label2;
    } m_date_tab;
    struct _m_adva_tab
    {
        FXCheckButton* m_cb1;
        FXComboBox* m_combo1;
        FXCheckButton* m_cb2;
    } m_adva_tab;
    void* m_gui_event;
    int m_sort_order;
    int m_last_header_click_mstime;
    FXDragType m_dnd_types[8];
    int m_width;
    int m_height;
    FXString m_dnd_str;
};

/*****************************************************************************/
GUIObject::GUIObject() : FXObject()
{
    LOGLN0((m_fi, LOG_INFO, LOGS "default", LOGP));
}

/*****************************************************************************/
GUIObject::GUIObject(int argc, char** argv, struct finder_info* fi) : FXObject()
{
    FXuint flags;
    FXSelector sel;
    FXCursor* cur;
    FXInputHandle ih;

    LOGLN0((m_fi, LOG_INFO, LOGS, LOGP));

    m_fi = fi;
    m_sort_order = 0;
    m_last_header_click_mstime = 0;
    memset(m_dnd_types, 0, sizeof(m_dnd_types));
    m_width = 0;
    m_height = 0;
    m_dnd_str = "";

    m_app = new FXApp("Finder", "Finder");
    m_mutex1 = new FXMutex();
    cur = new FXCursor(m_app, FX::CURSOR_ARROW);
    m_app->setDefaultCursor(DEF_RARROW_CURSOR, cur);
    m_app->init(argc, argv);
    m_mw = new FXFinderMainWindow(m_app);
    m_mw->setTarget(this);
    m_mw->setSelector(GUIObject::ID_MAINWINDOW);

    flags = LAYOUT_EXPLICIT;
    m_gb1 = new FXGroupBox(m_mw, "", flags);

    sel = GUIObject::ID_TABBOOK;
    flags = LAYOUT_FILL_X | LAYOUT_FILL_Y;
    m_tab_book = new FXTabBook(m_gb1, this, sel, flags);
    m_ti1 = new FXTabItem(m_tab_book, "Name/&Location");
    flags = LAYOUT_FILL_X | LAYOUT_FILL_Y | FRAME_THICK | FRAME_RAISED;
    m_tabframe1 = new FXGroupBox(m_tab_book, "", flags);
    m_ti2 = new FXTabItem(m_tab_book, "&Date Modified");
    m_tabframe2 = new FXGroupBox(m_tab_book, "", flags);
    m_ti3 = new FXTabItem(m_tab_book, "&Advanced");
    m_tabframe3 = new FXGroupBox(m_tab_book, "", flags);

    flags = LABEL_NORMAL | LAYOUT_EXPLICIT | JUSTIFY_LEFT;
    m_name_tab.m_label1 = new FXLabel(m_tabframe1, "&Named:", NULL, flags);

    flags = FRAME_SUNKEN | FRAME_THICK | LAYOUT_EXPLICIT;
    m_name_tab.m_combo1 = new FXComboBox(m_tabframe1, 0, NULL, 0, flags);
    m_name_tab.m_combo1->setNumVisible(10);

    flags = LABEL_NORMAL | LAYOUT_EXPLICIT | JUSTIFY_LEFT;
    m_name_tab.m_label2 = new FXLabel(m_tabframe1, "Look &in:", NULL, flags);

    flags = FRAME_SUNKEN | FRAME_THICK | LAYOUT_EXPLICIT;
    m_name_tab.m_combo2 = new FXComboBox(m_tabframe1, 0, NULL, 0, flags);
    m_name_tab.m_combo2->setNumVisible(10);

    sel = GUIObject::ID_BUTTON;
    flags = BUTTON_NORMAL | LAYOUT_EXPLICIT;
    m_name_tab.m_but1 = new FXButton(m_tabframe1, "&Browse", NULL, this, sel, flags);

    flags = CHECKBUTTON_NORMAL | LAYOUT_EXPLICIT | JUSTIFY_LEFT;
    m_name_tab.m_cb1 = new FXCheckButton(m_tabframe1, "Include subfolders", NULL, 0, flags);
    m_name_tab.m_cb1->setCheck(TRUE);
    m_name_tab.m_cb2 = new FXCheckButton(m_tabframe1, "Case sensitive search", NULL, 0, flags);
    m_name_tab.m_cb3 = new FXCheckButton(m_tabframe1, "Show hidden files", NULL, 0, flags);

    sel = GUIObject::ID_RADIOBUTTON;
    flags = RADIOBUTTON_NORMAL | LAYOUT_EXPLICIT | JUSTIFY_LEFT;
    m_date_tab.m_rb1 = new FXRadioButton(m_tabframe2, "All files", this, sel, flags);
    m_date_tab.m_rb1->setCheck(TRUE);
    m_date_tab.m_rb2 = new FXRadioButton(m_tabframe2, "Find all files", this, sel, flags);
    flags = FRAME_SUNKEN | FRAME_THICK | LAYOUT_EXPLICIT | COMBOBOX_STATIC;
    m_date_tab.m_combo1 = new FXComboBox(m_tabframe2, 0, NULL, 0, flags);
    flags = RADIOBUTTON_NORMAL | LAYOUT_EXPLICIT | JUSTIFY_LEFT;
    m_date_tab.m_rb3 = new FXRadioButton(m_tabframe2, "between", this, sel, flags);
    m_date_tab.m_rb3->setCheck(TRUE);
    m_date_tab.m_rb4 = new FXRadioButton(m_tabframe2, "during the previous", this, sel, flags);
    flags = FRAME_SUNKEN | FRAME_THICK | SPIN_NORMAL | LAYOUT_EXPLICIT;
    m_date_tab.m_spinner1 = new FXSpinner(m_tabframe2, 1, NULL, 0, flags);
    flags = LABEL_NORMAL | LAYOUT_EXPLICIT | JUSTIFY_LEFT;
    m_date_tab.m_label1 = new FXLabel(m_tabframe2, "months(s)", NULL, flags);
    flags = RADIOBUTTON_NORMAL | LAYOUT_EXPLICIT | JUSTIFY_LEFT;
    m_date_tab.m_rb5 = new FXRadioButton(m_tabframe2, "during the previous", this, sel, flags);
    flags = FRAME_SUNKEN | FRAME_THICK | SPIN_NORMAL | LAYOUT_EXPLICIT;
    m_date_tab.m_spinner2 = new FXSpinner(m_tabframe2, 1, NULL, 0, flags);
    flags = LABEL_NORMAL | LAYOUT_EXPLICIT | JUSTIFY_LEFT;
    m_date_tab.m_label2 = new FXLabel(m_tabframe2, "days(s)", NULL, flags);

    flags = CHECKBUTTON_NORMAL | LAYOUT_EXPLICIT | JUSTIFY_LEFT;
    m_adva_tab.m_cb1 = new FXCheckButton(m_tabframe3, "Search in files", NULL, 0, flags);

    flags = FRAME_SUNKEN | FRAME_THICK | LAYOUT_EXPLICIT;
    m_adva_tab.m_combo1 = new FXComboBox(m_tabframe3, 0, NULL, 0, flags);
    m_adva_tab.m_combo1->setNumVisible(10);

    flags = CHECKBUTTON_NORMAL | LAYOUT_EXPLICIT | JUSTIFY_LEFT;
    m_adva_tab.m_cb2 = new FXCheckButton(m_tabframe3, "Case sensitive search", NULL, 0, flags);

    sel = GUIObject::ID_BUTTON;
    flags = BUTTON_NORMAL | LAYOUT_EXPLICIT;
    m_but1 = new FXButton(m_mw, "F&ind", NULL, this, sel, flags);
    m_but2 = new FXButton(m_mw, "&Stop", NULL, this, sel, flags);
    m_but2->disable();
    sel = GUIObject::ID_EXIT;
    m_but3 = new FXButton(m_mw, "Exit", NULL, this, sel, flags);

    flags = LAYOUT_EXPLICIT;
    m_gb2 = new FXGroupBox(m_mw, "", flags);

    sel = GUIObject::ID_FOLDINGLIST;
    flags = FOLDINGLIST_NORMAL | LAYOUT_FILL_X | LAYOUT_FILL_Y;
    m_fl = new FXFoldingList(m_gb2, this, sel, flags);
    m_fl->appendHeader("Name", 0, 100);
    m_fl->appendHeader("In Subfolder", 0, 100);
    m_fl->appendHeader("Size", 0, 100);
    m_fl->appendHeader("Modified", 0, 100);
    m_flh = m_fl->getHeader();
    m_flh->setTarget(this);
    m_flh->setSelector(GUIObject::ID_FOLDINGLISTHEADER);

    m_fl_popup = new FXMenuPane(m_fl);
    sel = GUIObject::ID_COPY_FILENAME;
    new FXMenuCommand(m_fl_popup, "&Copy filename\t\tCopy the filename to clipboard.", NULL, this, sel);
    new FXMenuSeparator(m_fl_popup);
    sel = GUIObject::ID_COPY_FULL_PATH;
    new FXMenuCommand(m_fl_popup, "&Copy full path\t\tCopy the full path to clipboard.", NULL, this, sel);

    flags = LAYOUT_SIDE_TOP | LAYOUT_FILL_X;
    m_topdock = new FXDockSite(m_mw, flags);

    flags = FRAME_RAISED;
    m_tbs = new FXToolBarShell(m_mw, flags);

    flags = LAYOUT_DOCK_NEXT | LAYOUT_SIDE_TOP | LAYOUT_FILL_X | FRAME_RAISED;
    m_mb = new FXMenuBar(m_topdock, m_tbs, flags);

    m_filemenu = new FXMenuPane(m_mw);
    new FXMenuTitle(m_mb, "&File", NULL, m_filemenu);
    sel = GUIObject::ID_EXIT;
    new FXMenuCommand(m_filemenu, "&Exit\t\tExit the application.", NULL, this, sel);

    m_helpmenu = new FXMenuPane(m_mw);
    new FXMenuTitle(m_mb, "&Help", NULL, m_helpmenu);
    sel = GUIObject::ID_HELP;
    new FXMenuCommand(m_helpmenu, "&Help...\t\tDisplay help information.", NULL, this, sel);
    sel = GUIObject::ID_ABOUT;
    new FXMenuCommand(m_helpmenu, "&About\t\tDisplay version information.", NULL, this, sel);

    flags = LAYOUT_SIDE_BOTTOM | LAYOUT_FILL_X | STATUSBAR_WITH_DRAGCORNER | FRAME_RAISED;
    m_sb = new FXStatusBar(m_mw, flags);

    m_sbl1 = new FXLabel(m_sb, "", NULL, LAYOUT_RIGHT | LAYOUT_CENTER_Y);

    m_app->create();
    m_mw->show(PLACEMENT_SCREEN);

    finder_event_create(&m_gui_event);
    ih = (FXInputHandle)finder_event_get_wait_obj(m_gui_event);
    m_app->addInput(ih, INPUT_READ, this, GUIObject::ID_SOCKET);

    m_dnd_types[0] = m_app->registerDragType("UTF8_STRING");
    m_dnd_types[1] = m_app->registerDragType("text/uri-list");
}

/*****************************************************************************/
GUIObject::~GUIObject()
{
    LOGLN0((m_fi, LOG_INFO, LOGS, LOGP));
    finder_event_delete(m_gui_event);
    delete m_app;
    delete m_mutex1;
}

/*****************************************************************************/
int
GUIObject::findDone()
{
    int count;
    int width;
    int max_width[4];
    FXString str1;
    FXFont* ft;
    ItemObject* io;
    FXFoldingItem* folding_item;

    m_but1->enable();
    m_but2->disable();
    count = m_fl->getNumItems();
    str1.format("%d Items found", count);
    m_sbl1->setText(str1);

    /* resize the columns */
    ft = m_fl->getFont();
    max_width[0] = ft->getTextWidth("Name");
    max_width[1] = ft->getTextWidth("In Subfolder");
    max_width[2] = ft->getTextWidth("Size");
    max_width[3] = ft->getTextWidth("Modified");
    folding_item = m_fl->getFirstItem();
    while (folding_item != NULL)
    {
        io = (ItemObject*)(folding_item->getData());
        width = ft->getTextWidth(io->filename);
        max_width[0] = FXMAX(width, max_width[0]);
        width = ft->getTextWidth(io->in_subfolder);
        max_width[1] = FXMAX(width, max_width[1]);
        width = ft->getTextWidth(io->size_text);
        max_width[2] = FXMAX(width, max_width[2]);
        width = ft->getTextWidth(io->modified);
        max_width[3] = FXMAX(width, max_width[3]);
        folding_item = folding_item->getNext();
    }
    m_fl->setHeaderSize(0, max_width[0] + 8);
    m_fl->setHeaderSize(1, max_width[1] + 8);
    m_fl->setHeaderSize(2, max_width[2] + 8);
    m_fl->setHeaderSize(3, max_width[3] + 8);
    return 0;
}

/*****************************************************************************/
int
GUIObject::setEvent()
{
    m_mutex1->lock();
    finder_event_set(m_gui_event);
    m_mutex1->unlock();
    return 0;
}

/*****************************************************************************/
int
GUIObject::mainLoop()
{
    gui_init(m_fi);
    loadFromReg();
    m_app->run();
    gui_deinit(m_fi);
    return 0;
}

/*****************************************************************************/
int
GUIObject::loadCombo(FXComboBox* cb, const char* section,
                     const char* key_prefix)
{
    FXint index;
    FXRegistry* reg;
    FXString key;
    FXString val;

    reg = &(m_app->reg());
    for (index = 0; index < 100; index++)
    {
        key.format("%s%2.2d", key_prefix, index);
        val = reg->readStringEntry(section, key.text(), "_NoWay_");
        if (val != "_NoWay_")
        {
            cb->appendItem(val);
        }
    }
    return 0;
}

/*****************************************************************************/
int
GUIObject::loadCheckbox(FXCheckButton* cb, const char* section,
                        const char* key, FXbool default_checked)
{
    FXRegistry* reg;
    FXbool valbool;

    reg = &(m_app->reg());
    valbool = reg->readBoolEntry(section, key, default_checked);
    cb->setCheck(valbool);
    return 0;
}

/*****************************************************************************/
int
GUIObject::loadFromReg()
{
    /* Name/Location tab */
    loadCombo(m_name_tab.m_combo1, "NameLocation", "Named");
    loadCombo(m_name_tab.m_combo2, "NameLocation", "LookIn");
    loadCheckbox(m_name_tab.m_cb1, "NameLocation", "IncludeSubfolders", TRUE);
    loadCheckbox(m_name_tab.m_cb2, "NameLocation", "CaseSensitiveSearch", FALSE);
    loadCheckbox(m_name_tab.m_cb3, "NameLocation", "ShowHiddenFiles", FALSE);
    /* Advanced tab */
    loadCheckbox(m_adva_tab.m_cb1, "Advanced", "SearchInFiles", FALSE);
    loadCombo(m_adva_tab.m_combo1, "Advanced", "SearchInFilesText");
    loadCheckbox(m_adva_tab.m_cb2, "Advanced", "CaseSensitiveSearch", FALSE);
    return 0;
}

/*****************************************************************************/
int
GUIObject::saveCombo(FXComboBox* cb, const char* section,
                     const char* key_prefix)
{
    FXString str1;
    FXString str2;
    FXint index;
    FXint count;
    FXRegistry* reg;

    reg = &(m_app->reg());
    str1 = cb->getText();
    index = cb->findItem(str1);
    if (index < 0)
    {
        cb->insertItem(0, str1);
    }
    else if (index > 0)
    {
        cb->removeItem(index);
        cb->insertItem(0, str1);
    }
    cb->setCurrentItem(0, TRUE);
    for (index = 0; index < 100; index++)
    {
        str2.format("%s%2.2d", key_prefix, index);
        reg->deleteEntry(section, str2.text());
    }
    count = cb->getNumItems();
    if (count > 100)
    {
        count = 100;
    }
    for (index = 0; index < count; index++)
    {
        str1 = cb->getItem(index);
        str2.format("%s%2.2d", key_prefix, index);
        reg->writeStringEntry(section, str2.text(), str1.text());
    }
    return 0;
}

/*****************************************************************************/
int
GUIObject::saveCheckbox(FXCheckButton* cb, const char* section,
                        const char* key, FXbool default_checked)
{
    FXbool bool1;
    FXRegistry* reg;

    reg = &(m_app->reg());
    bool1 = cb->getCheck();
    if ((!bool1) == (!default_checked))
    {
        reg->deleteEntry(section, key);
    }
    else
    {
        reg->writeBoolEntry(section, key, bool1);
    }
    return 0;
}

/*****************************************************************************/
int
GUIObject::saveToReg()
{
    /* Name/Location tab */
    saveCombo(m_name_tab.m_combo1, "NameLocation", "Named");
    saveCombo(m_name_tab.m_combo2, "NameLocation", "LookIn");
    saveCheckbox(m_name_tab.m_cb1, "NameLocation", "IncludeSubfolders", TRUE);
    saveCheckbox(m_name_tab.m_cb2, "NameLocation", "CaseSensitiveSearch", FALSE);
    saveCheckbox(m_name_tab.m_cb3, "NameLocation", "ShowHiddenFiles", FALSE);
    /* Advanced tab */
    saveCheckbox(m_adva_tab.m_cb1, "Advanced", "SearchInFiles", FALSE);
    saveCombo(m_adva_tab.m_combo1, "Advanced", "SearchInFilesText");
    saveCheckbox(m_adva_tab.m_cb2, "Advanced", "CaseSensitiveSearch", FALSE);
    return 0;
}

/*****************************************************************************/
int
GUIObject::addOne(const char* filename, const char* in_subfolder,
                  FINDER_I64 size, const char* modified)
{
    FXFoldingItem* folding_item;
    FXString str1;
    ItemObject* io;
    char text[128];

    io = new ItemObject();
    str1 = filename;
    io->filename = filename;
    str1 += "\t";
    str1 += in_subfolder;
    io->in_subfolder = in_subfolder;
    str1 += "\t";
    format_commas(size, text, 128);
    io->size = size;
    io->size_text = text;
    str1 += text;
    str1 += "\t";
    str1 += modified;
    io->modified = modified;
    folding_item = new FXFoldingItem(str1);
    folding_item->setDraggable(TRUE);
    m_fl->appendItem(NULL, folding_item, TRUE);
    folding_item->setData(io);
    return 0;
}

/*****************************************************************************/
long
GUIObject::onDefault(FXObject* obj, FXSelector sel, void* ptr)
{
    LOGLN10((m_fi, LOG_DEBUG, LOGS "obj %p sel %d ptr %p", LOGP, obj, sel, ptr));
    return FXObject::onDefault(obj, sel, ptr);
}

/*****************************************************************************/
long
GUIObject::onPress(FXObject* obj, FXSelector sel, void* ptr)
{
    FXString str1;

    LOGLN0((m_fi, LOG_INFO, LOGS "obj %p sel %d ptr %p", LOGP, obj, sel, ptr));
    if (obj == m_but1)
    {
        LOGLN0((m_fi, LOG_INFO, LOGS "but1", LOGP));
        m_fl->clearItems(TRUE);
        str1 = m_name_tab.m_combo1->getText();
        snprintf(m_fi->named, sizeof(m_fi->named), "%s", str1.text());
        str1 = m_name_tab.m_combo2->getText();
        snprintf(m_fi->look_in, sizeof(m_fi->look_in), "%s", str1.text());
        m_fi->include_subfolders = m_name_tab.m_cb1->getCheck();
        m_fi->case_sensitive = m_name_tab.m_cb2->getCheck();
        m_fi->show_hidden = m_name_tab.m_cb3->getCheck();
        m_fi->search_in_files = m_adva_tab.m_cb1->getCheck();
        m_fi->search_in_case_sensitive = m_adva_tab.m_cb2->getCheck();
        str1 = m_adva_tab.m_combo1->getText();
        snprintf(m_fi->text, sizeof(m_fi->text), "%s", str1.text());

        saveToReg();

        //ap->fl->hide();
        start_find(m_fi);
        m_but1->disable();
        m_but2->enable();
    }
    if (obj == m_but2)
    {
        LOGLN0((m_fi, LOG_INFO, LOGS "but2", LOGP));
        stop_find(m_fi);
    }
    if (obj == m_but3)
    {
        LOGLN0((m_fi, LOG_INFO, LOGS "but3", LOGP));
        //ap->app->stop(0);
    }
    if (obj == m_name_tab.m_but1)
    {
        LOGLN0((m_fi, LOG_INFO, LOGS "but4", LOGP));
        str1 = m_name_tab.m_combo2->getText();
        str1 = FXDirDialog::getOpenDirectory(m_mw, "Select Look In directory", str1);
        if (str1 != "")
        {
            m_name_tab.m_combo2->setText(str1);
        }
    }

    return 0;
}

/*****************************************************************************/
long
GUIObject::onTabChange(FXObject* obj, FXSelector sel, void* ptr)
{
    (void)obj;
    (void)sel;
    (void)ptr;
    return 1;
}

/*****************************************************************************/
long
GUIObject::onCmdConfigure(FXObject* obj, FXSelector sel, void* ptr)
{
    FXint width;
    FXint height;
    FXint lw;

    (void)obj;
    (void)sel;
    (void)ptr;
    width = m_mw->getWidth();
    height = m_mw->getHeight();
    if ((width != m_width) || (height != m_height))
    {
        LOGLN10((m_fi, LOG_DEBUG, LOGS "resized to %dx%d, was %dx%d",
                LOGP, width, height, m_width, m_height));
        m_width = width;
        m_height = height;

        m_gb1->move(0, 22);
        m_gb1->resize(width - 120, 180);

        m_gb2->move(0, 200);
        m_gb2->resize(width, height - 225);

        m_but1->move(width - 110, 32);
        m_but1->resize(100, 30);

        m_but2->move(width - 110, 72);
        m_but2->resize(100, 30);

        m_but3->move(width - 110, 165);
        m_but3->resize(100, 30);

        m_name_tab.m_label1->move(8, 8);
        m_name_tab.m_label1->resize(100, 24);

        lw = m_tabframe1->getWidth();
        m_name_tab.m_combo1->move(85, 8);
        m_name_tab.m_combo1->resize(lw - 100, 24);

        m_name_tab.m_label2->move(8, 41);
        m_name_tab.m_label2->resize(100, 24);

        m_name_tab.m_combo2->move(85, 41);
        m_name_tab.m_combo2->resize(lw - 165, 24);

        m_name_tab.m_but1->move(lw - 75, 41);
        m_name_tab.m_but1->resize(60, 24);

        m_name_tab.m_cb1->move(8, 74);
        m_name_tab.m_cb1->resize(200, 24);

        m_name_tab.m_cb2->move(8, 107);
        m_name_tab.m_cb2->resize(200, 24);

        m_name_tab.m_cb3->move(216, 74);
        m_name_tab.m_cb3->resize(200, 24);

        m_adva_tab.m_cb1->move(10, 10);
        m_adva_tab.m_cb1->resize(120, 24);

        lw = m_tabframe3->getWidth();
        m_adva_tab.m_combo1->move(10, 40);
        m_adva_tab.m_combo1->resize(lw - 20, 24);

        m_adva_tab.m_cb2->move(10, 74);
        m_adva_tab.m_cb2->resize(160, 24);

        m_date_tab.m_rb1->move(10, 8);
        m_date_tab.m_rb1->resize(400, 24);

        m_date_tab.m_rb2->move(10, 8 + 24);
        m_date_tab.m_rb2->resize(400, 24);

        m_date_tab.m_rb3->move(30, 8 + 48);
        m_date_tab.m_rb3->resize(400, 24);

        m_date_tab.m_rb4->move(30, 8 + 72);
        m_date_tab.m_rb4->resize(400, 24);

        m_date_tab.m_rb5->move(30, 8 + 96);
        m_date_tab.m_rb5->resize(400, 24);

        m_date_tab.m_spinner1->move(200, 8 + 72);
        m_date_tab.m_spinner1->resize(48, 24);

        m_date_tab.m_spinner2->move(200, 8 + 96);
        m_date_tab.m_spinner2->resize(48, 24);

        m_date_tab.m_label1->move(250, 8 + 72);
        m_date_tab.m_label1->resize(100, 24);

        m_date_tab.m_label2->move(250, 8 + 96);
        m_date_tab.m_label2->resize(100, 24);

        m_date_tab.m_combo1->move(110, 8 + 24);
        m_date_tab.m_combo1->resize(100, 24);

    }
    return 0;
}

/*****************************************************************************/
long
GUIObject::onCmdExit(FXObject* obj, FXSelector sel, void* ptr)
{
    (void)obj;
    (void)sel;
    (void)ptr;
    LOGLN0((m_fi, LOG_INFO, LOGS, LOGP));
    m_mw->close(TRUE);
    return 1;
}

/*****************************************************************************/
long
GUIObject::onCmdHelp(FXObject* obj, FXSelector sel, void* ptr)
{
    (void)obj;
    (void)sel;
    (void)ptr;
    LOGLN0((m_fi, LOG_INFO, LOGS, LOGP));
    return 0;
}

/*****************************************************************************/
long
GUIObject::onCmdAbout(FXObject* obj, FXSelector sel, void* ptr)
{
    FXbool ok;
    AboutDialog* about;

    (void)obj;
    (void)sel;
    (void)ptr;
    LOGLN0((m_fi, LOG_INFO, LOGS, LOGP));
    about = new AboutDialog(m_app, m_mw, m_fi);
    ok = about->execute(PLACEMENT_OWNER);
    delete about;
    LOGLN0((m_fi, LOG_INFO, LOGS "ok %d", LOGP, ok));
    return 0;
}

/*****************************************************************************/
long
GUIObject::onEvent1(FXObject* obj, FXSelector sel, void* ptr)
{
    (void)obj;
    (void)sel;
    (void)ptr;
    finder_event_clear(m_gui_event);
    m_app->addTimeout(this, GUIObject::ID_MAINWINDOW1, 0, NULL);
    return 1;
}

/*****************************************************************************/
long
GUIObject::onEventTimeout(FXObject* obj, FXSelector sel, void* ptr)
{
    (void)obj;
    (void)sel;
    (void)ptr;
    event_callback(m_fi);
    return 1;
}

/*****************************************************************************/
/* sort column 1 acceding */
static FXint
sort00(const FXFoldingItem* a, const FXFoldingItem* b)
{
    ItemObject* ao;
    ItemObject* bo;

    ao = (ItemObject*)(a->getData());
    if (ao != NULL)
    {
        bo = (ItemObject*)(b->getData());
        if (bo != NULL)
        {
            if (ao->filename < bo->filename)
            {
                return -1;
            }
            if (ao->filename > bo->filename)
            {
                return 1;
            }
        }
    }
    return 0;
}

/*****************************************************************************/
/* sort column 1 descending */
static FXint
sort01(const FXFoldingItem* a, const FXFoldingItem* b)
{
    ItemObject* ao;
    ItemObject* bo;

    ao = (ItemObject*)(a->getData());
    if (ao != NULL)
    {
        bo = (ItemObject*)(b->getData());
        if (bo != NULL)
        {
            if (ao->filename < bo->filename)
            {
                return 1;
            }
            if (ao->filename > bo->filename)
            {
                return -1;
            }
        }
    }
    return 0;
}

/*****************************************************************************/
/* sort column 2 acceding */
static FXint
sort10(const FXFoldingItem* a, const FXFoldingItem* b)
{
    ItemObject* ao;
    ItemObject* bo;

    ao = (ItemObject*)(a->getData());
    if (ao != NULL)
    {
        bo = (ItemObject*)(b->getData());
        if (bo != NULL)
        {
            if (ao->in_subfolder < bo->in_subfolder)
            {
                return -1;
            }
            if (ao->in_subfolder > bo->in_subfolder)
            {
                return 1;
            }
        }
    }
    return 0;
}

/*****************************************************************************/
/* sort column 2 descending */
static FXint
sort11(const FXFoldingItem* a, const FXFoldingItem* b)
{
    ItemObject* ao;
    ItemObject* bo;

    ao = (ItemObject*)(a->getData());
    if (ao != NULL)
    {
        bo = (ItemObject*)(b->getData());
        if (bo != NULL)
        {
            if (ao->in_subfolder < bo->in_subfolder)
            {
                return 1;
            }
            if (ao->in_subfolder > bo->in_subfolder)
            {
                return -1;
            }
        }
    }
    return 0;
}

/*****************************************************************************/
/* sort column 3 acceding */
static FXint
sort20(const FXFoldingItem* a, const FXFoldingItem* b)
{
    ItemObject* ao;
    ItemObject* bo;

    ao = (ItemObject*)(a->getData());
    if (ao != NULL)
    {
        bo = (ItemObject*)(b->getData());
        if (bo != NULL)
        {
            if (ao->size < bo->size)
            {
                return -1;
            }
            if (ao->size > bo->size)
            {
                return 1;
            }
        }
    }
    return 0;
}

/*****************************************************************************/
/* sort column 3 descending */
static FXint
sort21(const FXFoldingItem* a, const FXFoldingItem* b)
{
    ItemObject* ao;
    ItemObject* bo;

    ao = (ItemObject*)(a->getData());
    if (ao != NULL)
    {
        bo = (ItemObject*)(b->getData());
        if (bo != NULL)
        {
            if (ao->size < bo->size)
            {
                return 1;
            }
            if (ao->size > bo->size)
            {
                return -1;
            }
        }
    }
    return 0;
}

/*****************************************************************************/
/* sort column 4 acceding */
static FXint
sort30(const FXFoldingItem* a, const FXFoldingItem* b)
{
    ItemObject* ao;
    ItemObject* bo;

    ao = (ItemObject*)(a->getData());
    if (ao != NULL)
    {
        bo = (ItemObject*)(b->getData());
        if (bo != NULL)
        {
            if (ao->modified < bo->modified)
            {
                return -1;
            }
            if (ao->modified > bo->modified)
            {
                return 1;
            }
        }
    }
    return 0;
}

/*****************************************************************************/
/* sort column 4 descending */
static FXint
sort31(const FXFoldingItem* a, const FXFoldingItem* b)
{
    ItemObject* ao;
    ItemObject* bo;

    ao = (ItemObject*)(a->getData());
    if (ao != NULL)
    {
        bo = (ItemObject*)(b->getData());
        if (bo != NULL)
        {
            if (ao->modified < bo->modified)
            {
                return 1;
            }
            if (ao->modified > bo->modified)
            {
                return -1;
            }
        }
    }
    return 0;
}

/*****************************************************************************/
long
GUIObject::onFoldingListHeader(FXObject* obj, FXSelector sel, void* ptr)
{
    int index;
    FXFoldingListSortFunc sf1[4] = { sort00, sort10, sort20, sort30 };
    FXFoldingListSortFunc sf2[4] = { sort01, sort11, sort21, sort31 };

    (void)obj;
    (void)sel;
    index = (int)(FXival)ptr;
    index &= 3;
    if ((m_sort_order & (1 << index)) == 0)
    {
        m_fl->setSortFunc(sf1[index]);
        m_sort_order |= 1 << index;
    }
    else
    {
        m_fl->setSortFunc(sf2[index]);
        m_sort_order &= ~(1 << index);
    }
    m_fl->sortItems();
    return 1;
}


/*****************************************************************************/
int
best_fit_column(GUIObject* go, int column)
{
    int width;
    int max_width;
    FXFont* ft;
    ItemObject* io;
    FXFoldingItem* folding_item;

    ft = go->m_fl->getFont();
    switch (column)
    {
        case 0:
            max_width = ft->getTextWidth("Name");
            folding_item = go->m_fl->getFirstItem();
            while (folding_item != NULL)
            {
                io = (ItemObject*)(folding_item->getData());
                width = ft->getTextWidth(io->filename);
                max_width = FXMAX(width, max_width);
                folding_item = folding_item->getNext();
            }
            go->m_fl->setHeaderSize(0, max_width + 8);
            break;
        case 1:
            max_width = ft->getTextWidth("In Subfolder");
            folding_item = go->m_fl->getFirstItem();
            while (folding_item != NULL)
            {
                io = (ItemObject*)(folding_item->getData());
                width = ft->getTextWidth(io->in_subfolder);
                max_width = FXMAX(width, max_width);
                folding_item = folding_item->getNext();
            }
            go->m_fl->setHeaderSize(1, max_width + 8);
            break;
        case 2:
            max_width = ft->getTextWidth("Size");
            folding_item = go->m_fl->getFirstItem();
            while (folding_item != NULL)
            {
                io = (ItemObject*)(folding_item->getData());
                width = ft->getTextWidth(io->size_text);
                max_width = FXMAX(width, max_width);
                folding_item = folding_item->getNext();
            }
            go->m_fl->setHeaderSize(2, max_width + 8);
            break;
        case 3:
            max_width = ft->getTextWidth("Modified");
            folding_item = go->m_fl->getFirstItem();
            while (folding_item != NULL)
            {
                io = (ItemObject*)(folding_item->getData());
                width = ft->getTextWidth(io->modified);
                max_width = FXMAX(width, max_width);
                folding_item = folding_item->getNext();
            }
            go->m_fl->setHeaderSize(3, max_width + 8);
            break;
    }
    return 0;
}

/*****************************************************************************/
long
GUIObject::onFoldingListHeaderClick(FXObject* obj, FXSelector sel, void* ptr)
{
    int time;
    int diff;
    int index;

    (void)obj;
    (void)sel;
    time = get_mstime();
    diff = time - m_last_header_click_mstime;
    if ((diff > 0) && (diff < 500))
    {
        index = (int)(FXival)ptr;
        best_fit_column(this, index);
    }
    m_last_header_click_mstime = time;
    return 1;
}

/*****************************************************************************/
long
GUIObject::onFoldingListItemDelete(FXObject* obj, FXSelector sel, void* ptr)
{
    ItemObject* io;
    FXFoldingItem* item;

    (void)obj;
    (void)sel;
    item = (FXFoldingItem*)ptr;
    if (item != NULL)
    {
        io = (ItemObject*)(item->getData());
        if (io != NULL)
        {
            delete io;
        }
    }
    return 1;
}

/*****************************************************************************/
long
GUIObject::onFLRightMouseUp(FXObject* obj, FXSelector sel, void* ptr)
{
    FXEvent* event;
    FXFoldingItem* rci;

    (void)obj;
    (void)sel;
    LOGLN0((m_fi, LOG_INFO, LOGS, LOGP));
    event = (FXEvent*)ptr;
    if (event->moved == FALSE)
    {
        rci = m_fl->getItemAt(event->click_x, event->click_y);
        if (rci != NULL)
        {
            if (m_fl->isItemSelected(rci) == FALSE)
            {
                m_fl->killSelection(TRUE);
                m_fl->setCurrentItem(rci, TRUE);
                m_fl->selectItem(rci, TRUE);
            }
            m_fl_popup->popup(NULL, event->root_x, event->root_y);
            m_app->runModalWhileShown(m_fl_popup);
        }
    }
    return 1;
}

/*****************************************************************************/
long
GUIObject::onCopyFilename(FXObject* obj, FXSelector sel, void* ptr)
{
    FXFoldingItem* fi;
    ItemObject* io;
    FXString str1;

    (void)obj;
    (void)sel;
    (void)ptr;
    LOGLN0((m_fi, LOG_INFO, LOGS, LOGP));
    m_dnd_str = "";
    fi = m_fl->getFirstItem();
    while (fi != NULL)
    {
        if (fi->isSelected())
        {
            io = (ItemObject*)(fi->getData());
            str1 = io->filename;
            if (m_dnd_str.length() > 0)
            {
                m_dnd_str += "\n";
            }
            m_dnd_str += str1;
        }
        fi = fi->getNext();
    }
    m_mw->acquireClipboard(m_dnd_types + 0, 1);
    return 1;
}

/*****************************************************************************/
long
GUIObject::onCopyFullPath(FXObject* obj, FXSelector sel, void* ptr)
{
    FXFoldingItem* fi;
    ItemObject* io;
    FXString str1;

    (void)obj;
    (void)sel;
    (void)ptr;
    LOGLN0((m_fi, LOG_INFO, LOGS, LOGP));
    m_dnd_str = "";
    fi = m_fl->getFirstItem();
    while (fi != NULL)
    {
        if (fi->isSelected())
        {
            io = (ItemObject*)(fi->getData());
            if (io->in_subfolder.length() < 1)
            {
                str1 = m_fi->look_in;
                str1 += "/";
                str1 += io->filename;
            }
            else
            {
                str1 = m_fi->look_in;
                str1 += "/";
                str1 += io->in_subfolder;
                str1 += "/";
                str1 += io->filename;
            }
            if (m_dnd_str.length() > 0)
            {
                m_dnd_str += "\n";
            }
            m_dnd_str += str1;
        }
        fi = fi->getNext();
    }
    m_mw->acquireClipboard(m_dnd_types + 0, 1);
    return 1;
}

/*****************************************************************************/
/* return 0 can close, 1 can not close */
long
GUIObject::onClose(FXObject* obj, FXSelector sel, void* ptr)
{
    FXuint rv;

    (void)obj;
    (void)sel;
    (void)ptr;
    LOGLN0((m_fi, LOG_INFO, LOGS, LOGP));
    rv = FXMessageBox::warning(m_mw, MBOX_YES_NO, "Question", "Do you want to exit?");
    if (rv == MBOX_CLICKED_YES)
    {
        m_fl->clearItems(TRUE);
        return 0;
    }
    return 1;
}

/*****************************************************************************/
long
GUIObject::onClipboardLost(FXObject* obj, FXSelector sel, void* ptr)
{
    (void)obj;
    (void)sel;
    (void)ptr;
    LOGLN10((m_fi, LOG_INFO, LOGS, LOGP));
    return 1;
}

/*****************************************************************************/
long
GUIObject::onClipboardGained(FXObject* obj, FXSelector sel, void* ptr)
{
    (void)obj;
    (void)sel;
    (void)ptr;
    LOGLN10((m_fi, LOG_INFO, LOGS, LOGP));
    return 1;
}

/*****************************************************************************/
long
GUIObject::onClipboardRequest(FXObject* obj, FXSelector sel, void* ptr)
{
    FXuchar* data;
    FXuint len;

    (void)obj;
    (void)sel;
    (void)ptr;
    LOGLN10((m_fi, LOG_INFO, LOGS, LOGP));
    len = m_dnd_str.length();
    FXMALLOC(&data, FXuchar, len + 8);
    memcpy(data, m_dnd_str.text(), len);
    data[len] = 0;
    m_mw->setDNDData(FROM_CLIPBOARD, m_dnd_types[0], data, len);
    return 1;
}

/*****************************************************************************/
long
GUIObject::onBeginDrag(FXObject* obj, FXSelector sel, void* ptr)
{
    FXFoldingItem* fi;
    ItemObject* io;
    FXString str1;

    (void)obj;
    (void)sel;
    (void)ptr;
    LOGLN10((m_fi, LOG_INFO, LOGS, LOGP));
    m_dnd_str = "";
    fi = m_fl->getFirstItem();
    while (fi != NULL)
    {
        if (fi->isSelected())
        {
            io = (ItemObject*)(fi->getData());
            if (io->in_subfolder.length() < 1)
            {
                str1 = "file://";
                str1 += m_fi->look_in;
                str1 += "/";
                str1 += io->filename;
            }
            else
            {
                str1 = "file://";
                str1 += m_fi->look_in;
                str1 += "/";
                str1 += io->in_subfolder;
                str1 += "/";
                str1 += io->filename;
            }
            m_dnd_str += str1;
            m_dnd_str += "\r\n";
        }
        fi = fi->getNext();
    }
    m_mw->beginDrag(m_dnd_types + 1, 1);
    return 1;
}

/*****************************************************************************/
long
GUIObject::onEndDrag(FXObject* obj, FXSelector sel, void* ptr)
{
    (void)obj;
    (void)sel;
    (void)ptr;
    LOGLN10((m_fi, LOG_INFO, LOGS, LOGP));
    m_mw->endDrag();
    m_fl->setDragCursor(m_app->getDefaultCursor(DEF_ARROW_CURSOR));
    return 1;
}

/*****************************************************************************/
long
GUIObject::onDragged(FXObject* obj, FXSelector sel, void* ptr)
{
    FXEvent* event;
    FXCursor* cur;

    (void)obj;
    (void)sel;
    LOGLN10((m_fi, LOG_INFO, LOGS, LOGP));
    event = (FXEvent*)ptr;
    m_mw->handleDrag(event->root_x, event->root_y);
    if (m_mw->didAccept() != DRAG_REJECT)
    {
        LOGLN10((m_fi, LOG_DEBUG, LOGS "drop_ok_cursor", LOGP));
        cur = m_app->getDefaultCursor(DEF_DNDCOPY_CURSOR);
        m_fl->setDragCursor(cur);
    }
    else
    {
        LOGLN10((m_fi, LOG_DEBUG, LOGS "drop_not_ok_cursor", LOGP));
        cur = m_app->getDefaultCursor(DEF_DNDSTOP_CURSOR);
        m_fl->setDragCursor(cur);
    }

    return 1;
}

/*****************************************************************************/
long
GUIObject::onDNDRequest(FXObject* obj, FXSelector sel, void* ptr)
{
    FXuchar* data;
    FXuint len;

    (void)obj;
    (void)sel;
    (void)ptr;
    LOGLN10((m_fi, LOG_INFO, LOGS, LOGP));
    len = m_dnd_str.length();
    FXMALLOC(&data, FXuchar, len + 8);
    memcpy(data, m_dnd_str.text(), len);
    data[len] = 0;
    m_mw->setDNDData(FROM_DRAGNDROP, m_dnd_types[1], data, len);
    return 1;
}

/*****************************************************************************/
long
GUIObject::onRadioButton(FXObject* obj, FXSelector sel, void* ptr)
{
    (void)sel;
    (void)ptr;
    LOGLN0((m_fi, LOG_INFO, LOGS, LOGP));
    if (obj == m_date_tab.m_rb1)
    {
        if (m_date_tab.m_rb1->getCheck())
        {
            m_date_tab.m_rb2->setCheck(FALSE);
        }
    }
    else if (obj == m_date_tab.m_rb2)
    {
        if (m_date_tab.m_rb2->getCheck())
        {
            m_date_tab.m_rb1->setCheck(FALSE);
        }
    }
    else if (obj == m_date_tab.m_rb3)
    {
        if (m_date_tab.m_rb3->getCheck())
        {
            m_date_tab.m_rb4->setCheck(FALSE);
            m_date_tab.m_rb5->setCheck(FALSE);
        }
    }
    else if (obj == m_date_tab.m_rb4)
    {
        if (m_date_tab.m_rb4->getCheck())
        {
            m_date_tab.m_rb3->setCheck(FALSE);
            m_date_tab.m_rb5->setCheck(FALSE);
        }
    }
    else if (obj == m_date_tab.m_rb5)
    {
        if (m_date_tab.m_rb5->getCheck())
        {
            m_date_tab.m_rb3->setCheck(FALSE);
            m_date_tab.m_rb4->setCheck(FALSE);
        }
    }
    return 1;
}

FXDEFMAP(GUIObject) GUIObjectMap[] =
{
    FXMAPFUNC(SEL_COMMAND, GUIObject::ID_BUTTON, GUIObject::onPress),
    FXMAPFUNC(SEL_COMMAND, GUIObject::ID_TABBOOK, GUIObject::onTabChange),
    FXMAPFUNC(SEL_FINDERCONF, GUIObject::ID_MAINWINDOW, GUIObject::onCmdConfigure),
    FXMAPFUNC(SEL_TIMEOUT, GUIObject::ID_MAINWINDOW1, GUIObject::onEventTimeout),
    FXMAPFUNC(SEL_COMMAND, GUIObject::ID_EXIT, GUIObject::onCmdExit),
    FXMAPFUNC(SEL_COMMAND, GUIObject::ID_HELP, GUIObject::onCmdHelp),
    FXMAPFUNC(SEL_COMMAND, GUIObject::ID_ABOUT, GUIObject::onCmdAbout),
    FXMAPFUNC(SEL_IO_READ, GUIObject::ID_SOCKET, GUIObject::onEvent1),
    FXMAPFUNC(SEL_COMMAND, GUIObject::ID_FOLDINGLISTHEADER, GUIObject::onFoldingListHeader),
    FXMAPFUNC(SEL_CLICKED, GUIObject::ID_FOLDINGLISTHEADER, GUIObject::onFoldingListHeaderClick),
    FXMAPFUNC(SEL_DELETED, GUIObject::ID_FOLDINGLIST, GUIObject::onFoldingListItemDelete),
    FXMAPFUNC(SEL_RIGHTBUTTONRELEASE, GUIObject::ID_FOLDINGLIST, GUIObject::onFLRightMouseUp),
    FXMAPFUNC(SEL_COMMAND, GUIObject::ID_COPY_FILENAME, GUIObject::onCopyFilename),
    FXMAPFUNC(SEL_COMMAND, GUIObject::ID_COPY_FULL_PATH, GUIObject::onCopyFullPath),
    FXMAPFUNC(SEL_CLOSE, GUIObject::ID_MAINWINDOW, GUIObject::onClose),
    FXMAPFUNC(SEL_CLIPBOARD_LOST, GUIObject::ID_MAINWINDOW, GUIObject::onClipboardLost),
    FXMAPFUNC(SEL_CLIPBOARD_GAINED, GUIObject::ID_MAINWINDOW, GUIObject::onClipboardGained),
    FXMAPFUNC(SEL_CLIPBOARD_REQUEST, GUIObject::ID_MAINWINDOW, GUIObject::onClipboardRequest),
    FXMAPFUNC(SEL_BEGINDRAG, GUIObject::ID_FOLDINGLIST, GUIObject::onBeginDrag),
    FXMAPFUNC(SEL_ENDDRAG, GUIObject::ID_FOLDINGLIST, GUIObject::onEndDrag),
    FXMAPFUNC(SEL_DRAGGED, GUIObject::ID_FOLDINGLIST, GUIObject::onDragged),
    FXMAPFUNC(SEL_DND_REQUEST, GUIObject::ID_MAINWINDOW, GUIObject::onDNDRequest),
    FXMAPFUNC(SEL_COMMAND, GUIObject::ID_RADIOBUTTON, GUIObject::onRadioButton)
};

FXIMPLEMENT(GUIObject, FXObject, GUIObjectMap, ARRAYNUMBER(GUIObjectMap))

/*****************************************************************************/
static int
gui_create(int argc, char** argv, struct finder_info** fi)
{
    GUIObject* go;

    *fi = (struct finder_info*)calloc(1, sizeof(struct finder_info));
    go = new GUIObject(argc, argv, *fi);
    (*fi)->gui_obj = go;
    return 0;
}

/*****************************************************************************/
static int
gui_main_loop(struct finder_info* fi)
{
    GUIObject* go;

    LOGLN0((fi, LOG_INFO, LOGS, LOGP));
    go = (GUIObject*)(fi->gui_obj);
    go->mainLoop();
    return 0;
}

/*****************************************************************************/
static int
gui_delete(struct finder_info* fi)
{
    GUIObject* go;

    LOGLN0((fi, LOG_INFO, LOGS, LOGP));
    go = (GUIObject*)(fi->gui_obj);
    go->m_app->exit(); /* close display, write registry */
    delete go;
    free(fi);
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
/* any thread */
int
gui_set_event(struct finder_info* fi)
{
    GUIObject* go;

    LOGLN10((fi, LOG_INFO, LOGS, LOGP));
    go = (GUIObject*)(fi->gui_obj);
    return go->setEvent();
}

/*****************************************************************************/
int
gui_find_done(struct finder_info* fi)
{
    GUIObject* go;

    LOGLN0((fi, LOG_INFO, LOGS, LOGP));
    go = (GUIObject*)(fi->gui_obj);
    return go->findDone();
}

/*****************************************************************************/
int
gui_add_one(struct finder_info* fi, const char* filename,
            const char* in_subfolder, FINDER_I64 size,
            const char* modified)
{
    GUIObject* go;

    go = (GUIObject*)(fi->gui_obj);
    return go->addOne(filename, in_subfolder, size, modified);
}

/*****************************************************************************/
int
gui_writeln(struct finder_info* fi, const char* msg)
{
    (void)fi;
    printf("%s\n", msg);
    return 0;
}

