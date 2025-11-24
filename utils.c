//
// Created by mayachen on 2025-11-22.
//

#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void throw_err(void *_, const char *msg) {
    THROW(msg);
}

inline size_pt inchToPt(const size_in size) {
    return size * 72;
}

inline size_in milToInch(const size_mil size) {
    return (float) size / 1000;
}

inline bool strEqual(const char *s1, const char *s2) {
    return strcmp(s1, s2) == 0;
}
