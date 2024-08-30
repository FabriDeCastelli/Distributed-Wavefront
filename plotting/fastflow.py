import os
import utils
import seaborn as sns
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd

# Load and filter data
fastflow_df_base = utils.read_fastflow()
fastflow_df = fastflow_df_base[fastflow_df_base['n'] == 3200]
sequential_df = utils.read_sequential()

# Get the time for the sequential execution of n = 3200
time_3200_sequential = sequential_df[sequential_df['n'] == 3200]['time'].values[0]
base_Fastflow_time = np.mean(fastflow_df[fastflow_df['w'] == 1]['time'].values[0])

# Lists for storing values
ideal_speedup = []
actual_speedup = []
efficiency = []
ideal_scalability = []
actual_scalability = []

# Calculate ideal speedup, actual speedup, efficiency, and scalability
for num_workers in sorted(fastflow_df['w'].unique()):

    ideal_speedup.append(num_workers)
    ideal_scalability.append(num_workers)

    process_times = fastflow_df[fastflow_df['w'] == num_workers]['time'].values[0]
    actual_speedup_value = time_3200_sequential / process_times
    actual_speedup.append(actual_speedup_value)

    efficiency.append(actual_speedup_value / num_workers)
    actual_scalability.append(base_Fastflow_time / process_times)

# Create DataFrames for plotting
speedup_df = pd.DataFrame({
    'Number of Workers': sorted(fastflow_df['w'].astype(int).values),
    'Ideal Speedup': ideal_speedup,
    'Actual Speedup': actual_speedup
})

efficiency_df = pd.DataFrame({
    'Number of Workers': sorted(fastflow_df['w'].astype(int).values),
    'Efficiency': efficiency
})

scalability_df = pd.DataFrame({
    'Number of Workers': sorted(fastflow_df['w'].astype(int).values),
    'Ideal Scalability': ideal_scalability,
    'Actual Scalability': actual_scalability
})

# Set up the plotting environment
sns.set(style="whitegrid")
sns.set_context('paper')

output_dir = "./Fastflow"
os.makedirs(output_dir, exist_ok=True)

# Plot 1: Speedup
plt.figure(figsize=(11, 8))
sns.lineplot(data=speedup_df, x='Number of Workers', y='Ideal Speedup', marker='o', label='Ideal Speedup')
sns.lineplot(data=speedup_df, x='Number of Workers', y='Actual Speedup', marker='o', label='Actual Speedup')
plt.xlabel('Number of Workers', fontsize=15, labelpad=20)
plt.ylabel('Speedup', fontsize=15, labelpad=20)
plt.xticks(speedup_df['Number of Workers'].values, fontsize=15)
plt.yticks(fontsize=15)
plt.title('Fastflow Speedup Analysis (Strong)', fontsize=20, pad=20, fontweight='bold')
plt.legend()
plt.savefig(output_dir + '/Fastflow_strong_speedup.png')
# plt.show()

# Plot 2: Efficiency
plt.figure(figsize=(11, 8))
sns.lineplot(data=efficiency_df, x='Number of Workers', y='Efficiency', marker='o')
plt.xlabel('Number of Workers', fontsize=15, labelpad=20)
plt.ylabel('Efficiency', fontsize=15, labelpad=20)
plt.xticks(efficiency_df['Number of Workers'].values, fontsize=15)
plt.yticks(fontsize=15)
plt.title('Fastflow Efficiency Analysis (Strong)', fontsize=20, pad=20, fontweight='bold')
plt.savefig(output_dir + '/Fastflow_strong_efficiency.png')
# plt.show()

# Plot 3: Scalability
plt.figure(figsize=(11, 8))
sns.lineplot(data=scalability_df, x='Number of Workers', y='Ideal Scalability', marker='o')
sns.lineplot(data=scalability_df, x='Number of Workers', y='Actual Scalability', marker='o')
plt.xlabel('Number of Workers', fontsize=15, labelpad=20)
plt.ylabel('Scalability', fontsize=15, labelpad=20)
plt.xticks(scalability_df['Number of Workers'].values, fontsize=15)
plt.yticks(fontsize=15)
plt.title('Fastflow Scalability Analysis (Strong)', fontsize=20, pad=20, fontweight='bold')
plt.savefig(output_dir + '/Fastflow_strong_scalability.png')
# plt.show()


