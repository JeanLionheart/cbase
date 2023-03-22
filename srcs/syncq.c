#include <stdio.h>
#include <syncq.h>

void sync_que_init(sync_que_t *sq, int element_size);

void sync_que_push(sync_que_t *sq, const void *src);

void sync_que_pop(sync_que_t *sq, void *dst);

void sync_que_deconstruct(sync_que_t *sq);

void sync_que_init(sync_que_t *sq, int element_size)
{
    sq->list = list_construct(element_size);
    pthread_mutex_init(&sq->lock, 0);
    pthread_cond_init(&sq->condv, 0);
}

void sync_que_push(sync_que_t *sq, const void *src)
{
    pthread_mutex_lock(&sq->lock);
    list_append(&sq->list, src);
    pthread_cond_signal(&sq->condv);
    pthread_mutex_unlock(&sq->lock);
}

void sync_que_pop(sync_que_t *sq, void *dst)
{
    pthread_mutex_lock(&sq->lock);
    while (list_empty(&sq->list))
    {
        pthread_cond_wait(&sq->condv, &sq->lock);
    }
    // list_get_begin()
    void *node = list_get_begin(&sq->list);
    list_node_get_value(&sq->list, node, dst);
    list_node_erase(&sq->list, node);

    pthread_mutex_unlock(&sq->lock);
}

int sync_que_try_pop(sync_que_t *sq, void *dst)
{
    pthread_mutex_lock(&sq->lock);

    if (list_empty(&sq->list))
    {
        pthread_mutex_unlock(&sq->lock);
        return 0;
    }

    void *node = list_get_begin(&sq->list);
    list_node_get_value(&sq->list, node, dst);
    list_node_erase(&sq->list, node);

    pthread_mutex_unlock(&sq->lock);

    return 1;
}

void sync_que_deconstruct(sync_que_t *sq)
{
    pthread_mutex_destroy(&sq->lock);
    pthread_cond_destroy(&sq->condv);
    list_deconstruct(&sq->list);
}