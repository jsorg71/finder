
#if !defined(FINDER_FILE_H)
#define FINDER_FILE_H

#ifdef __cplusplus
extern "C" {
#endif

int
finder_file_read_sections(const char* file_name, void* names_list);
int
finder_file_read_section(const char* file_name, const char* section,
                         void* names_list, void* values_list);
int
finder_file_write_section(const char* file_name, const char* section,
                          void* names_list, void* values_list);

#ifdef __cplusplus
}
#endif

#endif

