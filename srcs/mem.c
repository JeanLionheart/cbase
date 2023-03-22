#include "array.h"
#include <malloc.h>
#include <mem.h>
#include <stddef.h>
#include <threads.h>
#include <tree.h>

#ifdef debugging
#include <assert.h>
#include <stdio.h>
#define assert_cond(cond, expr) \
    if (!(cond)) {              \
        expr;                   \
        assert(0);              \
    };
#endif

#define DEFAULT_CAP 20

/*
the base level pool;
only construct, alloc, dealloc, and deconstruct;
 */

typedef struct mem_paragraph_s mem_paragraph_t;

typedef struct lst_s lst_t;

struct lst_s {
    lst_t* next;
};

struct mem_paragraph_s {
    int element_size;
    int remain_size;
    int capacity;
    lst_t* free_chunk;
    void *beg, *end;
};

mem_paragraph_t mempar_construct(const int element_size)
{
    mem_paragraph_t m;
    m.element_size = element_size;
    m.remain_size = DEFAULT_CAP;
    m.capacity = DEFAULT_CAP;

    int real_size = element_size + sizeof(int); // the last two byte to save the sizeof obj;
    void* data = malloc((real_size)*DEFAULT_CAP);

    m.beg = m.free_chunk = (lst_t*)data;
    m.end = ((char*)data + DEFAULT_CAP * real_size);
    if (data == NULL) {
        return m;
    }

    lst_t* ptr = (lst_t*)((char*)m.free_chunk + sizeof(int));
    m.free_chunk = ptr;
    for (int i = 0; i < DEFAULT_CAP - 1; i++) {
        int* siz = (int*)((char*)(ptr) - sizeof(int));
        *siz = (int)element_size;

        ptr->next = (lst_t*)((char*)ptr + real_size);
        ptr = ptr->next;
    }
    int* siz = (int*)((char*)(ptr) - sizeof(int));
    *siz = (int)element_size;
    ptr->next = NULL;

#ifdef debugging
    printf("initialize size:%d\n", m.element_size);
    ptr = m.free_chunk;
    for (int i = 0; i < DEFAULT_CAP; i++) {
        assert((void*)ptr < m.end && (void*)ptr >= m.beg);
        ptr = ptr->next;
    }
#endif

    return m;
}

void* mempar_alloc(mem_paragraph_t* m)
{
    if (!m->remain_size) {
        return 0;
    }
#ifdef debugging
    static int alc_time = 0;
    alc_time++;
    if (m->remain_size <= 0) {
        printf("rmsiz:%d\n", m->remain_size);
        assert(0);
    }
    assert(m->remain_size > 0);
    assert_cond(m->remain_size > 0, printf("m->re_siz:%d\n", m->remain_size));
    assert_cond(((void*)m->free_chunk->next >= m->beg
                    && (void*)m->free_chunk->next < m->end)
            || (m->free_chunk->next == 0),
        printf("\nm->beg:%p\nm->end:%p\nm->next:%p\n\n",m->beg,m->end,m->free_chunk->next));
#endif
    void* ret = m->free_chunk;
    m->remain_size--;
    m->free_chunk = m->free_chunk->next;
#ifdef debugging
    lst_t* ptr = m->free_chunk;
    int j;
    for (j = 0; ptr; j++) {
        assert((void*)ptr < m->end && (void*)ptr >= m->beg);
        ptr = ptr->next;
    }
    assert(j == m->remain_size);
#endif
    return ret;
}

int mempar_dealloc(mem_paragraph_t* m, void* mem)
{
#ifdef debugging
    // printf("dalc\n");
#endif
    if (mem < m->beg || mem >= m->end) {
        return 0;
    }
    lst_t* ptr = (lst_t*)mem;
    ptr->next = m->free_chunk;
    m->free_chunk = ptr;
    m->remain_size++;
    return 1;
}

void mempar_deconstruct(mem_paragraph_t* m)
{
    free(m->beg);
}

/* for debug */
#ifdef debugging
void mempar_test()
{
    mem_paragraph_t mp = mempar_construct(8);
    int* p[DEFAULT_CAP];
    for (int j = 0; j < 100; j++) {
        for (int i = 0; i < DEFAULT_CAP; i++) {
            p[i] = (int*)mempar_alloc(&mp);
        }
        for (int i = 0; i < DEFAULT_CAP; i++) {
            *(p[i]) = 1;
        }
        for (int i = 0; i < DEFAULT_CAP; i++) {
            mempar_dealloc(&mp, p[i]);
        }
    }
    mempar_deconstruct(&mp);
}
#endif
/* end for debug */

/*
the second level pool_array,
for a single size.
behave like a infinite pool.
*/

typedef struct mem_array_s mem_array_t;

struct mem_array_s {
    array_t par_array;
    int alloc_element_size;
};

mem_array_t memarr_construct(int element_size)
{
    mem_array_t ma;
    ma.alloc_element_size = element_size;
    ma.par_array = array_construct(sizeof(mem_paragraph_t));
    mem_paragraph_t mp = mempar_construct(element_size);
    array_append(&ma.par_array, &mp);
    return ma;
}

void* memarr_alloc(mem_array_t* ma)
{
    mem_paragraph_t* data = (mem_paragraph_t*)array_data(&ma->par_array);
    for (int i = 0; i < array_size(&ma->par_array); i++) {
        void* ret = mempar_alloc(&data[i]);
        if (ret) {
            return ret;
        }
    }
    mem_paragraph_t mp = mempar_construct(ma->alloc_element_size);
    if (mp.free_chunk == NULL)
        return NULL;
    array_append(&ma->par_array, &mp);
    return memarr_alloc(ma);
}

