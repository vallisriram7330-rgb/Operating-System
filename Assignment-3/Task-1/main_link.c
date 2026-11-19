#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

extern const char* crypto_version(void);
extern uint64_t encrypt_amount(uint64_t amount, uint32_t key);

void print_proc_maps(void) {
    FILE *fp = fopen("/proc/self/maps", "r");
    if (!fp) { perror("fopen /proc/self/maps"); return; }
    char line[256];
    int count = 0;
    printf("\n--- Virtual Memory Mappings (/proc/self/maps sample) ---\n");
    while (fgets(line, sizeof(line), fp) && count < 18) {
        printf("%s", line);
        count++;
    }
    fclose(fp);
}

int main(int argc, char *argv[]) {
    uint64_t tx_amount = 250000;
    uint32_t branch_key = 0x4B91;

    printf("Banking Terminal Started (PID: %d)\n", getpid());
    printf("Loaded Module : %s\n", crypto_version());
    printf("Encrypted Tx  : 0x%016llx\n",
           (unsigned long long)encrypt_amount(tx_amount, branch_key));

    if (argc > 1 && argv[1][0] == 'm') print_proc_maps();
    if (argc > 1 && argv[1][0] == 'w') {
        printf("Holding process open for 10s to inspect shared physical pages...\n");
        sleep(10);
    }
    return 0;
}
