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

    const int page_counter = getPageCount(
        "arial.ttf",
        11,
        8.5f,
        1,
        1,
        1,
        1,
        11,
        0.15f,
        0.0f,
        0.5f,
        0.5f,
        1.15f,
        "test.txt"
    );
    // stop_thread = true;
    // pthread_join(console_ui_thread, NULL);

    // should be 3
    printf("%i\n", page_counter);
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
