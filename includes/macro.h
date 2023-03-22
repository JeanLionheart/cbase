#ifndef __MACROH__
#define __MACROH__
#include<config.h>

#ifdef debugging
#include <assert.h>
#define todo(message) printf("to do:");printf(message);printf("\n");assert(0);
#endif

#define DEFAULT_BUFSIZ 2048
#define LONG_BUFSIZ 8192

#endif