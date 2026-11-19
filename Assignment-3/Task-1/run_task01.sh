#!/usr/bin/env bash
set -e

echo "=== Building Static, Dynamic, and dlopen Targets ==="
gcc -std=c11 -Wall -Wextra -O2 -c crypto_lib.c -o crypto_lib_static.o
ar rcs libbankcrypto.a crypto_lib_static.o
gcc -std=c11 -Wall -Wextra -O2 -static main_link.c -L. -lbankcrypto -o bank_static

gcc -std=c11 -Wall -Wextra -O2 -fPIC -shared crypto_lib.c -o libbankcrypto.so
gcc -std=c11 -Wall -Wextra -O2 main_link.c -L. -lbankcrypto -Wl,-rpath,'$ORIGIN' -o bank_dynamic

gcc -std=c11 -Wall -Wextra -O2 main_dlopen.c -ldl -o bank_dlopen

echo "=== 1. Executable Sizes ==="
ls -lh bank_static bank_dynamic bank_dlopen libbankcrypto.so

echo "=== 2. Shared Library Dependencies (ldd) ==="
ldd bank_static || true
ldd bank_dynamic

echo "=== 3. Runtime dlopen() & /proc/self/maps ==="
./bank_dlopen e

echo "=== 4. Live Shared Library Update to v2.0 Without Recompiling ==="
gcc -std=c11 -Wall -Wextra -O2 -fPIC -shared crypto_lib_v2.c -o libbankcrypto.so
./bank_dynamic
./bank_static

echo "=== 5. Missing Shared Library Test ==="
mv libbankcrypto.so libbankcrypto.so.bak
./bank_dynamic || true
./bank_dlopen e || true
mv libbankcrypto.so.bak libbankcrypto.so
