//
// Created by mayachen on 2025-11-22.
//

#include "utils.h"

#include <stdio.h>
#include <stdlib.h>

void throw_err(void *_, const char *msg) {
    THROW(msg);
}

inline size_pt inchToPt(const size_in size) {
    return size * 72;
}

inline size_in milToInch(const size_mil size) {
    return (float) size / 1000;
}