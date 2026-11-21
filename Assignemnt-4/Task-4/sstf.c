#include <stdio.h>
#include <stdlib.h>

#define N 8

int main(void)
{
    int req[N] = {98, 183, 37, 122, 14, 124, 65, 67};
    int done[N] = {0};
    int order[N];
    int pos = 53;

    for (int k = 0; k < N; k++) {
        int min_distance = 9999;
        int selected = -1;

        for (int i = 0; i < N; i++) {
            if (!done[i]) {
                int distance = abs(req[i] - pos);
                if (distance < min_distance) {
                    min_distance = distance;
                    selected = i;
                }
            }
        }

        done[selected] = 1;
        order[k] = req[selected];
        pos = req[selected];
    }

    pos = 53;
    int total = 0;

    printf("SSTF order: 53");
    for (int i = 0; i < N; i++) {
        printf(" -> %d", order[i]);
        total += abs(order[i] - pos);
        pos = order[i];
    }

    printf("\nTotal head movement: %d cylinders\n", total);
    return 0;
}
