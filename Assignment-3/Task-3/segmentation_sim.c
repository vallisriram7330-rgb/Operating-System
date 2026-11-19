#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <setjmp.h>
#include <unistd.h>

typedef struct {
    const char *name;
    unsigned int base;
    unsigned int limit;
    const char *perm;
} SegmentEntry;

static sigjmp_buf jump_buffer;

void segfault_handler(int sig) {
    printf("[OS SIGNAL HANDLER] Caught signal %d (SIGSEGV): Out-of-bounds access trapped!\n", sig);
    siglongjmp(jump_buffer, 1);
}

void simulate_fragmentation(void) {
    #define RAM_SIZE 1024
    int ram[RAM_SIZE] = {0};
    int seg_base[25], seg_len[25];
    srand(103);

    printf("\n--- 20-Segment Load/Remove External Fragmentation Experiment (1024 KB RAM) ---\n");
    for (int id = 1; id <= 20; id++) {
        int size = 20 + (rand() % 46);
        int placed = -1;
        for (int start = 0; start <= RAM_SIZE - size; start++) {
            int ok = 1;
            for (int k = 0; k < size; k++) if (ram[start + k] != 0) { ok = 0; break; }
            if (ok) { placed = start; break; }
        }
        seg_base[id] = placed;
        seg_len[id] = size;
        if (placed != -1) for (int k = 0; k < size; k++) ram[placed + k] = id;
    }

    /* Remove odd-numbered segments to create non-contiguous holes */
    for (int id = 1; id <= 19; id += 2) {
        if (seg_base[id] != -1)
            for (int k = 0; k < seg_len[id]; k++) ram[seg_base[id] + k] = 0;
    }

    int total_free = 0, largest_hole = 0, cur = 0, holes = 0;
    for (int i = 0; i < RAM_SIZE; i++) {
        if (ram[i] == 0) {
            total_free++; cur++;
            if (cur == 1) holes++;
            if (cur > largest_hole) largest_hole = cur;
        } else cur = 0;
    }
    double frag = total_free > 0 ? (100.0 * (total_free - largest_hole) / total_free) : 0.0;
    printf("Total Free: %d KB across %d holes | Largest Hole: %d KB | External Frag: %.2f%%\n",
           total_free, holes, largest_hole, frag);
}

int main(void) {
    SegmentEntry seg_table[4] = {
        {"Code",  1000,  400, "R-X"},
        {"Data",  2000,  800, "RW-"},
        {"Heap",  4000, 1200, "RW-"},
        {"Stack", 7000,  600, "RW-"}
    };

    int tests[8][2] = {
        {0, 120}, {0, 450}, {1, 0}, {1, 799},
        {2, 1200}, {2, 650}, {3, 300}, {3, 900}
    };

    printf("=== Segment Table Translation (8 Addresses) ===\n");
    for (int i = 0; i < 8; i++) {
        int s = tests[i][0];
        unsigned int d = (unsigned int)tests[i][1];
        printf("Test %d: (Seg %d [%s], Offset %4u) -> ", i + 1, s, seg_table[s].name, d);
        if (d < seg_table[s].limit)
            printf("VALID -> Physical Address = %u\n", seg_table[s].base + d);
        else
            printf("TRAP -> Offset %u >= Limit %u (Segmentation Fault)\n", d, seg_table[s].limit);
    }

    simulate_fragmentation();

    printf("\n=== Safe Hardware Segmentation Fault Demonstration ===\n");
    signal(SIGSEGV, segfault_handler);
    if (sigsetjmp(jump_buffer, 1) == 0) {
        int *bad_ptr = (int *)0x0;
        *bad_ptr = 42;
    } else {
        printf("Recovered safely after OS kernel delivered SIGSEGV.\n");
    }
    return 0;
}
