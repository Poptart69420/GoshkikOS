#include "list.h"

list_t *new_list(void) {
    list_t *l = kmalloc(sizeof(list_t));
    l->first_node = NULL;
    l->last_node = NULL;
    l->node_count = 0;

    return l;
}

void free_list(list_t *l) {
    list_node_t *prev = NULL;
    foreach (node, l) {
        prev = node;
    }

    if (prev)
        kfree(prev);

    kfree(l);
}

void list_append(list_t *l, void *value) {
    list_node_t *new_node = kmalloc(sizeof(list_node_t));
    memset(new_node, 0, sizeof(list_node_t));
    new_node->value = value;

    new_node->prev = l->last_node;

    if (l->last_node) {
        l->last_node->next = new_node;
    } else {
        l->first_node = new_node;
    }

    l->last_node = new_node;
    l->node_count++;
}

void list_add_after(list_t *l, list_node_t *node, void *value) {
    if (node) {
        list_node_t *new_node = kmalloc(sizeof(list_node_t));
        new_node->value = value;
        new_node->prev = node;
        new_node->next = node->next;
        node->next = new_node;

        if (new_node->next) {
            new_node->next->prev = new_node;
        } else {
            l->last_node = new_node;
        }
    } else {
        list_node_t *new_node = kmalloc(sizeof(list_node_t));
        new_node->value = value;
        new_node->prev = NULL;

        if (l->first_node) {
            l->first_node->prev = new_node;
        }

        new_node->next = l->first_node;
        l->first_node = new_node;
    }

    l->node_count++;
}

void list_remove(list_t *l, const void *value) {
    foreach (node, l) {
        if (node->value == value) {
            if (node->prev) {
                node->prev->next = node->next;
            } else {
                l->first_node = node->prev;
            }

            if (node->next) {
                node->next->prev = node->prev;
            } else {
                l->last_node = node->prev;
            }

            l->node_count--;
        }
    }
}
