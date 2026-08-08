//
// Created by mayachen on 2025-11-22.
//

#include "page_counter.h"
#include "include/ttf.h"
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <wchar.h>

#include <stdlib.h>

#include "utils.h"

signed short getHHEALineGap(const char *ttf_filename);

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

struct pagecounter {
  int page_count;

  const size_pt font_size;
  const size_pt paragraph_spacing;
  size_pt max_page_content_height;
  size_pt line_height;
  const double unit_per_em;

  size_pt page_curr_height;
  size_pt line_curr_width;
  size_pt word_curr_width;
  int para_line_count;

  bool first_and_last_para /*: 1*/;
};

typedef struct pagecounter pagecounter_t;

static size_pt unitToPt(const double size, const pagecounter_t *data) {
  return (size * data->font_size) / data->unit_per_em;
}

static void add_newline(pagecounter_t *this) {
#ifdef DEBUG
  printf("New line!");
#endif
  this->para_line_count = 1;
  this->word_curr_width = 0;
  this->line_curr_width = 0;
  this->first_and_last_para = false;

  this->page_curr_height += this->paragraph_spacing + this->line_height;

  if (this->page_curr_height >= this->max_page_content_height) {
#ifdef DEBUG
    printf("New page! Page Height was: %f\n", this->page_curr_height);
#endif
    this->page_count++;
    this->page_curr_height = this->line_height; // first line, new page
  }
}

static void wrap_line(pagecounter_t *this) {
  this->line_curr_width = this->word_curr_width;
  this->para_line_count++;
#ifdef DEBUG
  printf("New paragraph line: Line count: %i\n", this->para_line_count);
#endif

  if (this->first_and_last_para && this->para_line_count == 4) {
#ifdef DEBUG
    printf("The line count of the first paragraph was above 4 and it got "
           "trimmed to 2.\n");
#endif
    this->page_curr_height = 2 * (this->line_height);
    return;
  } else
    this->page_curr_height +=
        this->line_height; // add that the content now takes a new line
#ifdef DEBUG
  printf(" Current page height: %f/%f\n", this->page_curr_height,
         this->max_page_content_height);
#endif

  if (this->page_curr_height >= this->max_page_content_height) {
#ifdef DEBUG
    printf("New page! Page Height was: %f\n", this->page_curr_height);
#endif
    this->page_count++;

    this->first_and_last_para = true;
    if (this->para_line_count > 3) {
#ifdef DEBUG
      printf("The lines of the last paragraph was added to the new page.\n");
#endif
      this->page_curr_height = 2 * (this->line_height);
    } else
      this->page_curr_height = this->para_line_count * (this->line_height);
  }
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

int page_count(const char *filename, const char *ttf_filename,
               size_pt font_size, double user_line_spacing, size_in page_height,
               size_in page_width, size_in margin_top, size_in margin_bottom,
               size_in margin_left, size_in margin_right,
               size_in paragraph_spacing_before,
               size_in paragraph_spacing_after, size_in header_from_top,
               size_in footer_from_bottom) {
  const size_pt max_page_content_width =
      inchToPt(page_width - (margin_left + margin_right));

  ttf_t *const font_file = ttfCreate(ttf_filename, 0, throw_err, NULL);

  pagecounter_t data_holder = {
      .page_count = 1,

      .font_size = font_size,
      .paragraph_spacing =
          inchToPt(paragraph_spacing_before + paragraph_spacing_after),
      .unit_per_em = ttfGetUPM(font_file),

      .line_curr_width = 0,
      .word_curr_width = 0,
      .para_line_count = 1,

      .first_and_last_para = false};

  data_holder.max_page_content_height = getMaxPageContentHeight(
      inchToPt(page_height), inchToPt(margin_top), inchToPt(margin_bottom),
      inchToPt(header_from_top), inchToPt(footer_from_bottom),
      data_holder.line_height);
  data_holder.line_height = data_holder.page_curr_height = getLineHeight(
      font_size, ttfGetAscent(font_file), ttfGetDescent(font_file),
      getHHEALineGap(ttf_filename), data_holder.unit_per_em, user_line_spacing);

#ifdef DEBUG
  printf("Unit per em %f\n", data_holder.unit_per_em);
  printf("Paragraph spacing: %f\n", data_holder.paragraph_spacing);
  printf("Max page content height: %f\n", data_holder.max_page_content_height);
  printf("Max page content width: %f\n", max_page_content_width);
  printf("Line height: %f\n", data_holder.line_height);
#endif

  FILE *const file = filename == NULL ? stdin : fopen(filename, "r");

  wchar_t last_char;
  while ((last_char = fgetwc(file)) != WEOF) {
#ifdef DEBUG
    printf("%lc", last_char);
#endif

    bool first_and_last_para = false;
#ifdef _WIN64
    if (last_char == '\r') {
      // skip the `\n` character to do the same thing
      fgetc(file); // now `\n`
      add_newline(pagecounter_t * this);
      continue;
    }
#else
    if (last_char == '\n') {
      add_newline(&data_holder);
      continue;
    }
#endif

    const size_pt char_width =
        unitToPt(ttfGetWidth(font_file, last_char), &data_holder);

    if (last_char == ' ' || last_char == '\r' || last_char == '\t')
      data_holder.word_curr_width = 0;
    else
      data_holder.word_curr_width += char_width;

    // wrap line if needed
    data_holder.line_curr_width += char_width;
    if (data_holder.line_curr_width >=
        max_page_content_width /*UNSURE OF SIGN*/)
      wrap_line(&data_holder);
  }
#ifdef DEBUG
  printf("Last page content height: %f\n", data_holder.page_curr_height);
#endif

  fclose(file);
  ttfDelete(font_file);

  return data_holder.page_count;
}

#define HHEA_LINE_GAP_BYTES_OFFSET 8
#define FWORD_SIZE_BYTES (16 / 8)

signed short getHHEALineGap(const char *ttf_filename) {
  FILE *fp = fopen(ttf_filename, "rb");
  signed short result;

  int state = 0; // MUST stop before 4
  while (state < 4) {
    const int last_char = fgetc(fp);

    if (last_char == EOF) {
      THROW("The file structure was incorrect");
    }
    const char tag[4] = "hhea";

    state = (char)last_char == tag[state] ? state + 1 : 0;
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