# weak scalability analysis
fastflow_df = utils.read_fastflow()

# Lists for storing values
ideal_speedup = []
actual_speedup = []
efficiency = []
ideal_scalability = []
actual_scalability = []

# Calculate ideal speedup, actual speedup, efficiency, and scalability
workers = [1, 2, 4, 8, 16]
for num_workers in workers:

    problem_size = num_workers * 200
    sequential_time = sequential_df[sequential_df['n'] == problem_size]['time'].values[0]
    ideal_speedup.append(num_workers)
    ideal_scalability.append(num_workers)

    mean_process_time = fastflow_df[(fastflow_df['w'] == num_workers) & (fastflow_df['n'] == problem_size)]['time'].values[0]
    actual_speedup_value = sequential_time / mean_process_time
    actual_speedup.append(actual_speedup_value)
    efficiency.append(actual_speedup_value / num_workers)

    Fastflow_base_time_psize = fastflow_df[(fastflow_df['w'] == 1) & (fastflow_df['n'] == problem_size)]['time'].values[0]
    actual_scalability.append(Fastflow_base_time_psize / mean_process_time)


# Create DataFrames for plotting
speedup_df = pd.DataFrame({
    'Number of Workers': workers,
    'Ideal Speedup': ideal_speedup,
    'Actual Speedup': actual_speedup
})

efficiency_df = pd.DataFrame({
    'Number of Workers': workers,
    'Efficiency': efficiency
})

scalability_df = pd.DataFrame({
    'Number of Workers': workers,
    'Ideal Scalability': ideal_scalability,
    'Actual Scalability': actual_scalability
})

# Set up the plotting environment
sns.set(style="whitegrid")
sns.set_context('paper')

# Plot 1: Speedup
plt.figure(figsize=(11, 8))
sns.lineplot(data=speedup_df, x='Number of Workers', y='Ideal Speedup', marker='o', label='Ideal Speedup')
sns.lineplot(data=speedup_df, x='Number of Workers', y='Actual Speedup', marker='o', label='Actual Speedup')
plt.xlabel('Number of Workers', fontsize=15, labelpad=20)
plt.ylabel('Speedup', fontsize=15, labelpad=20)
plt.xticks(speedup_df['Number of Workers'].values, fontsize=15)
plt.yticks(fontsize=15)
plt.title('Fastflow Speedup Analysis (Weak)', fontsize=20, pad=20, fontweight='bold')
plt.savefig(output_dir + '/Fastflow_weak_speedup.png')
plt.show()

# Plot 2: Efficiency
plt.figure(figsize=(11, 8))
sns.lineplot(data=efficiency_df, x='Number of Workers', y='Efficiency', marker='o')
plt.xlabel('Number of Workers', fontsize=15, labelpad=20)
plt.ylabel('Efficiency', fontsize=15, labelpad=20)
plt.xticks(efficiency_df['Number of Workers'].values, fontsize=15)
plt.yticks(fontsize=15)
plt.title('Fastflow Efficiency Analysis (Weak)', fontsize=20, pad=20, fontweight='bold')
plt.savefig(output_dir + '/Fastflow_weak_efficiency.png')
plt.show()

# Plot 3: Scalability
plt.figure(figsize=(11, 8))
sns.lineplot(data=scalability_df, x='Number of Workers', y='Ideal Scalability', marker='o')
sns.lineplot(data=scalability_df, x='Number of Workers', y='Actual Scalability', marker='o')
plt.xlabel('Number of Workers', fontsize=15, labelpad=20)
plt.ylabel('Scalability', fontsize=15, labelpad=20)
plt.xticks(scalability_df['Number of Workers'].values, fontsize=15)
plt.yticks(fontsize=15)
plt.title('Fastflow Scalability Analysis (Weak)', fontsize=20, pad=20, fontweight='bold')
plt.savefig(output_dir + '/Fastflow_weak_scalability.png')
plt.show()

