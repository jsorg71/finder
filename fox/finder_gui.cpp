
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fx.h>

#include "finder.h"
#include "finder_event.h"
#include "finder_gui.h"

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
    virtual ~GUIObject();
    long onDefault(FXObject* obj, FXSelector sel, void* ptr);
    long onPress(FXObject* obj, FXSelector sel, void* ptr);
    long onTabChange(FXObject* obj, FXSelector sel, void* ptr);
    long onConfigure(FXObject* obj, FXSelector sel, void* ptr);
    long onResizeTimeout(FXObject* obj, FXSelector sel, void* ptr);
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
    enum _ids
    {
        ID_BUTTON = 0,
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
        ID_LAST
    };
public:
    struct finder_info* m_fi;
    FXApp* m_app;
    FXMutex* m_mutex1;
    FXMainWindow* m_mw;
    FXButton* m_but1;
    FXButton* m_but2;
    FXButton* m_but3;
    FXButton* m_but4;
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
    FXLabel* m_label1;
    FXLabel* m_label2;
    FXCheckButton* m_cb1;
    FXCheckButton* m_cb2;
    FXCheckButton* m_cb3;
    FXCheckButton* m_cb4;
    FXCheckButton* m_cb5;
    FXComboBox* m_combo1;
    FXComboBox* m_combo2;
    FXComboBox* m_combo3;
    FXTextField* m_text1;
    FXDockSite* m_topdock;
    FXToolBarShell* m_tbs;
    FXMenuBar* m_mb;
    FXMenuPane* m_filemenu;
    FXMenuPane* m_helpmenu;
    FXStatusBar* m_sb;
    FXLabel* m_sbl1;
    FXMenuPane* m_fl_popup;
    void* m_gui_event;
    int m_sort_order;
    int m_last_header_click_mstime;
    FXDragType m_dnd_types[8];
    int m_width;
    int m_height;
    FXString m_dnd_str;
};

/*****************************************************************************/
GUIObject::GUIObject():FXObject()
{
    m_fi = NULL;
    m_app = NULL;
    m_mutex1 = NULL;
    m_mw = NULL;
    m_but1 = NULL;
    m_but2 = NULL;
    m_but3 = NULL;
    m_but4 = NULL;
    m_ti1 = NULL;
    m_ti2 = NULL;
    m_ti3 = NULL;
    m_gb1 = NULL;
    m_gb2 = NULL;
    m_fl = NULL;
    m_flh = NULL;
    m_tab_book = NULL;
    m_tabframe1 = NULL;
    m_tabframe2 = NULL;
    m_tabframe3 = NULL;
    m_label1 = NULL;
    m_label2 = NULL;
    m_cb1 = NULL;
    m_cb2 = NULL;
    m_cb3 = NULL;
    m_cb4 = NULL;
    m_cb5 = NULL;
    m_combo1 = NULL;
    m_combo2 = NULL;
    m_combo3 = NULL;
    m_text1 = NULL;
    m_topdock = NULL;
    m_tbs = NULL;
    m_mb = NULL;
    m_filemenu = NULL;
    m_helpmenu = NULL;
    m_sb = NULL;
    m_sbl1 = NULL;
    m_fl_popup = NULL;
    m_gui_event = NULL;
    m_sort_order = 0;
    m_last_header_click_mstime = 0;
    memset(m_dnd_types, 0, sizeof(m_dnd_types));
    m_width = 0;
    m_height = 0;
    m_dnd_str = "";
}

/*****************************************************************************/
GUIObject::~GUIObject()
{
    writeln(m_fi, "GUIObject::~GUIObject");
}

/*****************************************************************************/
long
GUIObject::onDefault(FXObject* obj, FXSelector sel, void* ptr)
{
    //writeln(m_fi, "onDefault obj %p sel %d ptr %p", obj, sel, ptr);
    return FXObject::onDefault(obj, sel, ptr);
}

