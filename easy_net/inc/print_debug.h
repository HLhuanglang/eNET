#ifndef __EASYNET_print_debug_h
#define __EASYNET_print_debug_h

#include <cstdio>

#ifdef DEBUG
#    define printfd(fmt, args...) fprintf(stdout, fmt "\n", ##args)
#else
#    define printfd(fmt, args...)
#endif

#endif