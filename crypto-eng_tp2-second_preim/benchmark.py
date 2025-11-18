#!/usr/bin/env python3
import subprocess
import time
import matplotlib.pyplot as plt

# Disclaimer: Specifically this file was created using claude sonnet 4.5 
# because it was pretty simple and not part of the questions we thought this would be fine.
# no other code used generative ai

NUM_RUNS = 30
NUM_THREADS = 12


def run_benchmark(binary_path, num_runs):
    times = []
    print(f"Running {binary_path} {num_runs} times...")
    for i in range(num_runs):
        start = time.time()
        subprocess.run([binary_path])
        times.append(time.time() - start)
        print(f"  Run {i+1}: {times[-1]:.2f}s")
    return times


single_times = run_benchmark("./attack_single", NUM_RUNS)
multi_times = run_benchmark("./attack_multi", NUM_RUNS)


with open('timing_data.csv', 'w') as f:
    f.write("run,single_threaded,multi_threaded\n")

    for i in range(NUM_RUNS):
        f.write(f"{i},{single_times[i]:.4f},{multi_times[i]:.4f}\n")


fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(12, 5))
ax1.boxplot([single_times, multi_times], labels=['Single-threaded', 'Multi-threaded'])
ax1.set_ylabel('Time in seconds')
ax1.set_title('Execution Time Comparison')


multi_normalized = [t / NUM_THREADS for t in multi_times]
ax2.boxplot([single_times, multi_normalized], labels=['Single-threaded', f'Multi / {NUM_THREADS}'])
ax2.set_ylabel('Time in seconds')
ax2.set_title('Normalized by Thread Count')


plt.tight_layout()
plt.savefig('benchmark.pdf')
plt.show()