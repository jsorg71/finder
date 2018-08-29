
#if !defined(FINDER_H)
#define FINDER_H

/* main to work */
#define FINDER_CMD_START        1
/* work to main */
#define FINDER_CMD_DONE         101
#define FINDER_CMD_ADD_ONE      102

struct work_item
{
    int cmd;
    int pad0;
    char* filename;
    char* in_subfolder;
    char* size;
    char* modified;
};

struct finder_info
{
    void* gui_obj;
    char named[256];
    char look_in[256];
    int include_subfolders;
    int case_sensitive;
    int show_hidden;
    int search_in_files;
    int search_in_case_sensitive;
    char text[256];

    void* work_term_event;
    void* main_to_work_event;
    void* list_mutex;
    void* main_to_work_list;
    void* work_to_main_list;
};

#ifdef __cplusplus
extern "C" {
#endif

int
gui_init(struct finder_info* fi);
int
gui_deinit(struct finder_info* fi);
int
writeln(struct finder_info* fi, const char* format, ...);
int
start_find(struct finder_info* fi);
int
stop_find(struct finder_info* fi);
int
event_callback(struct finder_info* fi);

#ifdef __cplusplus
}
#endif

#endif

