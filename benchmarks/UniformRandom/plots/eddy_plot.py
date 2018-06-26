import pandas as pd
import matplotlib.pyplot as plt
import matplotlib as mpl

mpl.rcParams['hatch.linewidth'] = 2.0 # Changes hatch line width


BASE_DIR = '../ResultsEddy/'
# 1-10 full scan
# 11 - 20 cracking avl
# 21 - 30 b+ tree
# 31 - 40 cracking kd
# 41 - 50 kd-tree


def reset_plot():
    plt.cla()
    plt.clf()
    plt.close()

def translate_alg(alg):
    if alg == 'fs':
        return 'Full Scan'
    if alg == 'stdavl':
        return 'Database Cracking'
    if alg == 'fibpt':
        return 'Full Index B+Tree'
    if alg == 'stdkd':
        return 'Cracking KDTree'
    if alg == 'fikd':
        return 'Full Index KDTree'
    return alg

def get_me_the_average(folders):
    average = 0.0
    for f in folders:
        df1 = pd.read_csv(BASE_DIR + str(f) + '/results.csv', sep=';')
        average += df1['total_time'].sum()
    return average/len(folders)

def total_time_8_cols():
    full_scan = xrange(1, 11)
    db_cracking = xrange(11, 21)
    full_kd = xrange(41, 51)

    algs = ('Full Scan', 'Database Cracking', 'KD-Tree')
    hatchs = ['//', '\\\\', '|']
    pos = [0, 1, 2]

    performances = [
        get_me_the_average(full_scan),
        get_me_the_average(db_cracking),
        get_me_the_average(full_kd)
    ]

    for p, perf, hatch in zip(pos, performances, hatchs):
        # plt.bar(pos, performances, align='center', alpha = 0.5, hatch=hatch)
        plt.bar(p, perf, align='center', alpha = 0.5, hatch=hatch, color='grey')
    plt.xticks(pos, algs)
    plt.ylabel('Total time (s)')
    plt.title('Total time 8 columns')
    plt.savefig('totaltime_8.pdf')
    reset_plot()

def main():
    total_time_8_cols()
    

if __name__ == '__main__':
    main()