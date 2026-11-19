import subprocess
import matplotlib.pyplot as plt

out = subprocess.check_output(["./bench_replace"], text=True)
frames, loc_fifo, loc_clock, loc_lru = [], [], [], []
for line in out.splitlines():
    parts = [p.strip() for p in line.split("|")]
    if len(parts) == 7 and parts[0].isdigit():
        frames.append(int(parts[0]))
        loc_fifo.append(int(parts[1]))
        loc_clock.append(int(parts[2]))
        loc_lru.append(int(parts[3]))

plt.figure(figsize=(8, 4.5))
plt.plot(frames, loc_fifo, marker='s', label='FIFO')
plt.plot(frames, loc_clock, marker='^', label='Clock (Second-Chance)')
plt.plot(frames, loc_lru, marker='o', label='Exact LRU')
plt.title('Page Faults vs. Frame Count (100,000 Locality References)')
plt.xlabel('Number of Physical Frames')
plt.ylabel('Page Faults')
plt.grid(True, linestyle=':', alpha=0.7)
plt.legend()
plt.tight_layout()
plt.savefig('q5_replacement_comparison.png', dpi=150)
print("Saved q5_replacement_comparison.png")
