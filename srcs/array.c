#include <array.h>
#include <malloc.h>
#include <string.h>

array_t array_construct(int element_size);

void array_erase(array_t *arr, int pos);

int array_append(array_t *arr, const void *src);

void array_resize(array_t *arr, int size);

int array_element_size(const array_t *arr);

int array_size(const array_t *arr);

void *array_data(const array_t *arr);

void array_clean(array_t *arr);

void array_destruct(array_t *arr);

/* private part */
void array_expand(array_t *arr, int cap);

array_t array_construct(int element_size)
{
    array_t arr;
    arr.element_size = element_size;
    arr.cap = 0;
    arr.size = 0;
    return arr;
}

void array_erase(array_t *arr, int pos)
{
    void *dst = (char *)arr->data + arr->element_size * pos;
    void *src = (char *)arr->data + arr->element_size * (arr->size - 1);
    memmove(dst, src, arr->element_size);
    arr->size--;
}

int array_append(array_t *arr, const void *src)
{
    if (arr->cap <= arr->size + 1) array_expand(arr, arr->cap * 2);
    void *dst = (char *)arr->data + arr->element_size * arr->size;
    memmove(dst, src, arr->element_size);
    arr->size++;
    return arr->size - 1;
}

void array_resize(array_t *arr, int size)
{
    if (arr->cap > size)
    {
        arr->size = size;
        return;
    }
    else
    {
        array_expand(arr, size);
        arr->size = size;
    }
}

int array_size(const array_t *arr)
{
    return arr->size;
}

void *array_data(const array_t *arr)
{
    return arr->data;
}

void array_clean(array_t *arr)
{
    arr->size = 0;
    return;
}

void array_destruct(array_t *arr)
{
    free(arr->data);
}

void array_expand(array_t *arr, int cap)
{
    cap += 5;
    if (cap < arr->size)
    {
        cap = arr->size * 2 + 5;
    }

    void *new_arr = malloc(cap * arr->element_size);

    if (arr->size)
    {
        int len = arr->size * arr->element_size;
        memcpy(new_arr, arr->data, len);
    }

    if (arr->cap) free(arr->data);
    arr->data = new_arr;
    arr->cap = cap;
}