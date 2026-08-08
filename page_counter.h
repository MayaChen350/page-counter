//
// Created by mayachen on 2025-11-22.
//

#ifndef PAGE_COUNTER_PAGE_COUNTER_H
#define PAGE_COUNTER_PAGE_COUNTER_H
#include "utils.h"

struct page_count_params_t {
  const char *filename;
  const char *ttf_filename;
  size_pt font_size;
  double user_line_spacing;
  size_in page_height;
  size_in page_width;
  size_in margin_top;
  size_in margin_bottom;
  size_in margin_left;
  size_in margin_right;
  size_in paragraph_spacing_before;
  size_in paragraph_spacing_after;
  size_in header_from_top;
  size_in footer_from_bottom;
};

int page_count(const char *filename, const char *ttf_filename,
               size_pt font_size, double user_line_spacing, size_in page_height,
               size_in page_width, size_in margin_top, size_in margin_bottom,
               size_in margin_left, size_in margin_right,
               size_in paragraph_spacing_before,
               size_in paragraph_spacing_after, size_in header_from_top,
               size_in footer_from_bottom);

inline int page_countt(struct page_count_params_t *p) {
  return page_count(
      p->filename, p->ttf_filename, p->font_size, p->user_line_spacing,
      p->page_height, p->page_width, p->margin_top, p->margin_bottom,
      p->margin_left, p->margin_right, p->paragraph_spacing_before,
      p->paragraph_spacing_after, p->header_from_top, p->footer_from_bottom);
}

#endif // PAGE_COUNTER_PAGE_COUNTER_H
