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
    size_pt font_size,
    float user_line_spacing,
    const char *filename
);

#endif //PAGE_COUNTER_PAGE_COUNTER_H
