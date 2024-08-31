import os
import utils
import seaborn as sns
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd

# Load and filter data
mpi_df_base = utils.read_MPI()
mpi_df = mpi_df_base[(mpi_df_base['nodes'] == 2) & (mpi_df_base['n'] == 3200)]
sequential_df = utils.read_sequential()
sequential_df = sequential_df.groupby('n').mean().reset_index()

# Get the time for the sequential execution of n = 3200
time_3200_sequential = sequential_df[sequential_df['n'] == 3200]['time'].values[0]
base_mpi_time = np.mean(np.vstack(mpi_df[mpi_df['p'] == 2]['time'].values), axis=0)
base_mpi_time = np.mean(base_mpi_time)

# Lists for storing values
ideal_speedup = []
actual_speedup = []
efficiency = []
ideal_scalability = []
actual_scalability = []

# Calculate ideal speedup, actual speedup, efficiency, and scalability
for num_processes in sorted(mpi_df['p'].unique()):

    ideal_speedup.append(num_processes)
    ideal_scalability.append(num_processes)

    process_times = np.mean(np.vstack(mpi_df[mpi_df['p'] == num_processes]['time'].values), axis=0)
    mean_process_time = np.mean(process_times)
    actual_speedup_value = time_3200_sequential / mean_process_time
    actual_speedup.append(actual_speedup_value)

    efficiency.append(actual_speedup_value / num_processes)
    actual_scalability.append(base_mpi_time / mean_process_time)

# Create DataFrames for plotting
speedup_df = pd.DataFrame({
    'Number of Processes': sorted(mpi_df['p'].astype(int).unique()),
    'Ideal Speedup': ideal_speedup,
    'Actual Speedup': actual_speedup
})

efficiency_df = pd.DataFrame({
    'Number of Processes': sorted(mpi_df['p'].astype(int).unique()),
    'Efficiency': efficiency
})

scalability_df = pd.DataFrame({
    'Number of Processes': sorted(mpi_df['p'].astype(int).unique()),
    'Ideal Scalability': ideal_scalability,
    'Actual Scalability': actual_scalability
})

# Set up the plotting environment
sns.set(style="whitegrid")
sns.set_context('paper')

output_dir = "./MPI"
os.makedirs(output_dir, exist_ok=True)

# Plot 1: Speedup
plt.figure(figsize=(11, 8))
sns.lineplot(data=speedup_df, x='Number of Processes', y='Ideal Speedup', marker='o', label='Ideal Speedup')
sns.lineplot(data=speedup_df, x='Number of Processes', y='Actual Speedup', marker='o', label='Actual Speedup')
plt.xlabel('Number of Processes', fontsize=15, labelpad=20)
plt.ylabel('Speedup', fontsize=15, labelpad=20)
plt.xticks(speedup_df['Number of Processes'].values, fontsize=15)
plt.yticks(fontsize=15)
plt.title('MPI Speedup Analysis (Strong)', fontsize=20, pad=20, fontweight='bold')
plt.legend()
plt.savefig(output_dir + '/MPI_strong_speedup.png')
# plt.show()

# Plot 2: Efficiency
plt.figure(figsize=(11, 8))
sns.lineplot(data=efficiency_df, x='Number of Processes', y='Efficiency', marker='o')
plt.xlabel('Number of Processes', fontsize=15, labelpad=20)
plt.ylabel('Efficiency', fontsize=15, labelpad=20)
plt.xticks(efficiency_df['Number of Processes'].values, fontsize=15)
plt.yticks(fontsize=15)
plt.title('MPI Efficiency Analysis (Strong)', fontsize=20, pad=20, fontweight='bold')
plt.savefig(output_dir + '/MPI_strong_efficiency.png')
# plt.show()

# Plot 3: Scalability
plt.figure(figsize=(11, 8))
sns.lineplot(data=scalability_df, x='Number of Processes', y='Ideal Scalability', marker='o')
sns.lineplot(data=scalability_df, x='Number of Processes', y='Actual Scalability', marker='o')
plt.xlabel('Number of Processes', fontsize=15, labelpad=20)
plt.ylabel('Scalability', fontsize=15, labelpad=20)
plt.xticks(scalability_df['Number of Processes'].values, fontsize=15)
plt.yticks(fontsize=15)
plt.title('MPI Scalability Analysis (Strong)', fontsize=20, pad=20, fontweight='bold')
plt.savefig(output_dir + '/MPI_strong_scalability.png')
# plt.show()


