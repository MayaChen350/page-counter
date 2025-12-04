//
// Created by mayachen on 2025-11-22.
//

#ifndef PAGE_COUNTER_UTILS_H
#define PAGE_COUNTER_UTILS_H

#include "include/ttf.h"

void throw_err(void *_, const char *msg);

#define THROW(msg) \
    fprintf(stderr, "ERROR: %sn", msg); \
    exit(1);

typedef float size_pt;
typedef float size_em;
typedef float size_in;

size_pt inchToPt(size_in size);

size_in milToInch(size_mil size);

// Strings

bool strEqual(const char *s1, const char *s2);

#endif //PAGE_COUNTER_UTILS_H
