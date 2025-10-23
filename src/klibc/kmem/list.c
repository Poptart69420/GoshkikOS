#include <klibc/kmem/list.h>
#include <scheduling/mutex.h>

list_t *create_list(void)
{
  list_t *list = kmalloc(sizeof(list_t));
  list->first_node = NULL;
  list->last_node = NULL;
  list->node_count = 0;
  init_mutex(&list->mutex);
  return list;
}

void free_list(list_t *list)
{
  list_node_t *prev = NULL;

  FOR_EACH(node, list)
  {
    if (prev)
      kfree(prev);

    prev = node;
  }

  if (prev)
    kfree(prev);

  kfree(list);
}

void list_append(list_t *list, void *value)
{
  list_node_t *new_node = kmalloc(sizeof(list_node_t));
  memset(new_node, 0, sizeof(list_node_t));
  new_node->value = value;

  acquire_mutex(&list->mutex);

  new_node->prev = list->last_node;
  if (list->last_node)
  {
    list->last_node->next = new_node;
  }
  else
  {
    list->first_node = new_node;
  }

  list->last_node = new_node;

  list->node_count++;
  release_mutex(&list->mutex);
}

void list_append_after(list_t *list, list_node_t *node, void *value)
{
  if (node)
  {
    list_node_t *new_node = kmalloc(sizeof(list_node_t));
    new_node->value = value;
    acquire_mutex(&list->mutex);

    new_node->prev = node;
    new_node->next = node->next;
    node->next = new_node;

    if (new_node->next)
    {
      new_node->next->prev = new_node;
    }
    else
    {
      list->last_node = new_node;
    }
  }
  else
  {
    list_append(list, value);
  }
}

void list_remove(list_t *list, void *value)
{
  acquire_mutex(&list->mutex);
  FOR_EACH(node, list)
  {
    if (node->value == value)
    {
      if (node->prev)
      {
        node->prev->next = node->next;
      }
      else
      {
        list->first_node = node->next;
      }
      if (node->next)
      {
        node->next->prev = node->prev;
      }
      else
      {
        list->last_node = node->prev;
      }
      list->node_count--;
      release_mutex(&list->mutex);
      return;
    }
  }
  release_mutex(&list->mutex);
}
