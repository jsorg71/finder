
#if !defined(FINDER_GUI_H)
#define FINDER_GUI_H

#ifdef __cplusplus
extern "C" {
#endif

void*
gui_create(int argc, char** argv);
int
gui_main_loop(void* han);
int
gui_delete(void* han);

#ifdef __cplusplus
}
#endif

#endif

