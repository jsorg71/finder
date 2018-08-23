
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "finder_list.h"

/*****************************************************************************/
struct finder_list*
finder_list_create(void)
{
    struct finder_list *self;

    self = (struct finder_list*)calloc(1, sizeof(struct finder_list));
    self->grow_by = 10;
    self->alloc_size = 10;
    self->items = (ITYPE*)calloc(1, sizeof(ITYPE) * 10);
    return self;
}

/*****************************************************************************/
void
finder_list_delete(struct finder_list* self)
{
    int i;

    if (self == NULL)
    {
        return;
    }
    if (self->auto_free)
    {
        for (i = 0; i < self->count; i++)
        {
            free((void*)(self->items[i]));
            self->items[i] = 0;
        }
    }
    free(self->items);
    free(self);
}

/*****************************************************************************/
void
finder_list_add_item(struct finder_list* self, ITYPE item)
{
    ITYPE* p;
    int i;

    if (self->count >= self->alloc_size)
    {
        i = self->alloc_size;
        self->alloc_size += self->grow_by;
        p = (ITYPE*)calloc(1, sizeof(ITYPE) * self->alloc_size);
        memcpy(p, self->items, sizeof(ITYPE) * i);
        free(self->items);
        self->items = p;
    }
    self->items[self->count] = item;
    self->count++;
}

/*****************************************************************************/
ITYPE
finder_list_get_item(const struct finder_list *self, int index)
{
    if ((index < 0) || (index >= self->count))
    {
        return 0;
    }
    return self->items[index];
}

/*****************************************************************************/
void
finder_list_clear(struct finder_list* self)
{
    int i;

    if (self->auto_free)
    {
        for (i = 0; i < self->count; i++)
        {
            free((void*)(self->items[i]));
            self->items[i] = 0;
        }
    }
    free(self->items);
    self->count = 0;
    self->grow_by = 10;
    self->alloc_size = 10;
    self->items = (ITYPE*)calloc(1, sizeof(ITYPE) * 10);
}

/*****************************************************************************/
int
finder_list_index_of(struct finder_list* self, ITYPE item)
{
    int i;

    for (i = 0; i < self->count; i++)
    {
        if (self->items[i] == item)
        {
            return i;
        }
    }
    return -1;
}

/*****************************************************************************/
void
finder_list_remove_item(struct finder_list* self, int index)
{
    int i;

    if (index >= 0 && index < self->count)
    {
        if (self->auto_free)
        {
            free((void*)(self->items[index]));
            self->items[index] = 0;
        }
        for (i = index; i < (self->count - 1); i++)
        {
            self->items[i] = self->items[i + 1];
        }
        self->count--;
    }
}

/*****************************************************************************/
void
finder_list_insert_item(struct finder_list* self, int index, ITYPE item)
{
    ITYPE* p;
    int i;

    if (index == self->count)
    {
        finder_list_add_item(self, item);
        return;
    }
    if ((index >= 0) && (index < self->count))
    {
        self->count++;
        if (self->count > self->alloc_size)
        {
            i = self->alloc_size;
            self->alloc_size += self->grow_by;
            p = (ITYPE*)calloc(1, sizeof(ITYPE) * self->alloc_size);
            memcpy(p, self->items, sizeof(ITYPE) * i);
            free(self->items);
            self->items = p;
        }
        for (i = (self->count - 2); i >= index; i--)
        {
            self->items[i + 1] = self->items[i];
        }
        self->items[index] = item;
    }
}