/*****************************************************************************/
static int
save_combo(GUIObject* go, FXComboBox* cb,
           const char* section, const char* key_prefix)
{
    FXString str1;
    FXString str2;
    FXint index;
    FXint count;
    FXRegistry* reg;

    reg = &(go->m_app->reg());
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
static int
save_checkbox(GUIObject* go, FXCheckButton* cb,
              const char* section, const char* key, FXbool default_checked)
{
    FXbool bool1;
    FXRegistry* reg;

    reg = &(go->m_app->reg());
    bool1 = cb->getCheck();
    if ((!bool1) == (!default_checked))
    {
        reg->deleteEntry(section, key);
    }
    else
    {
        reg->writeStringEntry(section, key, default_checked ? "0" : "1");
    }
    return 1;
}

/*****************************************************************************/
long
GUIObject::onPress(FXObject* obj, FXSelector sel, void* ptr)
{
    FXString str1;

    writeln(m_fi, "onPress obj %p sel %d ptr %p", obj, sel, ptr);
    if (obj == m_but1)
    {
        writeln(m_fi, "but1");
        m_fl->clearItems(TRUE);
        str1 = m_combo1->getText();
        snprintf(m_fi->named, sizeof(m_fi->named), "%s", str1.text());
        str1 = m_combo2->getText();
        snprintf(m_fi->look_in, sizeof(m_fi->look_in), "%s", str1.text());
        m_fi->include_subfolders = m_cb1->getCheck();
        m_fi->case_sensitive = m_cb2->getCheck();
        m_fi->show_hidden = m_cb3->getCheck();
        m_fi->search_in_files = m_cb4->getCheck();
        m_fi->search_in_case_sensitive = m_cb5->getCheck();
        str1 = m_combo3->getText();
        snprintf(m_fi->text, sizeof(m_fi->text), "%s", str1.text());

        save_combo(this, m_combo1, "NameLocation", "Named");
        save_combo(this, m_combo2, "NameLocation", "LookIn");
        save_checkbox(this, m_cb1, "NameLocation", "IncludeSubfolders", 1);
        save_checkbox(this, m_cb2, "NameLocation", "CaseSensitiveSearch", 0);
        save_checkbox(this, m_cb3, "NameLocation", "ShowHiddenFiles", 0);

        //ap->fl->hide();
        start_find(m_fi);
        m_but1->disable();
        m_but2->enable();
    }
    if (obj == m_but2)
    {
        writeln(m_fi, "but2");
        stop_find(m_fi);
    }
    if (obj == m_but3)
    {
        writeln(m_fi, "but3");
        //ap->app->stop(0);
    }
    if (obj == m_but4)
    {
        writeln(m_fi, "but4");
        str1 = m_combo2->getText();
        str1 = FXDirDialog::getOpenDirectory(m_mw, "Select Look In directory", str1);
        if (str1 != "")
        {
            m_combo2->setText(str1);
        }
    }

    return 0;
}

/*****************************************************************************/
long
GUIObject::onTabChange(FXObject* obj, FXSelector sel, void* ptr)
{
    return 1;
}

/*****************************************************************************/
long
GUIObject::onConfigure(FXObject* obj, FXSelector sel, void* ptr)
{
    m_app->addTimeout(this, GUIObject::ID_MAINWINDOW, 0, NULL);
    return 0;
}

/*****************************************************************************/
long
GUIObject::onResizeTimeout(FXObject* obj, FXSelector sel, void* ptr)
{
    FXint width;
    FXint height;

    width = m_mw->getWidth();
    height = m_mw->getHeight();
    if ((width != m_width) || (height != m_height))
    {
        //writeln(ap->fi, "GUIObject::onResizeTimeout: resized to %dx%d, was %dx%d",
        //        width, height, ap->width, ap->height);
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

        m_label1->move(8, 8);
        m_label1->resize(100, 24);

        m_combo1->move(85, 8);
        m_combo1->resize(400, 24);

        m_label2->move(8, 41);
        m_label2->resize(100, 24);

        m_combo2->move(85, 41);
        m_combo2->resize(340, 24);

        m_but4->move(340 + 85 + 2, 41);
        m_but4->resize(60, 24);

        m_cb1->move(8, 74);
        m_cb1->resize(200, 24);

        m_cb2->move(8, 107);
        m_cb2->resize(200, 24);

        m_cb3->move(216, 74);
        m_cb3->resize(200, 24);

        m_cb4->move(10, 10);
        m_cb4->resize(120, 24);

        m_combo3->move(10, 40);
        m_combo3->resize(340, 24);

        m_cb5->move(10, 74);
        m_cb5->resize(160, 24);

        m_text1->move(85, 8);
        m_text1->resize(400, 24);

    }
    return 0;
}

/*****************************************************************************/
long
GUIObject::onCmdExit(FXObject* obj, FXSelector sel, void* ptr)
{
    writeln(m_fi, "GUIObject::onCmdExit:");
    m_mw->close(TRUE);
    return 1;
}

/*****************************************************************************/
long
GUIObject::onCmdHelp(FXObject* obj, FXSelector sel, void* ptr)
{
    writeln(m_fi, "GUIObject::onCmdHelp:");
    return 0;
}

/*****************************************************************************/
long
GUIObject::onCmdAbout(FXObject* obj, FXSelector sel, void* ptr)
{
    writeln(m_fi, "GUIObject::onCmdAbout:");
    return 0;
}

/*****************************************************************************/
long
GUIObject::onEvent1(FXObject* obj, FXSelector sel, void* ptr)
{
    finder_event_clear(m_gui_event);
    m_app->addTimeout(this, GUIObject::ID_MAINWINDOW1, 0, NULL);
    return 1;
}

/*****************************************************************************/
long
GUIObject::onEventTimeout(FXObject* obj, FXSelector sel, void* ptr)
{
    event_callback(m_fi);
    return 1;
}

/*****************************************************************************/
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
                if (width > max_width)
                {
                    max_width = width;
                }
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
                if (width > max_width)
                {
                    max_width = width;
                }
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
                if (width > max_width)
                {
                    max_width = width;
                }
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
                if (width > max_width)
                {
                    max_width = width;
                }
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

    writeln(m_fi, "GUIObject::onFLRightMouseUp:");
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

    writeln(m_fi, "GUIObject::onCopyFilename:");
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

    writeln(m_fi, "GUIObject::onCopyFullPath:");
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

    writeln(m_fi, "GUIObject::onClose:");
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
    //writeln(ap->fi, "GUIObject::onClipboardLost:");
    return 1;
}

