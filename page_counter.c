//
// Created by mayachen on 2025-11-22.
//

#include <stdio.h>
#include <math.h>
#include "include/ttf.h"
#include "page_counter.h"

#include <stdlib.h>

#include "utils.h"

signed short getLineGap(const char *ttf_filename);

static int getMaxLinesPerPage(size_pt page_height, size_pt line_height);

static size_pt getSizeByCharSize(const size_em width, const size_pt font_size) {
    return ((float) width / 1000) * font_size;
}

static size_em getLineHeight(const size_mil font_ascent,
                             const size_mil font_descent,
                             const size_em line_gap,
                             const float line_spacing) {
    return line_spacing * (float) (font_ascent + abs(font_descent) /*+ line_gap*/);
}

int getPageCount(
    const char *ttf_filename,
    size_in page_height,
    size_in page_width,
    const size_in margin_top,
    const size_in margin_bottom,
    const size_in margin_left,
    const size_in margin_right,
    const size_pt font_size,
    const float user_line_spacing,
    const char *filename
) {
    const size_em line_gap = getLineGap(ttf_filename);

    ttf_t *const font_file = ttfCreate(ttf_filename, 0, throw_err, NULL);

    page_height = inchToPt(page_height - (margin_top + margin_bottom));
    page_width = inchToPt(page_width - (margin_left + margin_right));
#ifndef NDEBUG
    printf("Page height: %f\n", page_height);
    printf("Page width: %f\n", page_width);
#endif
    const size_pt line_height = getSizeByCharSize(
        getLineHeight(
            ttfGetAscent(font_file),
            ttfGetDescent(font_file),
            line_gap,
            user_line_spacing), font_size);

    const int max_lines_per_page = getMaxLinesPerPage(page_height, line_height);

    FILE *const file = fopen(filename, "r");

    int pageCount = 0;
    int page_curr_lines = 0;
    size_pt line_curr_width = 0;
    size_pt curr_word_width = 0;
    char last_char;
    while ((last_char = (char) fgetc(file)) != EOF) {
        bool was_newline = false;
        if (last_char == '\n') {
            curr_word_width = 0;
            was_newline = true;
            goto Increase_line;
        }

        const size_pt char_width = getSizeByCharSize((float) ttfGetWidth(font_file, last_char), font_size);
        // #ifndef NDEBUG
        //         printf("Char width of %c: %f\n", last_char, char_width);
        // #endif
        curr_word_width = last_char == ' ' ? 0 : curr_word_width + char_width;

        // wrap line if needed
        line_curr_width += char_width;
        if (line_curr_width >= page_width /*UNSURE*/) {
        Increase_line:
            page_curr_lines++;
            line_curr_width = curr_word_width;
            if (page_curr_lines > max_lines_per_page) {
                pageCount++;
                page_curr_lines = 0;
            }
            if (was_newline) {
                was_newline = false;
                goto Increase_line;
            } // increase line TWICE if it was \n
        }
    }

    fclose(file);
    ttfDelete(font_file);

    return pageCount;
}

#define HHEA_LINE_GAP_BYTES_OFFSET 8
#define FWORD_SIZE_BYTES (16 / 8)

signed short getLineGap(const char *ttf_filename) {
    FILE *fp = fopen(ttf_filename, "r");
    signed short result;

    int state = 0; // stop at 4
    while (state < 4) {
        const char last_char = (char) fgetc(fp);

        if (last_char == EOF) {
            THROW("The file structure was incorrect");
        }

        if (state == 0 && last_char == 'h')
            state++;
        else if (state == 1)
            if (last_char == 'h')
                state++;
            else
                state = 0;
        else if (state == 2)
            if (last_char == 'e')
                state++;
            else
                state = 0;
        else if (state == 3)
            if (last_char == 'a')
                state++;
            else
                state = 0;
        else {
        }
    }

    fread(&state, HHEA_LINE_GAP_BYTES_OFFSET, 1, fp); // why not reusing state to discard data

    // now that
    fread(&result, FWORD_SIZE_BYTES, 1, fp);

    fclose(fp);
#ifndef NDEBUG
    printf("Line gap: %i\n", result);
#endif
    return result;
}

int getMaxLinesPerPage(const size_pt page_height, const size_pt line_height) {
    const int result = (int) floorf(page_height / line_height);
#ifndef NDEBUG
    printf("Line height: %f\n", line_height);
    printf("Max line per page: %i\n", result);
#endif
    return result;
}
