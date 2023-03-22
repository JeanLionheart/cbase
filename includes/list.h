#ifndef __LISTH__
#define __LISTH__

typedef struct list_s list_t;

struct list_s
{
    void *begin, *end; // reinterpret in list.c as struct node
    int element_size;
    int size;
};

list_t list_construct(int element_size);

int list_size(const list_t *l);

void list_insert_before(list_t *l, const void *src);

void list_append(list_t *l, const void *src);

void *list_get_begin(const list_t *l);

void *list_get_end(const list_t *l);

void *list_next_node(const list_t *l, void *node);

void *list_prev_node(const list_t *l, void *node);

void list_node_get_value(const list_t *l,const void *node, void *dst);

void list_node_erase(list_t *l, void *node);

void list_deconstruct(list_t *l);

int list_empty(const list_t *l);

#endif