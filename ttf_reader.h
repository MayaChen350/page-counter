//
// Created by mayachen on 2025-12-04.
//

#ifndef PAGECOUNTER_TTF_READER_H
#define PAGECOUNTER_TTF_READER_H
typedef signed short FWORD;

struct font_t {
    // head
    unsigned short unitsPerEm;
    // hhea
    FWORD ascender;
    FWORD descender;
    FWORD line_gap;
};

void font_t_init(struct font_t *font, const char *ttf_filename);

#endif //PAGECOUNTER_TTF_READER_H
