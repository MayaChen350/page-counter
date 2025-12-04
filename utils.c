//
// Created by mayachen on 2025-11-22.
//

#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void advance_file_until(FILE* fp, const char* until) {
    int state = 0; // MUST stop before 4
    while (state < 4) {
        const char last_char = (char) fgetc(fp);

        if (last_char == EOF) {
            THROW("The file structure was incorrect");
        }

        if (last_char == until[state]) {
            state++;
        } else state = 0;
    }
}

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