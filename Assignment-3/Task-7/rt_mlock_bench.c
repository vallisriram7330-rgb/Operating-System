#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/resource.h>

#define ITERATIONS 20000
#define BUF_BYTES  (32 * 1024 * 1024)

static inline uint64_t now_ns(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000000000ULL + (uint64_t)ts.tv_nsec;
}

int cmp_u64(const void *a, const void *b) {
    uint64_t x = *(const uint64_t *)a, y = *(const uint64_t *)b;
    return (x > y) - (x < y);
}

void run_control_loop(int lock_memory) {
    struct rusage ru;
    if (lock_memory) {
        if (mlockall(MCL_CURRENT | MCL_FUTURE) != 0)
            fprintf(stderr, "[WARN] mlockall failed (%s). Run with sudo.\n", strerror(errno));
        else
            printf("[INFO] mlockall(MCL_CURRENT | MCL_FUTURE) enabled.\n");
    }

    char *buf = malloc(BUF_BYTES);
    uint64_t *lat = malloc(ITERATIONS * sizeof(uint64_t));
    if (!buf || !lat) { perror("malloc"); exit(1); }

    /* Prefault all pages when locked so zero page faults occur inside the control loop */
    if (lock_memory)
        for (size_t off = 0; off < BUF_BYTES; off += 4096) buf[off] = 1;

    getrusage(RUSAGE_SELF, &ru);
    long f0 = ru.ru_minflt;

    uint64_t sum = 0, max_ns = 0;
    for (int i = 0; i < ITERATIONS; i++) {
        uint64_t t0 = now_ns();
        size_t idx = ((size_t)i * 4096) % BUF_BYTES;
        buf[idx] = (char)(i & 0xFF);
        uint64_t dt = now_ns() - t0;
        lat[i] = dt; sum += dt;
        if (dt > max_ns) max_ns = dt;
    }

    getrusage(RUSAGE_SELF, &ru);
    long f1 = ru.ru_minflt;
    qsort(lat, ITERATIONS, sizeof(uint64_t), cmp_u64);

    printf("Mode: %-15s | Avg: %6.1f ns | P99: %6lu ns | Max: %8lu ns | Loop Faults: %ld\n",
           lock_memory ? "LOCKED+PREFAULT" : "UNLOCKED",
           (double)sum / ITERATIONS, (unsigned long)lat[(size_t)(ITERATIONS * 0.99)],
           (unsigned long)max_ns, f1 - f0);

    free(lat); free(buf);
    if (lock_memory) munlockall();
}

int main(void) {
    printf("=== Smart Factory Real-Time Control Loop Latency Test ===\n");
    run_control_loop(0);
    run_control_loop(1);
    return 0;
}
