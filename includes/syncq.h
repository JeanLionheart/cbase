#ifndef __LISTQ__
#define __LISTQ__

#include <list.h>
#include <pthread.h>

typedef struct sync_que_s sync_que_t;

struct sync_que_s
{
    /* data */
    list_t list;
    pthread_mutex_t lock;
    pthread_cond_t condv;
};

void sync_que_init(sync_que_t *sq, int element_size);

void sync_que_push(sync_que_t *sq, const void *src);

void sync_que_pop(sync_que_t *sq, void *dst);

int sync_que_try_pop(sync_que_t *sq, void *dst);

void sync_que_destruct(sync_que_t *sq);

#endif