#include<config.h>
#include <mem.h>
#include <string.h>
#include <tree.h>
#define maxm(a, b) (a > b ? a : b)
#define minm(a, b) (a < b ? a : b)
#define abso(node) \
    (maxm(node->left_height, node->right_height) - minm(node->left_height, node->right_height))
#define dec_hei(node) (node->left_height - node->right_height)
#define node_height(node) \
    (node->left_height > node->right_height ? node->left_height : node->right_height)
#ifdef debugging
#include <assert.h>
#include <malloc.h>
#include <stdio.h>
#define debug_node(node) \
    printf("key:%d, lh:%d, rh:%d\n", node->key, node->left_height, node->right_height)
#define assert_node(node)  \
    if (abso(node) >= 2) { \
        debug_node(node);  \
        assert(0);         \
    }
#endif
tree_t tree_construct(int element_size);

int tree_size(const tree_t* tree);

int tree_element_size(const tree_t* tree);

void* tree_get_begin(const tree_t* tree);

void* tree_get_end(const tree_t* tree);

void tree_insert(tree_t* tree, long key, const void* src);

int tree_get(const tree_t* tree, long key, void* dst);

void tree_erase(tree_t* tree, long key);

int tree_empty(const tree_t* tree);

void tree_destruct(tree_t* tree);

/*
private struct and func
*/
typedef struct tree_node_s node_t;
struct tree_node_s {
    /* data */
    node_t *left, *right, *parent;
    int left_height, right_height;
    long key;
    void* val;
};

/*
        1                  3
    2       3   ===>    1     b
   x y     q  b       2   q
 */

node_t* left_balance(tree_t* tree, node_t* node)
{
    node_t* right = node->right;
    node_t* right_l = right->left;
    // right_height >left_height ==> right can't be NULL

    int flag = 0;
    if (node != tree->root) {
        node_t* p = node->parent;
        if (p->left == node) {
            p->left = right;
            right->parent = p;

            right->left = node;
            node->parent = right;

            node->right = right_l;
            if (right_l)
                right_l->parent = node;
        } else {
            p->right = right;
            right->parent = p;

            right->left = node;
            node->parent = right;

            node->right = right_l;
            if (right_l)
                right_l->parent = node;
        }
    } else {
        flag = 1;

        tree->root = right;
        right->parent = 0;

        right->left = node;
        node->parent = right;

        node->right = right_l;
        if (right_l)
            right_l->parent = node;
    }
    node->right_height = node->right ? node_height(node->right) + 1 : 1;
    right->left_height = node_height(node) + 1;
    if (!flag) {
        if (right->parent->left == right) {
            right->parent->left_height = node_height(right) + 1;
        } else {
            right->parent->right_height = node_height(right) + 1;
        }
    }
    return right;
}
node_t* right_balance(tree_t* tree, node_t* node)
{
    node_t* left = node->left;
    node_t* left_r = left->right;
    // right_height < left_height ==> left can't be NULL

    int flag = 0;
    if (node != tree->root) {
        node_t* p = node->parent;
        if (p->left == node) {
            p->left = left;
            left->parent = p;

            left->right = node;
            node->parent = left;

            node->left = left_r;
            if (left_r)
                left_r->parent = node;
        } else {
            p->right = left;
            left->parent = p;

            left->right = node;
            node->parent = left;

            node->left = left_r;
            if (left_r)
                left_r->parent = node;
        }
    } else {
        flag = 1;

        tree->root = left;
        left->parent = 0;

        left->right = node;
        node->parent = left;

        node->left = left_r;
        if (left_r)
            left_r->parent = node;
    }
    node->left_height = node->left ? node_height(node->left) + 1 : 1;
    left->right_height = node_height(node) + 1;
    if (!flag) {
        if (left->parent->left == left) {
            left->parent->left_height = node_height(left) + 1;
        } else {
            left->parent->right_height = node_height(left) + 1;
        }
    }
    return left;
}

node_t* node_construct(tree_t* tree, node_t* p, long key, const void* val)
{
    int element_size = tree->element_size;
    node_t* node = (node_t*)malc(sizeof(node_t));

    node->left = 0;
    node->right = 0;
    node->left_height = node->right_height = 1;

    node->parent = p;
    node->key = key;

    tree->size++;

    if (val) {
        node->val = malc(element_size);
        memcpy(node->val, val, element_size);
    }
    return node;
}

/* dtor of node who has parent and has no children, if no pa, it is root */
void node_deconstruct(tree_t* tree, node_t* node)
{
    tree->size--;
    dalc(node->val);
    dalc(node);
}

tree_t tree_construct(int element_size)
{
    tree_t tree;
    tree.element_size = element_size;

    tree.root = 0;
    tree.size = 0;

    return tree;
}

