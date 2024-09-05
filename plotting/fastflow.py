import os
import utils
import seaborn as sns
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd

# Load and filter data
fastflow_df_base = utils.read_fastflow()
fastflow_df_base = fastflow_df_base.groupby(['n', 'w']).mean().reset_index()

matrix_sizes = [200, 400, 800, 1600, 3200]

# Dictionaries to store the results
results = {
    'speedup': {},
    'efficiency': {},
    'scalability': {}
}

for n in matrix_sizes:
    fastflow_df = fastflow_df_base[fastflow_df_base['n'] == n]
    sequential_df = utils.read_sequential()
    sequential_df = sequential_df.groupby('n').mean().reset_index()

    time_sequential = sequential_df[sequential_df['n'] == n]['time'].values[0]

    ideal_speedup = []
    actual_speedup = []
    efficiency = []
    ideal_scalability = []
    actual_scalability = []

    for num_workers in sorted(fastflow_df['w'].unique()):

        ideal_speedup.append(num_workers)
        ideal_scalability.append(num_workers)

        process_times = fastflow_df[fastflow_df['w'] == num_workers]['time'].values[0]
        actual_speedup_value = time_sequential / process_times
        actual_speedup.append(actual_speedup_value)
        efficiency.append(actual_speedup_value / num_workers)

        base_fastflow_time = np.mean(fastflow_df[fastflow_df['w'] == 1]['time'].values[0])
        actual_scalability.append(base_fastflow_time / process_times)

    results['speedup'][f'n={n}'] = actual_speedup
    results['efficiency'][f'n={n}'] = efficiency
    results['scalability'][f'n={n}'] = actual_scalability


# Prepare DataFrames for plotting
workers = sorted(fastflow_df_base['w'].unique())
speedup_df = pd.DataFrame({'Number of Workers': workers, 'Ideal Speedup': [p for p in workers]})
efficiency_df = pd.DataFrame({'Number of Workers': workers})
scalability_df = pd.DataFrame({'Number of Workers': workers, 'Ideal Scalability': [p for p in workers]})

# Calculate metrics for each matrix size
for n in matrix_sizes:
    speedup_df[f'Actual Speedup {n}'] = results['speedup'][f'n={n}']
    efficiency_df[f'Efficiency {n}'] = results['efficiency'][f'n={n}']
    scalability_df[f'Actual Scalability {n}'] = results['scalability'][f'n={n}']


# Set up the plotting environment
sns.set(style="whitegrid")
sns.set_context('paper')

output_dir = "./Fastflow"
os.makedirs(output_dir, exist_ok=True)

def plot_metric(df, y_label, title, output_file):
    plt.figure(figsize=(11, 8))

    if title != 'Efficiency':
        sns.lineplot(data=df, x='Number of Workers', y=f'Ideal {title}', marker='o', label='Ideal', linewidth=3, markersize=6)
    for n in matrix_sizes:
        column_name = f'{y_label} {n}'
        if column_name in df.columns:
            sns.lineplot(data=df, x='Number of Workers', y=column_name, marker='o', label=f'n={n}', linewidth=3, markersize=6)
        else:
            print(f'Column {column_name} not found in DataFrame')

    plt.xlabel('Number of Workers', fontsize=15, labelpad=20)
    plt.ylabel(title, fontsize=15, labelpad=20)
    plt.xticks([2*i for i in range(12)], fontsize=15)
    if title == 'Efficiency':
        plt.yticks([0.1*i for i in range(12)], fontsize=15)
        plt.ylim(-0.01)
    else:
        plt.yticks(fontsize=15)
    plt.title(f'Fastflow ({title})', fontsize=20, pad=20, fontweight='bold')
    plt.legend()
    plt.savefig(output_dir + '/' + output_file)
    plt.show()

plot_metric(speedup_df, 'Actual Speedup', 'Speedup', 'Fastflow_speedup.png')
plot_metric(efficiency_df, 'Efficiency', 'Efficiency', 'Fastflow_efficiency.png')
plot_metric(scalability_df, 'Actual Scalability', 'Scalability', 'Fastflow_scalability.png')

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
sns.lineplot(data=speedup_df, x='Number of Workers', y='Ideal Speedup', marker='o', label='Ideal', linewidth=3, markersize=6)
sns.lineplot(data=speedup_df, x='Number of Workers', y='Actual Speedup', marker='o', label='Actual', linewidth=3, markersize=6)
plt.xlabel('Number of Workers', fontsize=15, labelpad=20)
plt.ylabel('Speedup', fontsize=15, labelpad=20)
plt.ylim(0)
plt.xticks([2*i for i in range(9)], fontsize=15)
plt.yticks(fontsize=15)
plt.title('Fastflow (Weak) Speedup', fontsize=20, pad=20, fontweight='bold')
plt.savefig(output_dir + '/Fastflow_weak_speedup.png')
plt.show()

# Plot 2: Efficiency
plt.figure(figsize=(11, 8))
sns.lineplot(data=efficiency_df, x='Number of Workers', y='Efficiency', marker='o', linewidth=3, markersize=6)
plt.xlabel('Number of Workers', fontsize=15, labelpad=20)
plt.ylabel('Efficiency', fontsize=15, labelpad=20)
plt.xticks([2*i for i in range(9)], fontsize=15)
plt.yticks([0.1*i for i in range(11)], fontsize=15)
plt.ylim(0, 1)
plt.title('Fastflow (Weak) Efficiency', fontsize=20, pad=20, fontweight='bold')
plt.savefig(output_dir + '/Fastflow_weak_efficiency.png')
plt.show()

# Plot 3: Scalability
plt.figure(figsize=(11, 8))
sns.lineplot(data=scalability_df, x='Number of Workers', y='Ideal Scalability', label='Ideal', marker='o', linewidth=3, markersize=6)
sns.lineplot(data=scalability_df, x='Number of Workers', y='Actual Scalability', label='Actual', marker='o', linewidth=3, markersize=6)
plt.xlabel('Number of Workers', fontsize=15, labelpad=20)
plt.ylabel('Scalability', fontsize=15, labelpad=20)
plt.xticks([2*i for i in range(9)], fontsize=15)
plt.yticks(fontsize=15)
plt.ylim(0)
plt.title('Fastflow (Weak) Scalability', fontsize=20, pad=20, fontweight='bold')
plt.savefig(output_dir + '/Fastflow_weak_scalability.png')
plt.show()