int memarr_dealloc(mem_array_t* ma, void* mem)
{
    mem_paragraph_t* data = (mem_paragraph_t*)array_data(&ma->par_array);
    for (int i = 0; i < array_size(&ma->par_array); i++) {
        int ret = mempar_dealloc(&data[i], mem);
        if (ret)
            return 1;
    }
    return 0;
}

void memarr_deconstruct(mem_array_t* ma)
{
    mem_paragraph_t* data = (mem_paragraph_t*)array_data(&ma->par_array);
    for (int i = 0; i < array_size(&ma->par_array); i++) {
        mempar_deconstruct(&data[i]);
    }
    array_deconstruct(&ma->par_array);
}

/* for debug */
#ifdef debugging
void memarr_test()
{
    mem_array_t ma = memarr_construct(80);
    int* p[DEFAULT_CAP * 10];
    int l = DEFAULT_CAP * 10; // 200
    for (int j = 0; j < 100; j++) {
        for (int i = 0; i < l; i++) {
            p[i] = (int*)memarr_alloc(&ma);
        }
        for (int i = 0; i < l; i++) {
            *(p[i]) = 1;
        }
        for (int i = 0; i < l; i++) {
            memarr_dealloc(&ma, p[i]);
        }
    }
    assert(ma.par_array.size == 10);
    memarr_deconstruct(&ma);
}
#endif

/* end for debug */

/*
the third level,
provide 16,32,48,64,128,256,512,1024,2048 sizes pool
behave like malloc/free
 */
static const int siz_arr[] = { 16, 32, 48, 64, 128, 256, 512, 1024, 2048 };

typedef struct mem_shell_s mem_shell_t;

struct mem_shell_s {
    mem_array_t marr[9];
};

mem_shell_t memshell_construct()
{
    mem_shell_t mshell;
    for (int i = 0; i < 9; i++) {
        mshell.marr[i] = memarr_construct(siz_arr[i]);
    }
    return mshell;
}

void* memshell_alloc(mem_shell_t* ms, int size)
{
    for (int i = 0; i < 9; i++) {
        if (siz_arr[i] >= size) {
            return memarr_alloc(&ms->marr[i]);
        }
    }
    return 0;
}

void memshell_dealloc(mem_shell_t* ms, void* mem)
{
    int* siz = (int*)((char*)mem - sizeof(int));
    for (int i = 0; i < 9; i++) {
        if (siz_arr[i] == *siz) {
            memarr_dealloc(&ms->marr[i], mem);
            return;
        }
    }
}

void memshell_deconstruct(mem_shell_t* ms)
{
    for (int i = 0; i < 9; i++) {
        memarr_deconstruct(&ms->marr[i]);
    }
}

/* interface level */

mem_t mem_cons();

void* m_alloc(mem_t* m, int size);

void m_dealloc(mem_t* m, void* mem);

void mem_decons(mem_t* m);

mem_t mem_cons()
{
    mem_t m;
    m.m_shell = malloc(sizeof(mem_shell_t));
    return m;
}

void* m_alloc(mem_t* m, int size)
{
    mem_shell_t* s;
    s = (mem_shell_t*)m->m_shell;
    return memshell_alloc(s, size);
}

void m_dealloc(mem_t* m, void* mem)
{
    mem_shell_t* s;
    s = (mem_shell_t*)m->m_shell;
    return memshell_dealloc(s, mem);
}

void mem_decons(mem_t* m)
{
    mem_shell_t* s;
    s = (mem_shell_t*)m->m_shell;
    return memshell_deconstruct(s);
}

/* for multi-thrd mem-alloc */

static thread_local mem_shell_t ms;
static thread_local int is_init = 0;

void* malc(int size);

void dalc(void* mem);

void mem_thrd_exit();

void* malc(int size)
{
    if (!is_init) {
        is_init = 1;
        ms = memshell_construct();
    }
    return memshell_alloc(&ms, size);
}

void dalc(void* mem)
{
    return memshell_dealloc(&ms, mem);
}

void mem_thrd_exit()
{
    is_init = 0;
    return memshell_deconstruct(&ms);
}

/* for debug */
#ifdef debugging
int alc_test(void* arg)
{
    int* idaddr = (int*)arg;
    int id = *idaddr;
    const int cap = 20000;
    int* parr[cap];
    for (int j = 0; j < 10; j++) {
        for (int i = 0; i < cap; i++) {
            int siz = i + 10 > 1024 ? 1024 : i + 10;
            parr[i] = malc(siz);
        }
        for (int i = 0; i < cap; i++) {
            *(parr[i]) = i + id;
        }
        for (int i = 0; i < cap; i++) {
            assert(*(parr[i]) == i + id);
        }
        for (int i = 0; i < cap; i++) {
            dalc(parr[i]);
        }
    }
    mem_thrd_exit();
    return 0;
}

void malc_test()
{
    const int cap = 10;
    thrd_t th[cap];
    int id[10] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    for (int i = 0; i < cap; i++) {
        thrd_create(&th[i], alc_test, &id[i]);
    }
    for (int i = 0; i < cap; i++) {
        thrd_join(th[i], 0);
    }
}
#endif