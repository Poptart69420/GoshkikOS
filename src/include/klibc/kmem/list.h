#ifndef LIST_H_
#define LIST_H_

#include <klibc/kmem/string.h>
#include <scheduling/mutex.h>

#define FOR_EACH(var, l) for (list_node_t *var = l->first_node; var; var = var->next)

struct list_struct;

typedef struct list_node_struct
{
  struct list_struct *list;
  struct list_node_struct *next;
  struct list_node_struct *prev;
  void *value;
} list_node_t;

typedef struct list_struct
{
  list_node_t *first_node;
  list_node_t *last_node;
  size_t node_count;
  mutex_t mutex;
} list_t;

list_t *create_list(void);
void free_list(list_t *list);
void list_append(list_t *list, void *value);
void list_append_after(list_t *list, list_node_t *node, void *value);

#endif // LIST_H_
