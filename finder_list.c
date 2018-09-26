/**
 * finder: A file finder GUI application.
 *
 * Copyright (C) Jay Sorg 2018
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

#include "finder_list.h"

/* finder_list */
struct finder_list
{
    ITYPE* items;
    int count;
    int alloc_size;
    int grow_by;
    int auto_free;
};

/*****************************************************************************/
int
finder_list_create(int grow_by, int alloc_size, void** list1)
{
    struct finder_list* self;

    self = (struct finder_list*)calloc(1, sizeof(struct finder_list));
    if (self == NULL)
    {
        return 1;
    }
    if ((grow_by == 0) && (alloc_size == 0))
    {
        grow_by = 10;
        alloc_size = 10;
    }
    if (grow_by < 1)
    {
        grow_by = 1;
    }
    if (alloc_size < 1)
    {
        alloc_size = 1;
    }
    self->grow_by = grow_by;
    self->alloc_size = alloc_size;
    self->items = (ITYPE*)calloc(self->alloc_size, sizeof(ITYPE));
    if (self->items == NULL)
    {
        free(self);
        return 2;
    }
    *list1 = self;
    return 0;
}

/*****************************************************************************/
void
finder_list_delete(void* list1)
{
    struct finder_list* self;
    int i;

    self = (struct finder_list*)list1;
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
int
finder_list_add_item(void* list1, ITYPE item)
{
    struct finder_list* self;
    ITYPE* p;
    int i;

    self = (struct finder_list*)list1;
    if (self->count >= self->alloc_size)
    {
        i = self->alloc_size;
        self->alloc_size += self->grow_by;
        p = (ITYPE*)calloc(self->alloc_size, sizeof(ITYPE));
        if (p == NULL)
        {
            return 1;
        }
        memcpy(p, self->items, sizeof(ITYPE) * i);
        free(self->items);
        self->items = p;
    }
    self->items[self->count] = item;
    self->count++;
    return 0;
}

/*****************************************************************************/
ITYPE
finder_list_get_item(const void* list1, int index)
{
    const struct finder_list* self;

    self = (const struct finder_list*)list1;
    if ((index < 0) || (index >= self->count))
    {
        return 0;
    }
    return self->items[index];
}

/*****************************************************************************/
int
finder_list_clear(void* list1, int grow_by, int alloc_size)
{
    struct finder_list* self;
    int i;

    self = (struct finder_list*)list1;
    if (self->auto_free)
    {
        for (i = 0; i < self->count; i++)
        {
            free((void*)(self->items[i]));
            self->items[i] = 0;
        }
    }
    free(self->items);
    if ((grow_by == 0) && (alloc_size == 0))
    {
        grow_by = 10;
        alloc_size = 10;
    }
    if (grow_by < 1)
    {
        grow_by = 1;
    }
    if (alloc_size < 1)
    {
        alloc_size = 1;
    }
    self->count = 0;
    self->grow_by = grow_by;
    self->alloc_size = alloc_size;
    self->items = (ITYPE*)calloc(self->alloc_size, sizeof(ITYPE));
    if (self->items == NULL)
    {
        return 1;
    }
    return 0;
}

/*****************************************************************************/
int
finder_list_index_of(const void* list1, ITYPE item)
{
    const struct finder_list* self;
    int i;

    self = (const struct finder_list*)list1;
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
finder_list_remove_item(void* list1, int index)
{
    struct finder_list* self;
    int i;

    self = (struct finder_list*)list1;
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
int
finder_list_insert_item(void* list1, int index, ITYPE item)
{
    struct finder_list* self;
    ITYPE* p;
    int i;

    self = (struct finder_list*)list1;
    if (index == self->count)
    {
        return finder_list_add_item(self, item);
    }
    if ((index >= 0) && (index < self->count))
    {
        self->count++;
        if (self->count > self->alloc_size)
        {
            i = self->alloc_size;
            self->alloc_size += self->grow_by;
            p = (ITYPE*)calloc(self->alloc_size, sizeof(ITYPE));
            if (p == NULL)
            {
                return 1;
            }
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
    return 0;
}

/*****************************************************************************/
int
finder_list_get_count(const void* list1)
{
    const struct finder_list* self;

    self = (const struct finder_list*)list1;
    return self->count;
}