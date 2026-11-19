#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int sim_fifo(const int *ref, int len, int frames) {
    int *f = malloc(frames * sizeof(int)), used = 0, next = 0, faults = 0;
    for (int i = 0; i < len; i++) {
        int hit = 0;
        for (int j = 0; j < used; j++) if (f[j] == ref[i]) { hit = 1; break; }
        if (hit) continue;
        faults++;
        if (used < frames) f[used++] = ref[i];
        else { f[next] = ref[i]; next = (next + 1) % frames; }
    }
    free(f); return faults;
}

int sim_lru(const int *ref, int len, int frames) {
    int *f = malloc(frames * sizeof(int)), *last = malloc(frames * sizeof(int));
    int used = 0, faults = 0;
    for (int i = 0; i < len; i++) {
        int hit = -1;
        for (int j = 0; j < used; j++) if (f[j] == ref[i]) { hit = j; break; }
        if (hit >= 0) { last[hit] = i; continue; }
        faults++;
        if (used < frames) { f[used] = ref[i]; last[used] = i; used++; }
        else {
            int v = 0;
            for (int j = 1; j < frames; j++) if (last[j] < last[v]) v = j;
            f[v] = ref[i]; last[v] = i;
        }
    }
    free(f); free(last); return faults;
}

int sim_clock(const int *ref, int len, int frames) {
    int *f = malloc(frames * sizeof(int)), *rbit = calloc(frames, sizeof(int));
    int used = 0, hand = 0, faults = 0;
    for (int i = 0; i < len; i++) {
        int hit = -1;
        for (int j = 0; j < used; j++) if (f[j] == ref[i]) { hit = j; break; }
        if (hit >= 0) { rbit[hit] = 1; continue; }
        faults++;
        if (used < frames) { f[used] = ref[i]; rbit[used] = 1; used++; }
        else {
            while (rbit[hand] == 1) { rbit[hand] = 0; hand = (hand + 1) % frames; }
            f[hand] = ref[i]; rbit[hand] = 1; hand = (hand + 1) % frames;
        }
    }
    free(f); free(rbit); return faults;
}

double ms_diff(struct timespec a, struct timespec b) {
    return (b.tv_sec - a.tv_sec) * 1000.0 + (b.tv_nsec - a.tv_nsec) / 1e6;
}

int main(void) {
    int N = 100000;
    int *loc = malloc(N * sizeof(int)), *rnd = malloc(N * sizeof(int));
    srand(55);
    for (int i = 0; i < N; i++) {
        loc[i] = (rand() % 100 < 80) ? (rand() % 4) : (4 + rand() % 16);
        rnd[i] = rand() % 20;
    }

    printf("Frames | Loc-FIFO | Loc-Clock | Loc-LRU | Rnd-FIFO | Rnd-Clock | Rnd-LRU\n");
    for (int fr = 3; fr <= 10; fr++) {
        printf("%6d | %8d | %9d | %7d | %8d | %9d | %7d\n", fr,
               sim_fifo(loc, N, fr), sim_clock(loc, N, fr), sim_lru(loc, N, fr),
               sim_fifo(rnd, N, fr), sim_clock(rnd, N, fr), sim_lru(rnd, N, fr));
    }

    struct timespec t0, t1, t2, t3;
    clock_gettime(CLOCK_MONOTONIC, &t0); sim_fifo(loc, N, 8);
    clock_gettime(CLOCK_MONOTONIC, &t1); sim_clock(loc, N, 8);
    clock_gettime(CLOCK_MONOTONIC, &t2); sim_lru(loc, N, 8);
    clock_gettime(CLOCK_MONOTONIC, &t3);

    printf("\n100,000 References Runtime (8 frames): FIFO=%.3f ms, Clock=%.3f ms, LRU=%.3f ms\n",
           ms_diff(t0, t1), ms_diff(t1, t2), ms_diff(t2, t3));
    free(loc); free(rnd); return 0;
}
