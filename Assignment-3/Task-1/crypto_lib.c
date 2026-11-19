#include <stdio.h>
#include <stdint.h>

/* 4 MB static lookup table to make executable size and RAM sharing clearly visible */
static const uint32_t CRYPTO_SBOX[1048576] = {0x5A3C9E11, 0x1F2E3D4C, 0x89ABCDEF};

const char* crypto_version(void) {
    return "BankCrypto Engine v1.0 (Initial Release)";
}

uint64_t encrypt_amount(uint64_t amount, uint32_t key) {
    uint32_t sbox_val = CRYPTO_SBOX[(amount ^ key) & 0xFFFFF];
    return (amount ^ ((uint64_t)key << 16)) + sbox_val;
}
