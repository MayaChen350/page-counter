//
// Created by mayachen on 2025-11-22.
//

#include "page_counter.h"
#include "include/ttf.h"
#include <math.h>
#include <stdio.h>
#include <wchar.h>

#include <stdlib.h>

#include "utils.h"

signed short getHHEALineGap(const char *ttf_filename);

static size_pt unitToPt(const double width, const size_pt font_size,
                        const double unit_per_em) {
  return (width * font_size) / unit_per_em;
}

static size_em getLineHeight(const size_pt font_size, const size_em font_ascent,
                             const size_em font_descent, const size_pt line_gap,
                             const double unit_per_em,
                             const double line_spacing) {
  const double factor =
      ((font_ascent + fabs(font_descent) + line_gap) / unit_per_em);
#ifdef DEBUG
  printf("Factor: %f\n", factor);
#endif
  return font_size * factor * line_spacing;
}

// static size_pt getLineHeight(const size_pt font_size,
//                              const double line_spacing) {
//     return font_size` * 1.2 * line_spacing;
// }

static size_pt
getHeightExtra(const size_pt extra_height, // text margin from top/bottom
               const size_pt page_margin, const size_pt line_height) {
  if (extra_height <= page_margin)
    return page_margin;
  else if (line_height >= extra_height)
    return page_margin + extra_height +
           (line_height > extra_height ? line_height : 0);
  else
    return line_height + extra_height;
}

static size_pt getMaxPageContentHeight(const size_pt page_height,
                                       const size_pt margin_top,
                                       const size_pt margin_bottom,
                                       const size_pt header_from_Top,
                                       const size_pt footer_from_bottom,
                                       const size_pt line_height) {
  const size_pt height_header =
      getHeightExtra(header_from_Top, margin_top, line_height);
  const size_pt height_footer =
      getHeightExtra(footer_from_bottom, margin_bottom, line_height);

  return page_height - height_header - height_footer;
}

int getPageCount(const char *ttf_filename, const size_in page_height,
                 const size_in page_width, const size_in margin_top,
                 const size_in margin_bottom, const size_in margin_left,
                 const size_in margin_right, const size_pt font_size,
                 const size_in paragraph_spacing_before,
                 const size_in paragraph_spacing_after,
                 const size_in header_from_top,
                 const size_in footer_from_bottom,
                 const double user_line_spacing, const char *filename) {
  const size_pt max_page_content_width =
      inchToPt(page_width - (margin_left + margin_right));
  const size_pt paragraph_spacing =
      inchToPt(paragraph_spacing_before + paragraph_spacing_after);

  ttf_t *const font_file = ttfCreate(ttf_filename, 0, throw_err, NULL);
  const double upm = ttfGetUPM(font_file);

  const size_pt line_height = getLineHeight(
      font_size, ttfGetAscent(font_file), ttfGetDescent(font_file),
      getHHEALineGap(ttf_filename), upm, user_line_spacing);

  const size_pt line_gap = (user_line_spacing - 1) *
                           line_height; // i have no idea what is that anymore
                                        // but this is probably wrong

  const size_pt max_page_content_height = getMaxPageContentHeight(
      inchToPt(page_height), inchToPt(margin_top), inchToPt(margin_bottom),
      inchToPt(header_from_top), inchToPt(footer_from_bottom), line_height);

#ifdef DEBUG
  printf("Unit per em %f\n", upm);
  printf("Paragraph spacing: %f\n", paragraph_spacing);
  printf("Max page content height: %f\n", max_page_content_height);
  printf("Max page content width: %f\n", max_page_content_width);
  printf("Line height: %f\n", line_height);
#endif

  FILE *const file = fopen(filename, "r");

  int pageCount = 1;
  int para_line_count = 1;
  size_pt curr_page_height = line_height; // start with line 1
  size_pt line_curr_width = 0;
  size_pt curr_word_width = 0;
  wchar_t last_char;
  while ((last_char = fgetwc(file)) != WEOF) {
#ifdef DEBUG
    printf("%lc", last_char);
#endif

    size_pt alt_curr_page_height = curr_page_height;
    bool first_and_last_para = false;
    bool was_newline = false;
#ifdef _WIN64
    if (last_char == '\r') {
      // skip the `\n` character to do the same thing
      fgetc(file); // now `\n`
#else
    if (last_char == '\n') {
#endif
#ifdef DEBUG
      printf("New line!");
#endif
      para_line_count = 1;
      curr_word_width = 0;
      line_curr_width = 0;
      was_newline = true;
      first_and_last_para = false;
      goto Increase_line;
    }

    const size_pt char_width =
        unitToPt(ttfGetWidth(font_file, last_char), font_size, upm);

    if (last_char == ' ' || last_char == '\r' || last_char == '\t')
      curr_word_width = 0;
    else
      curr_word_width += char_width;

    // wrap line if needed
    line_curr_width += char_width;
    if (line_curr_width >= max_page_content_width /*UNSURE*/) {
      line_curr_width = curr_word_width;
      para_line_count++;
#ifdef DEBUG
      printf("New paragraph line: Line count: %i\n", para_line_count);
#endif
    Increase_line:
      if (was_newline) {
        curr_page_height += paragraph_spacing;
      }

      if (first_and_last_para && para_line_count == 4) {
#ifdef DEBUG
        printf("The line count of the first paragraph was above 4 and it got "
               "trimmed to 2.\n");
#endif
        curr_page_height = 2 * (line_height);
        continue;
      } else
        curr_page_height +=
            line_height; // add that the content now takes a new line
#ifdef DEBUG
      printf(" Current page height: %f/%f\n", curr_page_height,
             max_page_content_height);
#endif

      // the line spacing of the last line of a page doesn't count
      alt_curr_page_height += font_size;
      if (curr_page_height >= max_page_content_height) {
#ifdef DEBUG
        printf("New page! Page Height was: %f\n", curr_page_height);
#endif
        pageCount++;
        if (was_newline) {
          curr_page_height = line_height; // first line, new page
        } else {
          first_and_last_para = true;
          if (para_line_count > 3) {
#ifdef DEBUG
            printf(
                "The lines of the last paragraph was added to the new page.\n");
#endif
            curr_page_height = 2 * (line_height);
          } else
            curr_page_height = para_line_count * (line_height);
        }
      }

      was_newline = false;
    }
  }
#ifndef NDEBUG
  //   printf("Last page content height: %f\n", curr_page_height);
#endif

  fclose(file);
  ttfDelete(font_file);

  return pageCount;
}

#define HHEA_LINE_GAP_BYTES_OFFSET 8
#define FWORD_SIZE_BYTES (16 / 8)

signed short getHHEALineGap(const char *ttf_filename) {
  FILE *fp = fopen(ttf_filename, "r");
  signed short result;

  int state = 0; // MUST stop before 4
  while (state < 4) {
    const char last_char = (char)fgetc(fp);

    if (last_char == EOF) {
      THROW("The file structure was incorrect");
    }
    const char tag[4] = "hhea";

    state = last_char == tag[state] ? state + 1 : 0;
  }

  fread(&state, HHEA_LINE_GAP_BYTES_OFFSET, 1,
        fp); // why not reusing state to discard data

  // now we have the actual line gap data
  fread(&result, FWORD_SIZE_BYTES, 1, fp);

  fclose(fp);
#ifdef DEBUG
  printf("Line gap: %i\n", result);
#endif
  return result;
}
