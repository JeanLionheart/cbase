#ifndef __TREEH__
#define __TREEH__

// #define debugging

typedef struct tree_s tree_t;

struct tree_s
{
    void *root;
    int element_size;
    int size;
};

tree_t tree_construct(int element_size);

int tree_size(const tree_t *tree);

int tree_element_size(const tree_t *tree);

void *tree_get_begin(const tree_t *tree);

void *tree_get_end(const tree_t *tree);

void tree_insert(tree_t *tree, long key, const void *src);

int tree_get(const tree_t *tree, long key, void *dst);

void tree_erase(tree_t *tree, long key);

int tree_empty(const tree_t *tree);

void tree_destruct(tree_t *tree);
#ifdef debugging
void tree_debug(tree_t*tree);
#endif

#endif