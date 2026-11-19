#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>
#include <unistd.h>

typedef const char* (*ver_fn_t)(void);
typedef uint64_t (*enc_fn_t)(uint64_t, uint32_t);

int main(int argc, char *argv[]) {
    printf("Banking Terminal (On-Demand dlopen Mode) PID: %d\n", getpid());
    if (argc < 2 || argv[1][0] != 'e') {
        printf("No encryption requested for this session. Exiting cleanly.\n");
        return 0;
    }

    void *handle = dlopen("./libbankcrypto.so", RTLD_LAZY);
    if (!handle) {
        fprintf(stderr, "[ERROR] Failed to load library: %s\n", dlerror());
        return EXIT_FAILURE;
    }

    dlerror();
    ver_fn_t get_ver = (ver_fn_t)dlsym(handle, "crypto_version");
    char *err = dlerror();
    if (err != NULL) {
        fprintf(stderr, "[ERROR] Symbol crypto_version failed: %s\n", err);
        dlclose(handle);
        return EXIT_FAILURE;
    }

    enc_fn_t enc_amt = (enc_fn_t)dlsym(handle, "encrypt_amount");
    err = dlerror();
    if (err != NULL) {
        fprintf(stderr, "[ERROR] Symbol encrypt_amount failed: %s\n", err);
        dlclose(handle);
        return EXIT_FAILURE;
    }

    printf("Dynamically Loaded: %s\n", get_ver());
    printf("Encrypted Tx      : 0x%016llx\n",
           (unsigned long long)enc_amt(250000, 0x4B91));

    FILE *fp = fopen("/proc/self/maps", "r");
    if (fp) {
        char line[256];
        printf("\n--- Mappings Containing bank_dlopen or libbankcrypto ---\n");
        while (fgets(line, sizeof(line), fp)) {
            if (strstr(line, "bank") || strstr(line, "libc") ||
                strstr(line, "[heap]") || strstr(line, "[stack]")) {
                printf("%s", line);
            }
        }
        fclose(fp);
    }

    dlclose(handle);
    return EXIT_SUCCESS;
}
