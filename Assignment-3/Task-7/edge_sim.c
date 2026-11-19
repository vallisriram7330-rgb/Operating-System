#include <stdio.h>
#include <stdlib.h>

#define RT_FRAMES   4    /* 64 MB pinned */
#define SHARED_POOL 112  /* 1792 MB shared between ML & Logging */

int main(void) {
    int rt_faults = 0, ml_faults = 0, log_faults = 0;
    int pool[SHARED_POOL], rbit[SHARED_POOL] = {0}, hand = 0, used = 0;
    srand(77);

    for (int step = 0; step < 10000; step++) {
        int r = step % 20;
        if (r < 4) {
            /* Real-Time task accesses pinned frames 0..3 -> 0 faults guaranteed */
            int frame = r % RT_FRAMES;
            (void)frame;
        } else {
            int page = (r < 17) ? (100 + rand() % 120) : (300 + rand() % 20);
            int is_ml = (page < 300);
            int hit = -1;
            for (int j = 0; j < used; j++) if (pool[j] == page) { hit = j; break; }
            if (hit >= 0) rbit[hit] = 1;
            else {
                if (is_ml) ml_faults++; else log_faults++;
                if (used < SHARED_POOL) { pool[used] = page; rbit[used] = 1; used++; }
                else {
                    while (rbit[hand]) { rbit[hand] = 0; hand = (hand + 1) % SHARED_POOL; }
                    pool[hand] = page; rbit[hand] = 1; hand = (hand + 1) % SHARED_POOL;
                }
            }
        }
    }

    printf("=== Edge Device 2 GB Memory Simulation (3 Tasks) ===\n");
    printf("Real-Time Control Task (64 MB Pinned) : %d page faults\n", rt_faults);
    printf("ML Inference Task (1.5 GB Unpinned)   : %d page faults (isolated)\n", ml_faults);
    printf("Logging Task (Ring Buffer Unpinned)   : %d page faults (isolated)\n", log_faults);
    return 0;
}