/*****************************************************************************/
long
GUIObject::onClipboardGained(FXObject* obj, FXSelector sel, void* ptr)
{
    //writeln(ap->fi, "GUIObject::onClipboardGained:");
    return 1;
}

/*****************************************************************************/
long
GUIObject::onClipboardRequest(FXObject* obj, FXSelector sel, void* ptr)
{
    FXuchar* data;
    FXuint len;

    //writeln(ap->fi, "GUIObject::onClipboardRequest:");
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

    //writeln(ap->fi, "GUIObject::onBeginDrag:");
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
    //writeln(ap->fi, "GUIObject::onEndDrag:");
    m_mw->endDrag();
    return 1;
}

/*****************************************************************************/
long
GUIObject::onDragged(FXObject* obj, FXSelector sel, void* ptr)
{
    FXEvent* event;

    //writeln(ap->fi, "GUIObject::onDragged:");
    event = (FXEvent*)ptr;
    m_mw->handleDrag(event->root_x, event->root_y);

    if (m_mw->didAccept() != DRAG_REJECT)
    {
        //setDragCursor(drop_ok_cursor);
        //writeln(ap->fi, "drop_ok_cursor");
    }
    else
    {
        //setDragCursor(drop_not_ok_cursor);
        //writeln(ap->fi, "drop_not_ok_cursor");
    }

    return 1;
}

