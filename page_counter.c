//
// Created by mayachen on 2025-11-22.
//

#include <stdio.h>
#include <math.h>
#include "include/ttf.h"
#include "page_counter.h"

#include <stdlib.h>

#include "ttf_reader.h"
#include "utils.h"

signed short getHHEALineGap(const char *ttf_filename);

static size_pt getSizeByCharSize(const float width, const size_pt font_size, const unsigned short unit_per_em) {
    printf("%f", width);
    return width * font_size;
}

static size_pt getLineHeight(const FWORD font_ascent,
                             const FWORD font_descent,
                             const FWORD line_gap,
                             const unsigned short unit_per_em,
                             const size_pt font_size) {
    return font_size * ((float) (font_ascent + abs(font_descent) + line_gap) / unit_per_em);
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

    struct font_t font;
    font_t_init(&font, ttf_filename);

    const size_pt line_height = getLineHeight(
        font.ascender,
        font.descender,
        font.line_gap,
        font.unitsPerEm,
        font_size);

    const size_pt line_gap = user_line_spacing * line_height;

    const size_pt max_page_content_height = getMaxPageContentHeight(
        inchToPt(page_height),
        inchToPt(margin_top),
        inchToPt(margin_bottom),
        inchToPt(header_from_top),
        inchToPt(footer_from_bottom),
        // the line height spacing is like 1.0
        (float) line_height * 1.0f
    );

#ifndef NDEBUG
    printf("Max page content height: %f\n", max_page_content_height);
    printf("Max page content width: %f\n", max_page_content_width);
    printf("Line height: %f\n", line_height);
#endif

    FILE *const file = fopen(filename, "r");
    ttf_t *const font_file = ttfCreate(ttf_filename, 0, throw_err, NULL);

    int pageCount = 0;
    size_pt curr_page_height = line_height; // start with line 1
    size_pt line_curr_width = 0;
    size_pt curr_word_width = 0;
    char last_char;
    while ((last_char = (char) fgetc(file)) != EOF) {
        bool was_newline = false;
#ifdef _WIN64
        if (last_char == '\r') {
            // pass the `\n` character to do the same thing
            fgetc(file); // now `\n`
#else
        if (last_char == '\n') {
#endif
            curr_word_width = 0;
            line_curr_width = 0;
            was_newline = true;
            goto Increase_line;
        }

        const size_pt char_width = getSizeByCharSize(ttfGetWidth(font_file, last_char), font_size, font.unitsPerEm);
        #ifndef NDEBUG
                if (last_char == ' ')
                    printf("Char width of %c: %f\n", last_char, char_width);
        #endif
        if (last_char == ' ' || last_char == '\r' || last_char == '\t')
            curr_word_width = 0;
        else
            curr_word_width = curr_word_width + char_width;

        // wrap line if needed
        line_curr_width += char_width;
        if (line_curr_width >= max_page_content_width /*UNSURE*/) {
            line_curr_width = curr_word_width;
        Increase_line:
            if (was_newline)
                curr_page_height += paragraph_spacing;
            else
                curr_page_height += line_gap + line_height; // add that the content now takes a new line

            if (curr_page_height >= max_page_content_height) {
                pageCount++;
                curr_page_height = line_height; // first line, new page
                continue; // if a new paragraph start on a new line, I guess there is no extra space needed??
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