node_t* _tree_find_parent(node_t* node, long key)
{
    static node_t* prev_iterate_node = 0;
    if (!node) {
        node_t* ret = prev_iterate_node;
        prev_iterate_node = 0;
        return ret;
    }

    if (key < node->key) {
        prev_iterate_node = node;
        return _tree_find_parent(node->left, key);
    } else if (key > node->key) {
        prev_iterate_node = node;
        return _tree_find_parent(node->right, key);
    } else {
        node_t* ret = prev_iterate_node;
        prev_iterate_node = 0;
        return ret;
    }
    return node;
}

node_t* tree_find(node_t* node, long key)
{
    if (!node)
        return 0;
    if (key < node->key) {
        return tree_find(node->left, key);
    } else if (key > node->key) {
        return tree_find(node->right, key);
    }
    return node;
}

int tree_get(const tree_t* tree, long key, void* dst)
{
    node_t* node = tree_find((node_t*)tree->root, key);
    if (node) {
        memcpy(dst, node->val, tree->element_size);
        return 1;
    }
    return 0;
}

node_t* adjust_maybe(tree_t* tree, node_t* node)
{
    if (dec_hei(node) <= -2) {
        return left_balance(tree, node);
    } else if (dec_hei(node) >= 2) {
        return right_balance(tree, node);
    }
    return node;
}

int _tree_intsert(tree_t* tree, node_t* node, long key, const void* src)
{
    if (!tree->root) {
        tree->root = node_construct(tree, 0, key, src);
        return 1;
    }
    if (key < node->key) {
        if (!node->left) {
            node->left = node_construct(tree, node, key, src);
            node->left_height = 2;
            return node_height(node);
        } else {
            node->left_height = _tree_intsert(tree, node->left, key, src) + 1;
            node_t* new_this_node = adjust_maybe(tree, node);
            return node_height(new_this_node);
        }
    } else if (key > node->key) {
        if (!node->right) {
            node->right = node_construct(tree, node, key, src);
            node->right_height = 2;
            return node_height(node);
        } else {
            node->right_height = _tree_intsert(tree, node->right, key, src) + 1;
            node_t* new_this_node = adjust_maybe(tree, node);
            return node_height(new_this_node);
        }
    }
    return node_height(node);
}

void tree_insert(tree_t* tree, long key, const void* src)
{
    _tree_intsert(tree, (node_t*)tree->root, key, src);
}

int _tree_erase(tree_t* tree, node_t* node, long key)
{
    if (!node) {
        return 0;
    }
    if (key < node->key) {
        node->left_height = _tree_erase(tree, node->left, key) + 1;
        node_t* new_this_node = adjust_maybe(tree, node);
        return node_height(new_this_node);
    } else if (key > node->key) {
        node->right_height = _tree_erase(tree, node->right, key) + 1;
        node_t* new_this_node = adjust_maybe(tree, node);
        return node_height(new_this_node);
    }

    if (!node->left && !node->right) {
        node_t* pa = node->parent;
        if (pa) {
            if (pa->left == node) {
                pa->left = 0;
            } else {
                pa->right = 0;
            }
        } else {
            tree->root = 0;
        }
        node_deconstruct(tree, node);
        return 0;
    } else if (node->left && node->right) {
        node_t* q = node->left;
        while (q->right)
            q = q->right;
        node->key = q->key;
        memmove(node->val, q->val, tree->element_size);
        node->left_height = _tree_erase(tree, node->left, q->key) + 1;
        node_t* new_this_node = adjust_maybe(tree, node);
        return node_height(new_this_node);
    } else if (node->left) {
        node_t* q = node->left;
        node->key = q->key;
        memmove(node->val, q->val, tree->element_size);

        node->left = q->left;
        node->right = q->right;
        if (q->left) {
            q->left->parent = node;
        }
        if (q->right) {
            q->right->parent = node;
        }
        node->left_height = q->left_height;
        node->right_height = q->right_height;

        q->left = q->right = q->parent = 0;
        node_deconstruct(tree, q);

        return node_height(node);
    } else {
        node_t* q = node->right;
        node->key = q->key;
        memmove(node->val, q->val, tree->element_size);

        node->left = q->left;
        node->right = q->right;
        if (q->left) {
            q->left->parent = node;
        }
        if (q->right) {
            q->right->parent = node;
        }
        node->left_height = q->left_height;
        node->right_height = q->right_height;

        q->left = q->right = q->parent = 0;
        node_deconstruct(tree, q);

        return node_height(node);
    }
}

void tree_erase(tree_t* tree, long key)
{
    _tree_erase(tree, (node_t*)tree->root, key);
}

#ifdef DEBUG
void tree_debug_loop(node_t* node, int* iter)
{
    if (node) {
        // debug_node(node);
        (*iter)++;
        assert_node(node);
    } else {
        return;
    }
    tree_debug_loop(node->left, iter);
    tree_debug_loop(node->right, iter);
}
void tree_debug(tree_t* tree)
{
    static int iter = 0;
    iter = 0;
    tree_debug_loop(tree->root, &iter);
    printf("%d\n", iter);
    assert(iter == tree_size(tree));
}
#endif
int tree_size(const tree_t* tree)
{
    return tree->size;
}

void tree_destruct(tree_t* tree)
{
    /* TODO */
}