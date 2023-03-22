#ifndef __BASE__
#define __BASE__
#include <array.h>
#include <list.h>
#include <tree.h>
#include<syncq.h>
#define MAX_BUFSIZ 4096

#ifdef debugging
#define debugging
#include <assert.h>
#include <base.h>
#include <config.h>
#include <mem.h>
#include <pthread.h>
#include <string.h>

#define for_loop for (int i = 0; i < 50000; i++)

#define for_loop2 for (long i = 0; i < 5000; i++)


void tree_test()
{
    tree_t t = tree_construct(8);
    for_loop2
    {
        tree_insert(&t, i, &i);
    }
    for_loop2
    {
        long v;
        tree_get(&t, i, &v);
        assert(v == i);
    }
    for_loop2
    {
        tree_erase(&t, i);
    }
}

void array_test()
{
    array_t arr = array_construct(sizeof(int));
    for_loop
    {
        array_append(&arr, &i);
    }
    for (int i = 1024; i < 50000; i++) {
        array_erase(&arr, i);
    }
    assert(array_size(&arr) == 1024);
    for (int i = 0; i < 1024; i++) {
        int* data = (int*)arr.data;
        assert(data[i] == i);
    }
    array_deconstruct(&arr);
}

int test_arr[0xffffff];
pthread_mutex_t mu;

void* qpush(void* arg)
{
    sync_que_t* q = (sync_que_t*)arg;
    for_loop
    {
        sync_que_push(q, &i);
    }
    return 0;
}

void* qpop(void* arg)
{
    sync_que_t* q = (sync_que_t*)arg;
    for_loop
    {
        int v;
        sync_que_pop(q, &v);
        pthread_mutex_lock(&mu);
        test_arr[v]++;
        pthread_mutex_unlock(&mu);
    }
    return 0;
}

void syncq_test()
{
    sync_que_t q;
    sync_que_init(&q, sizeof(int));
    pthread_t pthrd[20];

    pthread_mutex_init(&mu, 0);

    for (int i = 0; i < 2; i++) {
        pthread_create(&pthrd[i], 0, qpush, &q);
        pthread_create(&pthrd[i + 2], 0, qpop, &q);
    }
    for (int i = 0; i < 4; i++) {
        pthread_join(pthrd[i], 0);
    }

    sync_que_deconstruct(&q);
}

#undef for_loop
#endif

#endif
