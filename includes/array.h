#ifndef __ARRAYH__
#define __ARRAYH__

/* use byts as arr */
typedef struct array_s array_t;
struct array_s
{
    /* data */
    void *data;
    int element_size;
    int cap;
    int size;
};

array_t array_construct(int element_size);

void array_erase(array_t *arr, int pos);

int array_append(array_t *arr, const void *src);

void array_pop_back(array_t *arr, void *dst);

void array_resize(array_t *arr, int size);

void array_expand(array_t *arr, int cap);

int array_size(const array_t *arr);

void *array_data(const array_t *arr);

void array_clean(array_t *arr);

void array_destruct(array_t *arr);

#endif