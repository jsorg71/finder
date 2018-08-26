
#if !defined(FINDER_GUI_H)
#define FINDER_GUI_H

#ifdef __cplusplus
extern "C" {
#endif

int
gui_set_event(struct finder_info* fi);
int
gui_find_done(struct finder_info* fi);
int
gui_add_one(struct finder_info* fi, const char* filename,
            const char* in_subfolder, const char* size,
            const char* modified);

#ifdef __cplusplus
}
#endif

#endif

