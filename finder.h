
#if !defined(FINDER_H)
#define FINDER_H

struct finder_info
{
    void* gui_obj;
    char named[256];
    char look_in[256];
    int include_subfolders;
    int case_sensitive;
    int show_hidden;
    int search_in_files;
    char text[256];
};

#ifdef __cplusplus
extern "C" {
#endif

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

