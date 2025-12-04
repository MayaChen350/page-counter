//
// Created by mayachen on 2025-12-04.
//

#include "ttf_reader.h"

#include <stdio.h>
#include <stdlib.h>

#include "utils.h"

#define HEAD_UNIT_PER_EM_OFFSET 6
#define HHEA_ASCENDER_BYTES_OFFSET 4

void font_t_init(struct font_t *font, const char *ttf_filename) {
    FILE *fp = fopen(ttf_filename, "r");

    advance_file_until(fp, "head");
    fread(&(font->unitsPerEm), HEAD_UNIT_PER_EM_OFFSET, 1, fp); // junk
    fread(&(font->unitsPerEm), sizeof(font->unitsPerEm), 1, fp);

    advance_file_until(fp, "hhea");
    fread(&(font->ascender), HHEA_ASCENDER_BYTES_OFFSET, 1, fp); // junk
    fread(&(font->ascender), sizeof(font->ascender), 1, fp);
    fread(&(font->descender), sizeof(font->descender), 1, fp);
    fread(&(font->line_gap), sizeof(font->line_gap), 1, fp);

    fclose(fp);
#ifndef NDEBUG
    printf("Line gap: %i\n", font->line_gap);
#endif
}
