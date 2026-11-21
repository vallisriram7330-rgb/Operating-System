#include <stdio.h>

#define CHUNKS 1000

int main(void)
{
    int target;

    printf("Enter requested video chunk (0-999): ");
    if (scanf("%d", &target) != 1) {
        printf("Invalid input.\n");
        return 1;
    }

    if (target < 0 || target >= CHUNKS) {
        printf("Invalid chunk number.\n");
        return 1;
    }

    int sequential_reads = target + 1;
    int indexed_reads = 1;

    printf("\n--- Video Storage Comparison ---\n");
    printf("Requested chunk: %d\n", target);
    printf("Sequential reads: %d\n", sequential_reads);
    printf("Indexed data reads: %d\n", indexed_reads);
    printf("Sequential access complexity: O(n)\n");
    printf("Indexed access complexity: O(1)\n");

    return 0;
}