/*****************************************************************************/
long
GUIObject::onDNDRequest(FXObject* obj, FXSelector sel, void* ptr)
{
    FXuchar* data;
    FXuint len;

    //writeln(ap->fi, "GUIObject::onDNDRequest:");
    len = m_dnd_str.length();
    FXMALLOC(&data, FXuchar, len + 8);
    memcpy(data, m_dnd_str.text(), len);
    data[len] = 0;
    m_mw->setDNDData(FROM_DRAGNDROP, m_dnd_types[1], data, len);
    return 1;
}

FXDEFMAP(GUIObject) GUIObjectMap[] =
{
    FXMAPFUNC(SEL_COMMAND, GUIObject::ID_BUTTON, GUIObject::onPress),
    FXMAPFUNC(SEL_COMMAND, GUIObject::ID_TABBOOK, GUIObject::onTabChange),
    FXMAPFUNC(SEL_CONFIGURE, GUIObject::ID_MAINWINDOW, GUIObject::onConfigure),
    FXMAPFUNC(SEL_TIMEOUT, GUIObject::ID_MAINWINDOW, GUIObject::onResizeTimeout),
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
    FXMAPFUNC(SEL_DND_REQUEST, GUIObject::ID_MAINWINDOW, GUIObject::onDNDRequest)
};

FXIMPLEMENT(GUIObject, FXObject, GUIObjectMap, ARRAYNUMBER(GUIObjectMap))

