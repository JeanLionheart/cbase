#ifndef __MEMH__
#define __MEMH__

#include <config.h>

typedef struct mem_s mem_t;

struct mem_s {
    void* m_shell;
};

mem_t mem_cons();

void* m_alloc(mem_t* m, int size);

void m_dealloc(mem_t* m, void* mem);

void mem_des(mem_t* m);

void* malc(int size);

void dalc(void*mem);

void mem_thrd_exit();

#ifdef debugging
void mempar_test();
void memarr_test();
void malc_test();
#endif

#endif