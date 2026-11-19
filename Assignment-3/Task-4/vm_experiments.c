#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/resource.h>
#include <sys/wait.h>

double now_sec(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec * 1e-9;
}

long get_minor_faults(void) {
    struct rusage ru;
    getrusage(RUSAGE_SELF, &ru);
    return ru.ru_minflt;
}

void run_access_test(size_t mb, int random_order) {
    size_t bytes = mb * 1024ULL * 1024ULL;
    size_t num_pages = bytes / 4096;
    char *buf = malloc(bytes);
    if (!buf) { printf("Allocation of %zu MB failed!\n", mb); return; }

    size_t *order = malloc(num_pages * sizeof(size_t));
    for (size_t i = 0; i < num_pages; i++) order[i] = i;
    if (random_order) {
        srand(42);
        for (size_t i = num_pages - 1; i > 0; i--) {
            size_t j = (size_t)rand() % (i + 1);
            size_t tmp = order[i]; order[i] = order[j]; order[j] = tmp;
        }
    }

    long f0 = get_minor_faults();
    double t0 = now_sec();
    for (int pass = 0; pass < 4; pass++)
        for (size_t i = 0; i < num_pages; i++)
            buf[order[i] * 4096] = (char)(pass + i);
    double t1 = now_sec();
    long f1 = get_minor_faults();

    printf("%-10s | Size: %4zu MB | Time: %7.4f s | Minor Faults: %8ld\n",
           random_order ? "Random" : "Sequential", mb, t1 - t0, f1 - f0);
    free(order); free(buf);
}

void test_copy_on_write(void) {
    size_t bytes = 64ULL * 1024ULL * 1024ULL;
    char *shared = malloc(bytes);
    memset(shared, 1, bytes);

    printf("\n=== Copy-on-Write (fork) Demonstration (64 MB Buffer) ===\n");
    pid_t pid = fork();
    if (pid == 0) {
        long f0 = get_minor_faults();
        volatile long sum = 0;
        for (size_t i = 0; i < bytes; i += 4096) sum += shared[i];
        long f1 = get_minor_faults();

        for (size_t i = 0; i < bytes; i += 4096) shared[i] = 2;
        long f2 = get_minor_faults();

        printf("Child PID %d -> Minor faults during READ scan : %ld\n", getpid(), f1 - f0);
        printf("Child PID %d -> Minor faults during WRITE scan: %ld (COW triggered)\n", getpid(), f2 - f1);
        free(shared);
        exit(0);
    } else {
        wait(NULL);
        free(shared);
    }
}

int main(void) {
    printf("=== Sequential vs Random Memory Access & Scaling Test ===\n");
    size_t sizes[] = {64, 256, 512, 1024};
    for (int i = 0; i < 4; i++) {
        run_access_test(sizes[i], 0);
        run_access_test(sizes[i], 1);
    }
    test_copy_on_write();
    return 0;
}
