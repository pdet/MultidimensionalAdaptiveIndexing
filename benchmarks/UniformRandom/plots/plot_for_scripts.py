import pandas as pd
import matplotlib.pyplot as plt

files = ['fi', 'fsh', 'fsv', 'kd', 'kdf', 'std']
markers=['.', 's', '+', 'D', 'p', '8']
markers_every = [100, 120, 140, 160, 180, 200]
columns = ['indexCreation', 'indexLookup', 'scanTime', 'joinTime', 'totalTime']


def translate_alg(alg):
    if alg == 'fsh':
        return 'Full Scan Horizontal'
    if alg == 'fsv':
        return 'Full Scan Vertical'
    if alg == 'std':
        return 'Standard Cracking AVL'
    if alg == 'fi':
        return 'Full Index B+Tree'
    if alg == 'kd':
        return 'Cracking KDTree'
    if alg == 'kdf':
        return 'Full Index KDTree'
    return alg


for f, mark, me in zip(files, markers, markers_every):
    df = pd.read_csv(f + '.txt', sep=';', header=None)
    df.columns = columns
    ts = df['totalTime'].cumsum().plot(
            label=translate_alg(f), legend=True,
            marker=mark, markevery=me
        )
    ts.legend(loc=2)
ts.set_xlabel('Query (#)')
ts.set_ylabel('Cumulative Time (s)')
ts.set_title('Accumulated Response Time (5 columns)')
plt.savefig('cum_5.pdf')