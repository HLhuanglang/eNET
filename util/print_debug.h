#ifndef __print_debug_h
#define __print_debug_h

#include <cstdio>

#ifdef DEBUG
#define printfd(fmt, args...) fprintf(stdout, fmt "\n", ##args)
#else
#define printfd(fmt, args...)
#endif

#endif