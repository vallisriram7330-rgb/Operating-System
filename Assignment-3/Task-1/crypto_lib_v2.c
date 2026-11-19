#include <stdio.h>
#include <stdint.h>

static const uint32_t CRYPTO_SBOX[1048576] = {0x99887766, 0x55443322, 0x11223344};

const char* crypto_version(void) {
    return "BankCrypto Engine v2.0 (SECURITY PATCH APPLIED)";
}

uint64_t encrypt_amount(uint64_t amount, uint32_t key) {
    uint32_t sbox_val = CRYPTO_SBOX[(amount ^ key) & 0xFFFFF];
    return ((amount ^ ((uint64_t)key << 20)) + sbox_val) ^ 0xA5A5A5A5ULL;
}
