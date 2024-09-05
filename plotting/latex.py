import numpy as np
import pandas as pd

import utils

sequential_df = utils.read_sequential()
sequential_df = sequential_df.groupby('n').mean().reset_index()
print(sequential_df.set_index('n').T.to_latex(index=False, float_format="%.2f", header=True))

fastflow_df = utils.read_fastflow()
fastflow_df = fastflow_df.groupby(['n', 'w']).mean().reset_index()
fastflow_df = fastflow_df.pivot(index='w', columns='n', values='time')
print(fastflow_df.to_latex(float_format="%.2f", header=True, column_format='c' * (len(fastflow_df.columns) + 1)))


MPI_df = utils.read_MPI()
result_df = []

for nodes in [1, 2, 4, 8]:
    mpi_df_node = MPI_df[(MPI_df['nodes'] == nodes)]
    for p in mpi_df_node['p'].unique():
        mpi_df_p = mpi_df_node[(mpi_df_node['p'] == p)]
        for n in mpi_df_p['n'].unique():
            mean_time = np.mean(np.vstack(mpi_df_p[mpi_df_p['n'] == n]['time']), axis=0)
            mean_time = np.mean(mean_time)
            result_df.append({
                'Nodes': nodes,
                'p': p,
                'n': n,
                'time (ms)': mean_time
            })

result_df = pd.DataFrame(result_df)

for nodes in [1, 2, 4, 8]:
    result_df_p = result_df[(result_df['Nodes'] == nodes)]
    print(result_df_p.pivot(index='p', columns='n', values='time (ms)').to_latex(index=True, float_format="%.2f", header=True, column_format='c' * (len(result_df_p['n'].unique()) + 1)))