/*****************************************************************************/
static int
gui_create(int argc, char** argv, struct finder_info** fi)
{
    GUIObject* go;
    FXuint flags;
    FXSelector sel;
    FXCursor* cur;
    FXInputHandle ih;

    *fi = (struct finder_info*)calloc(1, sizeof(struct finder_info));
    go = new GUIObject();
    (*fi)->gui_obj = go;
    go->m_fi = *fi;
    go->m_app = new FXApp("Finder", "Finder");
    go->m_mutex1 = new FXMutex();
    cur = new FXCursor(go->m_app, FX::CURSOR_ARROW);
    go->m_app->setDefaultCursor(DEF_RARROW_CURSOR, cur);
    go->m_app->init(argc, argv);
    go->m_mw = new FXMainWindow(go->m_app, "Finder", NULL, NULL, DECOR_ALL, 0, 0, 640, 480);
    go->m_mw->setTarget(go);
    go->m_mw->setSelector(GUIObject::ID_MAINWINDOW);

    flags = LAYOUT_EXPLICIT;
    go->m_gb1 = new FXGroupBox(go->m_mw, "", flags);

    sel = GUIObject::ID_TABBOOK;
    flags = LAYOUT_FILL_X | LAYOUT_FILL_Y;
    go->m_tab_book = new FXTabBook(go->m_gb1, go, sel, flags);
    go->m_ti1 = new FXTabItem(go->m_tab_book, "Name/&Location");
    flags = LAYOUT_FILL_X | LAYOUT_FILL_Y | FRAME_THICK | FRAME_RAISED;
    go->m_tabframe1 = new FXGroupBox(go->m_tab_book, "", flags);
    go->m_ti2 = new FXTabItem(go->m_tab_book, "&Date Modified");
    go->m_tabframe2 = new FXGroupBox(go->m_tab_book, "", flags);
    go->m_ti3 = new FXTabItem(go->m_tab_book, "&Advanced");
    go->m_tabframe3 = new FXGroupBox(go->m_tab_book, "", flags);

    flags = LABEL_NORMAL | LAYOUT_EXPLICIT | JUSTIFY_LEFT;
    go->m_label1 = new FXLabel(go->m_tabframe1, "&Named:", NULL, flags);

    flags = FRAME_SUNKEN | FRAME_THICK | LAYOUT_EXPLICIT;
    go->m_combo1 = new FXComboBox(go->m_tabframe1, 0, NULL, 0, flags);
    go->m_combo1->setNumVisible(10);

    flags = LABEL_NORMAL | LAYOUT_EXPLICIT | JUSTIFY_LEFT;
    go->m_label2 = new FXLabel(go->m_tabframe1, "Look &in:", NULL, flags);

    flags = FRAME_SUNKEN | FRAME_THICK | LAYOUT_EXPLICIT;
    go->m_combo2 = new FXComboBox(go->m_tabframe1, 0, NULL, 0, flags);
    go->m_combo2->setNumVisible(10);

    sel = GUIObject::ID_BUTTON;
    flags = BUTTON_NORMAL | LAYOUT_EXPLICIT;
    go->m_but4 = new FXButton(go->m_tabframe1, "&Browse", NULL, go, sel, flags);

    flags = CHECKBUTTON_NORMAL | LAYOUT_EXPLICIT | JUSTIFY_LEFT;
    go->m_cb1 = new FXCheckButton(go->m_tabframe1, "Include subfolders", NULL, 0, flags);
    go->m_cb1->setCheck(TRUE);
    go->m_cb2 = new FXCheckButton(go->m_tabframe1, "Case sensitive search", NULL, 0, flags);
    go->m_cb3 = new FXCheckButton(go->m_tabframe1, "Show hidden files", NULL, 0, flags);

    flags = TEXTFIELD_NORMAL;
    go->m_text1 = new FXTextField(go->m_tabframe2, 0, NULL, 0, flags);

    flags = CHECKBUTTON_NORMAL | LAYOUT_EXPLICIT | JUSTIFY_LEFT;
    go->m_cb4 = new FXCheckButton(go->m_tabframe3, "Search in files");

    flags = FRAME_SUNKEN | FRAME_THICK | LAYOUT_EXPLICIT;
    go->m_combo3 = new FXComboBox(go->m_tabframe3, 0, NULL, 0, flags);

    flags = CHECKBUTTON_NORMAL | LAYOUT_EXPLICIT | JUSTIFY_LEFT;
    go->m_cb5 = new FXCheckButton(go->m_tabframe3, "Case sensitive search");

    sel = GUIObject::ID_BUTTON;
    flags = BUTTON_NORMAL | LAYOUT_EXPLICIT;
    go->m_but1 = new FXButton(go->m_mw, "&Find", NULL, go, sel, flags);
    go->m_but2 = new FXButton(go->m_mw, "&Stop", NULL, go, sel, flags);
    go->m_but2->disable();
    sel = GUIObject::ID_EXIT;
    go->m_but3 = new FXButton(go->m_mw, "Exit", NULL, go, sel, flags);

    flags = LAYOUT_EXPLICIT;
    go->m_gb2 = new FXGroupBox(go->m_mw, "", flags);

    sel = GUIObject::ID_FOLDINGLIST;
    flags = FOLDINGLIST_NORMAL | LAYOUT_FILL_X | LAYOUT_FILL_Y;
    go->m_fl = new FXFoldingList(go->m_gb2, go, sel, flags);
    go->m_fl->appendHeader("Name", 0, 100);
    go->m_fl->appendHeader("In Subfolder", 0, 100);
    go->m_fl->appendHeader("Size", 0, 100);
    go->m_fl->appendHeader("Modified", 0, 100);
    go->m_flh = go->m_fl->getHeader();
    go->m_flh->setTarget(go);
    go->m_flh->setSelector(GUIObject::ID_FOLDINGLISTHEADER);

    go->m_fl_popup = new FXMenuPane(go->m_fl);
    sel = GUIObject::ID_COPY_FILENAME;
    new FXMenuCommand(go->m_fl_popup, "&Copy filename\t\tCopy the filename to clipboard.", NULL, go, sel);
    new FXMenuSeparator(go->m_fl_popup);
    sel = GUIObject::ID_COPY_FULL_PATH;
    new FXMenuCommand(go->m_fl_popup, "&Copy full path\t\tCopy the full path to clipboard.", NULL, go, sel);

    flags = LAYOUT_SIDE_TOP | LAYOUT_FILL_X;
    go->m_topdock = new FXDockSite(go->m_mw, flags);

    flags = FRAME_RAISED;
    go->m_tbs = new FXToolBarShell(go->m_mw, flags);

    flags = LAYOUT_DOCK_NEXT | LAYOUT_SIDE_TOP | LAYOUT_FILL_X | FRAME_RAISED;
    go->m_mb = new FXMenuBar(go->m_topdock, go->m_tbs, flags);

    go->m_filemenu = new FXMenuPane(go->m_mw);
    new FXMenuTitle(go->m_mb, "&File", NULL, go->m_filemenu);
    sel = GUIObject::ID_EXIT;
    new FXMenuCommand(go->m_filemenu, "&Exit\t\tExit the application.", NULL, go, sel);

    go->m_helpmenu = new FXMenuPane(go->m_mw);
    new FXMenuTitle(go->m_mb, "&Help", NULL, go->m_helpmenu);
    sel = GUIObject::ID_HELP;
    new FXMenuCommand(go->m_helpmenu, "&Help...\t\tDisplay help information.", NULL, go, sel);
    sel = GUIObject::ID_ABOUT;
    new FXMenuCommand(go->m_helpmenu, "&About\t\tDisplay version information.", NULL, go, sel);

    flags = LAYOUT_SIDE_BOTTOM | LAYOUT_FILL_X | STATUSBAR_WITH_DRAGCORNER | FRAME_RAISED;
    go->m_sb = new FXStatusBar(go->m_mw, flags);

    go->m_sbl1 = new FXLabel(go->m_sb, "", NULL,LAYOUT_RIGHT | LAYOUT_CENTER_Y);

    go->m_app->create();
    go->m_mw->show(PLACEMENT_SCREEN);

    finder_event_create(&(go->m_gui_event));
    ih = (FXInputHandle)finder_event_get_wait_obj(go->m_gui_event);
    go->m_app->addInput(ih, INPUT_READ, go, GUIObject::ID_SOCKET);

    go->m_dnd_types[0] = go->m_app->registerDragType("UTF8_STRING");
    go->m_dnd_types[1] = go->m_app->registerDragType("text/uri-list");

    return 0;
}

