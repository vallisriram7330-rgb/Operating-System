import matplotlib.pyplot as plt

ref = [1,2,3,4,5,6,1,2,3,7,8,9,1,2,3,4,5,10,11,12,1,2,3,4,5,6,7,8,9,10] * 10

def lru_faults(refs, frames):
    cache, faults = [], 0
    for p in refs:
        if p in cache:
            cache.remove(p)
            cache.append(p)
        else:
            faults += 1
            if len(cache) >= frames:
                cache.pop(0)
            cache.append(p)
    return faults

frames = list(range(2, 14))
faults = [lru_faults(ref, f) for f in frames]

plt.figure(figsize=(8, 4.5))
plt.plot(frames, faults, marker='o', linewidth=2, color='#1b4f72')
plt.axvline(x=5, color='red', linestyle='--', label='Working-Set Knee (~5-6 frames)')
plt.title('Page Faults vs. Allocated Physical Frames (Simulated Medical Trace)')
plt.xlabel('Allocated Physical Frames')
plt.ylabel('Total Page Faults')
plt.grid(True, linestyle=':', alpha=0.7)
plt.legend()
plt.tight_layout()
plt.savefig('q4_faults_vs_frames.png', dpi=150)
print("Saved q4_faults_vs_frames.png")
