
#if !defined(FINDER_LIST_H)
#define FINDER_LIST_H

#define ITYPE size_t

/* finder_list */
struct finder_list
{
    ITYPE* items;
    int count;
    int alloc_size;
    int grow_by;
    int auto_free;
};

#ifdef __cplusplus
extern "C" {
#endif

struct finder_list*
finder_list_create(void);
void
finder_list_delete(struct finder_list* self);
void
finder_list_add_item(struct finder_list* self, ITYPE item);
ITYPE
finder_list_get_item(const struct finder_list *self, int index);
void
finder_list_clear(struct finder_list* self);
int
finder_list_index_of(struct finder_list* self, ITYPE item);
void
finder_list_remove_item(struct finder_list* self, int index);
void
finder_list_insert_item(struct finder_list* self, int index, ITYPE item);

#ifdef __cplusplus
}
#endif

#endif

