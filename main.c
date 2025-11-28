#include <stdio.h>
#include <pthread.h>
#include <stdbool.h>
#include <unistd.h>

#include "page_counter.h"

// volatile bool stop_thread = false;

void *console_loading();

int main(void) {
    // pthread_t console_ui_thread;
    // pthread_create(&console_ui_thread, NULL, console_loading, NULL);

    // TESTS FOR NOW???

    const char *file_names[2] = {"test.txt", "test2.txt"};
    const int expected[2] = {13, 3};
    int return_code = 0;

    for (int i = 0; i < 2; i++) {
        const int page_counter = getPageCount(
            "arial.ttf",
            11,
            8.5f,
            1,
            1,
            1,
            1,
            11,
            0.0f,
            0.15f,
            0.5f,
            0.5f,
            1.15f,
            file_names[i]
        );
        printf("Result: %i\n", page_counter);
        if (page_counter != expected[i]) {
            fprintf(stderr, "\nExpected %i, Actual: %i\n\n", expected[i], page_counter);
            return_code = 1;
        }
    }


    // stop_thread = true;
    // pthread_join(console_ui_thread, NULL);

    // should be 3
    return return_code;
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