/*****************************************************************************/
static int
load_stuff(GUIObject* go)
{
    FXint index;
    FXRegistry* reg;
    FXString key;
    FXString val;

    reg = &(go->m_app->reg());
    for (index = 0; index < 100; index++)
    {
        key.format("Named%2.2d", index);
        val = reg->readStringEntry("NameLocation", key.text(), "_NoWay_");
        if (val != "_NoWay_")
        {
            go->m_combo1->appendItem(val);
        }
    }
    for (index = 0; index < 100; index++)
    {
        key.format("LookIn%2.2d", index);
        val = reg->readStringEntry("NameLocation", key.text(), "_NoWay_");
        if (val != "_NoWay_")
        {
            go->m_combo2->appendItem(val);
        }
    }
    val = reg->readStringEntry("NameLocation", "IncludeSubfolders", "_NoWay_");
    if (val != "_NoWay_")
    {
        go->m_cb1->setCheck(atoi(val.text()));
    }
    val = reg->readStringEntry("NameLocation", "CaseSensitiveSearch", "_NoWay_");
    if (val != "_NoWay_")
    {
        go->m_cb2->setCheck(atoi(val.text()));
    }
    val = reg->readStringEntry("NameLocation", "ShowHiddenFiles", "_NoWay_");
    if (val != "_NoWay_")
    {
        go->m_cb3->setCheck(atoi(val.text()));
    }

    return 0;
}

/*****************************************************************************/
static int
gui_main_loop(struct finder_info* fi)
{
    GUIObject* go;

    writeln(fi, "gui_main_loop");
    go = (GUIObject*)(fi->gui_obj);
    gui_init(go->m_fi);
    load_stuff(go);
    go->m_app->run();
    gui_deinit(go->m_fi);
    return 0;
}

