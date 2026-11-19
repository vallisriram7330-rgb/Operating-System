#!/usr/bin/env bash
set -e

echo "=== 1. Block Devices & Partitions (lsblk) ==="
lsblk

echo "=== 2. Partition Table Details (fdisk -l) ==="
sudo fdisk -l 2>/dev/null || fdisk -l || true

echo "=== 3. Active Swap Space (swapon --show) ==="
swapon --show

echo "=== 4. Safe Non-Destructive File Read Throughput Test (256 MB temp file) ==="
dd if=/dev/zero of=./test_read.tmp bs=1M count=256 conv=fdatasync
dd if=./test_read.tmp of=/dev/null bs=1M
rm -f ./test_read.tmp
echo "Temporary test file cleaned up."