# weak scalability analysis
mpi_df = utils.read_MPI()
mpi_df = mpi_df[mpi_df['nodes'] == 2]

# Lists for storing values
ideal_speedup = []
actual_speedup = []
efficiency = []
ideal_scalability = []
actual_scalability = []

# Calculate ideal speedup, actual speedup, efficiency, and scalability
processes = [2, 4, 8, 16]
for num_processes in processes:

    problem_size = num_processes*200
    sequential_time = sequential_df[sequential_df['n'] == problem_size]['time'].values[0]
    ideal_speedup.append(num_processes)
    ideal_scalability.append(num_processes)

    mean_process_time = np.mean(np.vstack(mpi_df[(mpi_df['p'] == num_processes) & (mpi_df['n'] == problem_size)]['time'].values), axis=0)
    mean_process_time = np.mean(mean_process_time)
    actual_speedup_value = sequential_time / mean_process_time
    actual_speedup.append(actual_speedup_value)
    efficiency.append(actual_speedup_value / num_processes)

    mpi_base_time_psize = np.mean(np.vstack(mpi_df[(mpi_df['p'] == 2) & (mpi_df['n'] == problem_size)]['time'].values), axis=0)
    mpi_base_time_psize = np.mean(mpi_base_time_psize)
    actual_scalability.append(mpi_base_time_psize / mean_process_time)


# Create DataFrames for plotting
speedup_df = pd.DataFrame({
    'Number of Processes': processes,
    'Ideal Speedup': ideal_speedup,
    'Actual Speedup': actual_speedup
})

efficiency_df = pd.DataFrame({
    'Number of Processes': processes,
    'Efficiency': efficiency
})

scalability_df = pd.DataFrame({
    'Number of Processes': processes,
    'Ideal Scalability': ideal_scalability,
    'Actual Scalability': actual_scalability
})

# Set up the plotting environment
sns.set(style="whitegrid")
sns.set_context('paper')

# Plot 1: Speedup
plt.figure(figsize=(11, 8))
sns.lineplot(data=speedup_df, x='Number of Processes', y='Ideal Speedup', marker='o', label='Ideal Speedup')
sns.lineplot(data=speedup_df, x='Number of Processes', y='Actual Speedup', marker='o', label='Actual Speedup')
plt.xlabel('Number of Processes', fontsize=15, labelpad=20)
plt.ylabel('Speedup', fontsize=15, labelpad=20)
plt.xticks(speedup_df['Number of Processes'].values, fontsize=15)
plt.yticks(fontsize=15)
plt.title('MPI Speedup Analysis (Weak)', fontsize=20, pad=20, fontweight='bold')
plt.savefig(output_dir + '/MPI_weak_speedup.png')
plt.show()

# Plot 2: Efficiency
plt.figure(figsize=(11, 8))
sns.lineplot(data=efficiency_df, x='Number of Processes', y='Efficiency', marker='o')
plt.xlabel('Number of Processes', fontsize=15, labelpad=20)
plt.ylabel('Efficiency', fontsize=15, labelpad=20)
plt.xticks(efficiency_df['Number of Processes'].values, fontsize=15)
plt.yticks(fontsize=15)
plt.title('MPI Efficiency Analysis (Weak)', fontsize=20, pad=20, fontweight='bold')
plt.savefig(output_dir + '/MPI_weak_efficiency.png')
plt.show()

# Plot 3: Scalability
plt.figure(figsize=(11, 8))
sns.lineplot(data=scalability_df, x='Number of Processes', y='Ideal Scalability', marker='o')
sns.lineplot(data=scalability_df, x='Number of Processes', y='Actual Scalability', marker='o')
plt.xlabel('Number of Processes', fontsize=15, labelpad=20)
plt.ylabel('Scalability', fontsize=15, labelpad=20)
plt.xticks(scalability_df['Number of Processes'].values, fontsize=15)
plt.yticks(fontsize=15)
plt.title('MPI Scalability Analysis (Weak)', fontsize=20, pad=20, fontweight='bold')
plt.savefig(output_dir + '/MPI_weak_scalability.png')
plt.show()

