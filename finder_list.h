
#if !defined(FINDER_LIST_H)
#define FINDER_LIST_H

#define ITYPE size_t

#ifdef __cplusplus
extern "C" {
#endif

int
finder_list_create(int grow_by, int alloc_size, void** list1);
void
finder_list_delete(void* list1);
int
finder_list_add_item(void* list1, ITYPE item);
ITYPE
finder_list_get_item(const void* list1, int index);
int
finder_list_clear(void* list1, int grow_by, int alloc_size);
int
finder_list_index_of(const void* list1, ITYPE item);
void
finder_list_remove_item(void* list1, int index);
int
finder_list_insert_item(void* list1, int index, ITYPE item);
int
finder_list_get_count(const void* list1);

#ifdef __cplusplus
}
#endif

#endif

