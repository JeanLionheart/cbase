#include <list.h>
#include <malloc.h>
#include <string.h>

list_t list_construct(int element_size);

int list_size(const list_t* l);

void list_append(list_t* l, const void* src);

void* list_get_begin(const list_t* l);

void list_node_get_value(const list_t* l, const void* node, void* dst);

void list_node_erase(list_t* l, void* node);

void list_destruct(list_t* l);

int list_empty(const list_t* l);
/*  */
typedef struct lnode_s lnode_t;
struct lnode_s {
    /* data */
    lnode_t *prev, *next;
    void* value;
};

lnode_t* list_node_construct(list_t* l, lnode_t* prev, lnode_t* next, const void* src)
{
    lnode_t* node = (lnode_t*)malloc(sizeof(lnode_t));
    node->value = malloc(l->element_size);
    node->prev = prev;
    node->next = next;
    memmove(node->value, src, l->element_size);
    l->size++;
    return node;
}

void list_node_deconstruct(list_t* l, lnode_t* node)
{
    free(node->value);
    free(node);
    l->size--;
}

list_t list_construct(int element_size)
{
    list_t l;
    l.element_size = element_size;
    l.begin = l.end = 0;
    l.size = 0;
    return l;
}

int list_size(const list_t* l)
{
    return l->size;
}

void list_append(list_t* l, const void* src)
{
    lnode_t* end = (lnode_t*)l->end;
    if (l->size == 0) {
        l->end = l->begin = list_node_construct(l, 0, 0, src);
    } else {
        end->next = list_node_construct(l, end, 0, src);
        l->end = end->next;
    }
}

void* list_get_begin(const list_t* l)
{
    return l->begin;
}

void list_node_get_value(const list_t* l, const void* node, void* dst)
{
    const lnode_t* lnode = (lnode_t*)node;
    memmove(dst, lnode->value, l->element_size);
}

void list_node_erase(list_t* l, void* node)
{
    lnode_t* lnode = (lnode_t*)node;
    if (l->size == 1) {
        l->begin = l->end = 0;
        list_node_deconstruct(l, (lnode_t*)node);
        return;
    }

    if (lnode == l->begin) {
        l->begin = lnode->next;
        lnode->next->prev = 0;
    } else if (lnode == l->end) {
        l->end = lnode->prev;
        lnode->prev->next = 0;
    } else {
        lnode->prev->next = lnode->next;
        lnode->next->prev = lnode->prev;
    }
    list_node_deconstruct(l, lnode);
}

void list_destruct(list_t* l)
{
    while (!list_empty(l)) {
        list_node_erase(l, l->begin);
    }
}

int list_empty(const list_t* l)
{
    return l->size == 0;
}