/*****************************************************************************/
static int
gui_delete(struct finder_info* fi)
{
    GUIObject* go;

    writeln(fi, "gui_delete");
    go = (GUIObject*)(fi->gui_obj);
    go->m_app->exit(); /* close display, write registry */
    delete go->m_app;
    delete go->m_mutex1;
    delete go;
    finder_event_delete(go->m_gui_event);
    free(fi);
    return 0;
}

/*****************************************************************************/
/* any thread */
int
gui_set_event(struct finder_info* fi)
{
    GUIObject* go;

    //writeln(fi, "gui_set_event");
    go = (GUIObject*)(fi->gui_obj);
    go->m_mutex1->lock();
    finder_event_set(go->m_gui_event);
    go->m_mutex1->unlock();
    return 0;
}

/*****************************************************************************/
int
main(int argc, char** argv)
{
    struct finder_info* fi;

    if (sizeof(FINDER_I64) != 8)
    {
        printf("main: bad FINDER_I64\n");
        return 1;
    }
    if (sizeof(FINDER_INTPTR) != sizeof(void*))
    {
        printf("main: bad FINDER_INTPTR\n");
        return 1;
    }
    gui_create(argc, argv, &fi);
    gui_main_loop(fi);
    gui_delete(fi);
    return 0;
}

/*****************************************************************************/
int
gui_find_done(struct finder_info* fi)
{
    GUIObject* go;
    int count;
    int width;
    int max_width[4];
    FXString str1;
    FXFont* ft;
    ItemObject* io;
    FXFoldingItem* folding_item;

    writeln(fi, "gui_find_done");
    go = (GUIObject*)(fi->gui_obj);
    go->m_but1->enable();
    go->m_but2->disable();
    count = go->m_fl->getNumItems();
    str1.format("%d Items found", count);
    go->m_sbl1->setText(str1);

    //ap->fl->show();

    /* resize the columns */
    ft = go->m_fl->getFont();
    max_width[0] = ft->getTextWidth("Name");
    max_width[1] = ft->getTextWidth("In Subfolder");
    max_width[2] = ft->getTextWidth("Size");
    max_width[3] = ft->getTextWidth("Modified");
    folding_item = go->m_fl->getFirstItem();
    while (folding_item != NULL)
    {
        io = (ItemObject*)(folding_item->getData());
        width = ft->getTextWidth(io->filename);
        if (width > max_width[0])
        {
            max_width[0] = width;
        }
        width = ft->getTextWidth(io->in_subfolder);
        if (width > max_width[1])
        {
            max_width[1] = width;
        }
        width = ft->getTextWidth(io->size_text);
        if (width > max_width[2])
        {
            max_width[2] = width;
        }
        width = ft->getTextWidth(io->modified);
        if (width > max_width[3])
        {
            max_width[3] = width;
        }
        folding_item = folding_item->getNext();
    }
    go->m_fl->setHeaderSize(0, max_width[0] + 8);
    go->m_fl->setHeaderSize(1, max_width[1] + 8);
    go->m_fl->setHeaderSize(2, max_width[2] + 8);
    go->m_fl->setHeaderSize(3, max_width[3] + 8);

    return 0;
}

/*****************************************************************************/
int
gui_add_one(struct finder_info* fi, const char* filename,
            const char* in_subfolder, FINDER_I64 size,
            const char* modified)
{
    GUIObject* go;
    FXFoldingItem* folding_item;
    FXString str1;
    ItemObject* io;
    char text[128];

    io = new ItemObject();

    //writeln(fi, "gui_add_one");
    go = (GUIObject*)(fi->gui_obj);
    str1 = filename;
    io->filename = filename;
    str1 += "\t";
    str1 += in_subfolder;
    io->in_subfolder = in_subfolder;
    str1 += "\t";
    format_commas(size, text);
    io->size = size;
    io->size_text = text;
    str1 += text;
    str1 += "\t";
    str1 += modified;
    io->modified = modified;
    folding_item = new FXFoldingItem(str1);
    folding_item->setDraggable(TRUE);
    go->m_fl->appendItem(NULL, folding_item, TRUE);
    folding_item->setData(io);
    return 0;
}
