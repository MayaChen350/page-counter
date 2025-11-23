#include <stdio.h>
#include <pthread.h>
#include <stdbool.h>

#include "page_counter.h"

volatile bool stop_thread = false;

void* console_loading();

int main(void) {
    pthread_t console_ui_thread;
    pthread_create(&console_ui_thread, NULL, console_loading, NULL);

    const int page_counter = 1;
    stop_thread = true;
    pthread_join(console_ui_thread, NULL);

    printf("%i\n", page_counter);
    return 0;
}
