//
// Created by mayachen on 2025-11-22.
//

#include <stdio.h>
#include <math.h>
#include "include/ttf.h"
#include "page_counter.h"

#include <stdlib.h>

#include "utils.h"

signed short getHHEALineGap(const char *ttf_filename);

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

inline static size_pt getHeightExtra(const size_pt extra_height,
                                     const size_pt margin,
                                     const size_pt line_height) {
    if (extra_height >= margin)
        return margin + extra_height + (line_height > extra_height ? line_height : 0);
    else if (line_height >= extra_height)
        return line_height + extra_height;
    else
        return extra_height;
}

static size_pt getMaxPageContentHeight(const size_pt page_height,
                                       const size_pt margin_top,
                                       const size_pt margin_bottom,
                                       const size_pt header_from_Top,
                                       const size_pt footer_from_bottom,
                                       const size_pt line_height
) {
    const size_pt height_header = getHeightExtra(header_from_Top, margin_top, line_height);
    const size_pt height_footer = getHeightExtra(footer_from_bottom, margin_bottom, line_height);

    return page_height - height_header - height_footer;
}

int getPageCount(
    const char *ttf_filename,
    const size_in page_height,
    const size_in page_width,
    const size_in margin_top,
    const size_in margin_bottom,
    const size_in margin_left,
    const size_in margin_right,
    const size_pt font_size,
    const size_in paragraph_spacing_before,
    const size_in paragraph_spacing_after,
    const size_in header_from_top,
    const size_in footer_from_bottom,
    const float user_line_spacing,
    const char *filename
) {
    const size_pt max_page_content_width = inchToPt(page_width - (margin_left + margin_right));
    const size_pt paragraph_spacing = inchToPt(paragraph_spacing_before + paragraph_spacing_after);

    ttf_t *const font_file = ttfCreate(ttf_filename, 0, throw_err, NULL);

    const size_pt line_height = getSizeByCharSize(
        getLineHeight(
            ttfGetAscent(font_file),
            ttfGetDescent(font_file),
            getHHEALineGap(ttf_filename),
            user_line_spacing), font_size);

    const size_em line_gap = user_line_spacing * line_height;

    const size_pt max_page_content_height = getMaxPageContentHeight(
        inchToPt(page_height),
        inchToPt(margin_top),
        inchToPt(margin_bottom),
        inchToPt(header_from_top),
        inchToPt(footer_from_bottom),
        // the line height spacing is like 1.0
        ((float) line_height / user_line_spacing) * 1.0f
    );

#ifndef NDEBUG
    printf("Max page content height: %f\n", max_page_content_height);
    printf("Max page content width: %f\n", max_page_content_width);
    printf("Line height: %f\n", line_height);
#endif

    FILE *const file = fopen(filename, "r");

    int pageCount = 0;
    size_pt curr_page_height = line_height; // start with line 1
    size_pt line_curr_width = 0;
    size_pt curr_word_width = 0;
    char last_char;
    while ((last_char = (char) fgetc(file)) != EOF) {
        bool was_newline = false;
        if (last_char == '\n') {
            curr_word_width = 0;
            line_curr_width = 0;
            was_newline = true;
            goto Increase_line;
        }

        const size_pt char_width = getSizeByCharSize((float) ttfGetWidth(font_file, last_char), font_size);
        // #ifndef NDEBUG
        //         if (last_char == ' ')
        //             printf("Char width of %c: %f\n", last_char, char_width);
        // #endif
        curr_word_width = last_char == ' ' ? 0 : curr_word_width + char_width;

        // wrap line if needed
        line_curr_width += char_width;
        if (line_curr_width >= max_page_content_width /*UNSURE*/) {
            line_curr_width = curr_word_width;
        Increase_line:
            if (was_newline)
                curr_page_height += paragraph_spacing;
            else
                curr_page_height += line_gap + line_height; // add that the content now takes a new line

            if (curr_page_height > max_page_content_height) {
                pageCount++;
                curr_page_height = 0;
            }

            if (was_newline) {
                was_newline = false;
                goto Increase_line;
            }
        }
    }
#ifndef NDEBUG
    printf("Last page content height: %f\n", curr_page_height);
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
