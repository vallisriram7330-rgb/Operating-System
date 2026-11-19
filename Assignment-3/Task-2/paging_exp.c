#include <stdio.h>
#include <stdlib.h>

int main(void) {
    srand(42);
    size_t proc_sizes[100], total_req = 0;
    for (int i = 0; i < 100; i++) {
        proc_sizes[i] = 1024 + (rand() % (512 * 1024));
        total_req += proc_sizes[i];
    }

    size_t p_sizes[3] = {1024, 4096, 2 * 1024 * 1024};
    const char *names[3] = {"1 KB", "4 KB", "2 MB"};
    printf("100 Random Processes Total Requested: %zu bytes (%.2f MB)\n",
           total_req, total_req / (1024.0 * 1024.0));
    for (int p = 0; p < 3; p++) {
        size_t alloc = 0;
        for (int i = 0; i < 100; i++)
            alloc += ((proc_sizes[i] + p_sizes[p] - 1) / p_sizes[p]) * p_sizes[p];
        size_t frag = alloc - total_req;
        printf("Page Size %-4s -> Allocated: %10zu B | Internal Frag: %10zu B (%.2f%%)\n",
               names[p], alloc, frag, (100.0 * frag) / total_req);
    }

    double t_tlb = 10.0, t_mem = 100.0;
    double hits[3] = {0.80, 0.95, 0.99};
    printf("\nTLB EAT (t_TLB=10ns, t_mem=100ns, No-TLB=200ns):\n");
    for (int h = 0; h < 3; h++) {
        double a = hits[h];
        double eat = a * (t_tlb + t_mem) + (1.0 - a) * (t_tlb + 2.0 * t_mem);
        printf("Hit Ratio %.0f%% -> EAT = %.2f ns\n", a * 100.0, eat);
    }

    unsigned ps = 4096, max_p = 8;
    unsigned bounds[] = {0, 4095, 4096, 32767, 32768};
    printf("\nBoundary Tests (8 pages x 4096 B):\n");
    for (int k = 0; k < 5; k++) {
        unsigned pg = bounds[k] / ps, off = bounds[k] % ps;
        printf("Addr %5u -> Page %u, Offset %4u : %s\n",
               bounds[k], pg, off, (pg < max_p) ? "VALID" : "INVALID (Beyond Last Page)");
    }
    return 0;
}
