//
// Created by mayachen on 2025-11-22.
//

#ifndef PAGE_COUNTER_PAGE_COUNTER_H
#define PAGE_COUNTER_PAGE_COUNTER_H
#include "utils.h"

int getPageCount(
    const char *ttf_filename,

    size_in page_height,
    size_in page_width,

    size_in margin_top,
    size_in margin_bottom,
    size_in margin_left,
    size_in margin_right,

    size_pt font_size,

    size_in paragraph_spacing_before,
    size_in paragraph_spacing_after,

    float user_line_spacing,
    const char *filename
);

#endif //PAGE_COUNTER_PAGE_COUNTER_H
