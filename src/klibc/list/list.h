#ifndef LIST_H_
#define LIST_H_

#include "../mem/kheap.h"
#include "../mem/mem.h"
#include <stddef.h>

struct list_struct_t;

typedef struct list_node_struct_t {
    struct list_struct_t *list;
    struct list_node_struct_t *next;
    struct list_node_struct_t *prev;
    void *value;
} list_node_t;

typedef struct list_struct_t {
    list_node_t *first_node;
    list_node_t *last_node;
    size_t node_count;
} list_t;

#define foreach(var, l)                                                        \
    for (list_node_t *var = l->first_node; var; var = var->next)

list_t *new_list(void);
void free_list(list_t *l);
void list_append(list_t *l, void *value);
void list_add_after(list_t *l, list_node_t *node, void *value);
void list_remove(list_t *l, const void *value);

#endif // LIST_H_
