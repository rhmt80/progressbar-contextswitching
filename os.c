#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>
#include <pthread.h>

#define ESC "\033"
#define CSI "["
#define PREV_LINE "F"
#define BACKSPACE "D"

const int PROG_BAR_LEN = 30;
const int NUM_THREADS = 5;

typedef struct {
    int count_to_val;
    int progress;
    pthread_t thethread;
} thread_info;

void update_bar(thread_info *tinfo) {
    int num_chars = (tinfo->progress * 100 / tinfo->count_to_val) * PROG_BAR_LEN / 100;
    printf("[");
    for (int i = 0; i < num_chars; i++) {
        printf("=");
    }

    if (tinfo->progress < tinfo->count_to_val) {
        printf(ESC CSI BACKSPACE ">");
    }

    for (int i = num_chars; i < PROG_BAR_LEN; i++) {
        printf(" ");
    }
    printf("]\n");
}

void *update_progress(void *arg) {
	thread_info *tinfo = (thread_info *) arg;
	for (int i = 0; i < tinfo->count_to_val; i++) {
		tinfo->progress++;
		usleep(1000);
	}
	pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    thread_info threads[NUM_THREADS];
	int total = 53142;

	printf("Total: %d\n", total);

    for (int i = 0; i < NUM_THREADS-1; i++) {
		threads[i].count_to_val = total / NUM_THREADS;
        threads[i].progress = 0;
        pthread_create(&threads[i].thethread, NULL, update_progress, &threads[i]);
    }

	threads[NUM_THREADS-1].count_to_val = total-(total/NUM_THREADS)*(NUM_THREADS-1);
	threads[NUM_THREADS-1].progress = 0;
	pthread_create(&threads[NUM_THREADS-1].thethread, NULL, update_progress, &threads[NUM_THREADS-1]);

    int current_thread = 0;
    bool done = false;

    while (!done) {
        done = true;
        for (int i = 0; i < NUM_THREADS; i++) {
            if (threads[i].progress < threads[i].count_to_val) {
                done = false;
            }
			update_bar(&threads[i]);
			printf("Thread %ld: %d\n", threads[i].thethread, threads[i].progress);
        }

        if (!done) {
            printf(ESC CSI "%d" PREV_LINE, 2*(NUM_THREADS));
        }

        usleep(10000);
    }

    pthread_exit(NULL);
}