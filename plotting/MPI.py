import os
import sys
import utils
import seaborn as sns
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd

# Get a parameter from the command line
if len(sys.argv) != 2:
    print("Usage: python MPI.py <number of nodes>")
    sys.exit()

nodes = int(sys.argv[1])

# Load and filter data
mpi_df_base = utils.read_MPI()
sequential_df = utils.read_sequential()
sequential_df = sequential_df.groupby('n').mean().reset_index()

# List of matrix sizes
matrix_sizes = [200, 400, 800, 1600, 3200]

# Dictionaries to store the results
results = {
    'speedup': {},
    'efficiency': {},
    'scalability': {}
}

# Calculate metrics for each matrix size
for n in matrix_sizes:
    mpi_df = mpi_df_base[(mpi_df_base['nodes'] == nodes) & (mpi_df_base['n'] == n)]
    time_sequential = sequential_df[sequential_df['n'] == n]['time'].values[0]
    # base_mpi_time = np.mean(np.vstack(mpi_df[mpi_df['p'] == 2]['time'].values), axis=0)
    # base_mpi_time = np.mean(base_mpi_time)

    speedup = []
    efficiency = []
    scalability = []

    for num_processes in sorted(mpi_df['p'].astype(int).unique()):
        process_times = np.mean(np.vstack(mpi_df[mpi_df['p'] == num_processes]['time'].values), axis=0)
        mean_process_time = np.mean(process_times)

        speedup_value = time_sequential / mean_process_time
        speedup.append(speedup_value)
        efficiency.append(speedup_value / num_processes)
        # scalability.append(base_mpi_time / mean_process_time)

    results['speedup'][f'n={n}'] = speedup
    results['efficiency'][f'n={n}'] = efficiency
    results['scalability'][f'n={n}'] = scalability

# Prepare DataFrames for plotting
processes = sorted(mpi_df_base[mpi_df_base['nodes'] == nodes]['p'].astype(int).unique())
speedup_df = pd.DataFrame({'Number of Processes': processes})
efficiency_df = pd.DataFrame({'Number of Processes': processes})
ideal_speedup = [p for p in processes]

speedup_df['Ideal Speedup'] = ideal_speedup
# scalability_df['Ideal Scalability'] = ideal_speedup

for n in matrix_sizes:
    speedup_df[f'Actual Speedup {n}'] = results['speedup'][f'n={n}']
    efficiency_df[f'Efficiency {n}'] = results['efficiency'][f'n={n}']
    # scalability_df[f'Actual Scalability {n}'] = results['scalability'][f'n={n}']

# Set up the plotting environment
sns.set(style="whitegrid")
sns.set_context('paper')

output_dir = "./MPI"
os.makedirs(output_dir, exist_ok=True)


# Function to plot metrics
def plot_metric(df, y_label, title, filename):
    plt.figure(figsize=(11, 8))
    if title != 'Efficiency':
        sns.lineplot(data=df, x='Number of Processes', y=f'Ideal {title}', marker='o', label='Ideal', linewidth=3,
                     markersize=6)
    for n in matrix_sizes:
        column_name = f'{y_label} {n}'
        if column_name in df.columns:
            sns.lineplot(data=df, x='Number of Processes', y=column_name, marker='o', label=f'n={n}', linewidth=3,
                         markersize=6)
        else:
            print(f"Warning: Column {column_name} not found in DataFrame")
    plt.xlabel('Number of Processes', fontsize=15, labelpad=20)
    plt.ylabel(title, fontsize=15, labelpad=20)
    plt.xticks([2 * i for i in range(12)], fontsize=15)
    plt.yticks(fontsize=15)
    if title == 'Efficiency':
        plt.yticks([0.1 * i for i in range(14)], fontsize=15)
        plt.ylim(-0.01)
    plt.title(f'MPI (Strong) {title} with {nodes} nodes', fontsize=20, pad=20, fontweight='bold')
    plt.legend()
    plt.savefig(output_dir + f'/{filename}_{nodes}.png')
    plt.show()


# Plot metrics
plot_metric(speedup_df, 'Actual Speedup', 'Speedup', 'MPI_strong_speedup')
plot_metric(efficiency_df, 'Efficiency', 'Efficiency', 'MPI_strong_efficiency')
# plot_metric(scalability_df, 'Actual Scalability', 'Scalability', 'MPI_strong_scalability')

# weak scalability
mpi_df = utils.read_MPI()

# Nodes to analyze
nodes = [1, 2, 4, 8]

# Prepare dictionaries to store metrics
actual_speedup = {}
efficiency = {}
actual_scalability = {}

# Lists for ideal metrics
ideal_speedup = []
ideal_scalability = []

# Processes to analyze
processes = [2, 4, 8, 16]

# Loop over nodes to calculate the metrics
for node in nodes:
    actual_speedup[node] = []
    efficiency[node] = []
    actual_scalability[node] = []

    mpi_df_node = mpi_df[mpi_df['nodes'] == node]

    for num_processes in processes:

        if node > num_processes:
            continue

        problem_size = num_processes * 200
        sequential_time = sequential_df[sequential_df['n'] == problem_size]['time'].values[0]

        if node == 1:
            ideal_speedup.append(num_processes)
            ideal_scalability.append(num_processes)

        mean_process_time = np.mean(
            np.vstack(
                mpi_df_node[(mpi_df_node['p'] == num_processes) & (mpi_df_node['n'] == problem_size)]['time'].values),
            axis=0
        )
        mean_process_time = np.mean(mean_process_time)

        actual_speedup_value = sequential_time / mean_process_time
        actual_speedup[node].append(actual_speedup_value)
        efficiency[node].append(actual_speedup_value / num_processes)

        base_num_processes = max(node, 2)

        mpi_base_time_psize = np.mean(
            np.vstack(mpi_df_node[(mpi_df_node['p'] == base_num_processes) & (mpi_df_node['n'] == problem_size)][
                          'time'].values), axis=0
        )
        mpi_base_time_psize = np.mean(mpi_base_time_psize)

        actual_scalability[node].append(mpi_base_time_psize / mean_process_time)


# Plotting
def plot_metric(metric_dict, ideal_metric, ylabel, title, filename):
    plt.figure(figsize=(11, 8))
    for node, values in metric_dict.items():
        sns.lineplot(x=processes[-len(values):], y=values, marker='o', label=f' {node} Nodes', linewidth=3,
                     markersize=6)
    if ideal_metric:
        sns.lineplot(x=processes, y=ideal_metric, marker='o', label='Ideal', linewidth=3, markersize=6)
    plt.xlabel('Number of Processes', fontsize=15, labelpad=20)
    plt.ylabel(ylabel, fontsize=15, labelpad=20)
    plt.xticks([2 * i for i in range(9)], fontsize=15)
    plt.ylim(-0.01)
    plt.yticks(fontsize=15)
    if ylabel == 'Efficiency':
        plt.yticks([0.1 * i for i in range(11)], fontsize=15)
    plt.title(title, fontsize=20, pad=20, fontweight='bold')
    plt.savefig(output_dir + '/' + filename)
    plt.show()


# Plot Speedup
plot_metric(actual_speedup, ideal_speedup, 'Speedup', 'MPI (Weak) Speedup', 'MPI_weak_speedup.png')

# Plot Efficiency
plot_metric(efficiency, None, 'Efficiency', 'MPI (Weak) Efficiency', 'MPI_weak_efficiency.png')

# Plot Scalability
plot_metric(actual_scalability, ideal_scalability, 'Scalability', 'MPI (Weak) Scalability', 'MPI_weak_scalability.png')
