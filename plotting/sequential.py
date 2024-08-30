import os
import utils
import seaborn as sns
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd

sequential_df = utils.read_sequential()

# Set up the plotting environment
sns.set(style="whitegrid")
sns.set_context('paper')

output_dir = "./Sequential"
os.makedirs(output_dir, exist_ok=True)

plt.figure(figsize=(11, 8))
sns.lineplot(data=sequential_df, x='n', y='time', marker='o', label='Execution time (ms)')
plt.xlabel('Matrix dimension', fontsize=15, labelpad=20)
plt.ylabel('Time (ms)', fontsize=15, labelpad=20)
plt.xticks(fontsize=15)
plt.yticks(fontsize=15)
plt.title('Sequential Wavefront Execution Time', fontsize=20, pad=20, fontweight='bold')
plt.legend()
plt.savefig(output_dir + '/sequential_time.png')
plt.show()
