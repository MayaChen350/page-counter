#include <locale.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "page_counter.h"

// volatile bool stop_thread = false;

void *console_loading();

int startswith(const char *str1, const char *str2) {
  while (*str1 == *str2) {
    str1++;
    str2++;
    if (*str2 == '\0')
      return 1;
    if (*str1 == '\0')
      return 0;
  }

  return 0;
}

int is_atleastlen(const char *str, const size_t min_len) {
  for (size_t i; i < min_len; i++) {
    if (str[i] == '\0')
      return 0;
  }

  return 1;
}

int main(int argc, char **argv) {
  setlocale(LC_ALL, "");
  // pthread_t console_ui_thread;
  // pthread_create(&console_ui_thread, NULL, console_loading, NULL);
  char usage_help[] =
      "usage: pagecounter (<file path>/--stdin) <font path> <params>\n"
      "Params:\n"
      "\t--font-size (default: 12 pt)\n"
      "\t--page-height (default: 11 \")\n"
      "\t--page-width (default: 8.5 \")\n"
      "\t--margin-top (default: 1 \")\n"
      "\t--margin-bottom (default: 1 \")\n"
      "\t--margin-left (default: 1 \")\n"
      "\t--margin-right (default: 1 \")\n"
      "\t--paragraph-spacing-before (default: 0 \")\n"
      "\t--paragraph-spacing-after (default: 0.14 \")\n"
      "\t--header-from-top (default: 0.5 \")\n"
      "\t--header-from-bottom (default: 0.5 \")\n"
      "\t--line-spacing (default: 1.15)\n";

  if (argc < 3 ||
      (argc - 3) % 2 != 0) { // every optional parameters must be in a pair
    fprintf(stderr, "%s", usage_help);
    return 1;
  }

  // processing arguments
  size_pt font_size = 12;
  size_in page_height = 11;
  size_in page_width = 8.5;
  size_in margin_top, margin_bottom, margin_left, margin_right;
  margin_top = margin_bottom = margin_left = margin_right = 1;
  size_in para_spacing_bef = 0;
  size_in para_spacing_aft = 0.14;
  size_in header_from_top, header_from_bottom;
  header_from_top = header_from_bottom = 0.5;
  double line_spacing = 1.15;

  if (argc >= 5)
    for (int i = 3; i < argc; i += 2) {
      if (!is_atleastlen(argv[i], 3))
        continue;
      const char *argname = argv[i] + 2; // 2 for the --

      if (argname[0] == 'f') {
        if (!strcmp(argname, "font-size"))
          font_size = atof(argv[i + 1]);
      } else if (argname[0] == 'l') {
        if (!strcmp(argname, "line-spacing"))
          line_spacing = atof(argv[i + 1]);
      } else if (argname[0] == 'p' && argname[1] == 'a') {
        if (startswith(argname + 2, "ge--")) {
          if (!strcmp(argname + 6, "height"))
            page_height = atof(argv[i + 1]);
          else if (!strcmp(argname + 6, "width"))
            page_width = atof(argv[i + 1]);
        } else if (startswith(argname + 2, "ragraph-spacing-")) {
          if (!strcmp(argname + 6, "before"))
            para_spacing_bef = atof(argv[i + 1]);
          else if (!strcmp(argname + 6, "after"))
            para_spacing_aft = atof(argv[i + 1]);
        }
      } else if (startswith(argname, "margin-")) {
        const char *margin_type = argname + 7;

        if (!strcmp(margin_type, "top"))
          margin_top = atof(argv[i + 1]);
        else if (!strcmp(margin_type, "bottom"))
          margin_bottom = atof(argv[i + 1]);
        else if (!strcmp(margin_type, "left"))
          margin_left = atof(argv[i + 1]);
        else if (!strcmp(margin_type, "right"))
          margin_right = atof(argv[i + 1]);
      } else if (startswith(argname, "header-from-")) {
        const char *margin_type = argname + 12;

        if (!strcmp(margin_type, "top"))
          header_from_top = atof(argv[i + 1]);
        else if (!strcmp(margin_type, "bottom"))
          header_from_bottom = atof(argv[i + 1]);
      }
    }

  char *filename = strcmp(argv[1], "--stdin") == 0 ? NULL : argv[1];

  int result = page_count(
      filename, argv[2], font_size, line_spacing, page_height, page_width,
      margin_top, margin_bottom, margin_left, margin_right, para_spacing_bef,
      para_spacing_aft, header_from_top, header_from_bottom);

  // stop_thread = true;
  // pthread_join(console_ui_thread, NULL);

  printf("%i", result);
  return 0;
}

// void *console_loading() {
//     printf("Loading");
//     int state = 0;
//     while (!stop_thread) {
//         if (state == 0)
//             printf("\b\b\b");
//         else
//             printf(".");
//         state = (state + 1) % 4;
//         sleep(2);
//     }
//     printf("\n");
//     return NULL;
// }
