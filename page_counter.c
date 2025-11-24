//
// Created by mayachen on 2025-11-22.
//

#include <stdio.h>
#include <math.h>
#include "include/ttf.h"
#include "page_counter.h"
#include "utils.h"

signed short getLineGap(const char *ttf_filename);

static int getMaxLinesPerPage(size_pt page_height, size_pt line_height);

static inline size_pt getSizeByCharSize(const size_pt font_size, const size_em width) {
    return ((float) width / 1000) * font_size;
}

static inline size_em getLineHeight(const int font_ascent,
                                    const int font_descent,
                                    const int line_gap,
                                    const float line_spacing) {
    return line_spacing * (float) (font_ascent + font_descent + line_gap);
}

int getPageCount(
    const char *ttf_filename,
    size_in page_height,
    size_in page_width,
    size_in margin_top,
    size_in margin_bottom,
    size_in margin_left,
    size_in margin_right,
    const size_pt font_size,
    const float user_line_spacing,
    const char *filename
) {
    const int line_gap = getLineGap(ttf_filename);

    ttf_t *font_file = ttfCreate(ttf_filename, 0, throw_err, NULL);

    page_height = getSizeByCharSize(font_size, page_height);
    page_width = getSizeByCharSize(font_size, page_width);
    const size_pt line_height = getSizeByCharSize(
        getLineHeight(
            ttfGetAscent(font_file),
            ttfGetDescent(font_file),
            line_gap,
            user_line_spacing), font_size);

    const int max_lines_per_page = getMaxLinesPerPage(page_height, line_height);

    FILE *file = fopen(filename, "r");

    int pageCount = 0;
    int page_curr_lines = 0;
    size_pt line_curr_width = 0;
    char last_char;
    while ((last_char = (char) fgetc(file)) != EOF) {
        // new line
        if (last_char == '\n') {
            page_curr_lines++;
            if (page_curr_lines > max_lines_per_page) {
                goto Increase_page;
            }
            continue;
        }

        // wrap line if needed
        line_curr_width += getSizeByCharSize((float) ttfGetWidth(font_file, last_char), font_size);
        if (line_curr_width < page_width /*UNSURE*/) {
            continue;
        }

    Increase_page: {
            pageCount++;
            page_curr_lines = 0;
            line_curr_width = 0;
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

    // ReSharper disable once CppJoinDeclarationAndAssignment
    char last_char;
    int state = 0; // stop at 4
    while (state < 4) {
        if (fgetc(fp) == EOF) {
            THROW("The file structure was incorrect");
        }

        last_char = (char) fgetc(fp);

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
    return result;
}

int getMaxLinesPerPage(const size_pt page_height, const size_pt line_height) {
    return (int) floorf(page_height / line_height);
}
