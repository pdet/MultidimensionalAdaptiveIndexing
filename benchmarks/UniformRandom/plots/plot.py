import os
abspath = os.path.abspath(__file__)
dname = os.path.dirname(abspath)
os.chdir(dname)

import pandas as pd
import matplotlib.pyplot as plt
import matplotlib as mpl
import numpy as np
import itertools



mpl.rcParams['hatch.linewidth'] = 2.0 # Changes hatch line width


BASE_DIR = '../../../Results/'

# 1-10 full scan
# 11 - 20 cracking avl
# 21 - 30 cracking kd
# 31 - 40 full kd
# 41 - 50 sideways
# 51 - 60 quasii

def translate_alg(alg):
    if alg == 'fs':
        return 'Full Scan'
    if alg == 'stdavl':
        return 'Standard Cracking'
    if alg == 'stdkd':
        return 'Cracking KD-Tree'
    if alg == 'fikd':
        return 'Full KD-Tree'
    if alg == 'swc':
        return 'Sideways Cracking'
    if alg == 'quasii':
        return 'Quasii'
    return alg

def fix_total_time(df):
    df_fixed = df.drop(columns=['total_time'])
    df_fixed['total_time'] = df['index_creation'] + df['index_lookup'] + df['scan_time'] + df['join_time'] + df['projection_time']
    
    return df_fixed

def average_df(dir_list):
    df = pd.read_csv(BASE_DIR + str(dir_list[0]) + '/results.csv', sep=';')
    alg = df['algorithm']
    df = df.drop(columns=['algorithm'])

    for f in dir_list[1:]:
        df1 = pd.read_csv(BASE_DIR + str(f) + '/results.csv', sep=';')
        df1 = df1.drop(columns=['algorithm'])
        df = df.add(df1, fill_value = 0)/2.0
    df['algorithm'] = alg
    return fix_total_time(df)

fs = {
    2: average_df(range(1, 10)),
    4: average_df(range(61, 70)),
    8: average_df(range(121, 130)),
    16: average_df(range(181, 190))
}

c = {
    2: average_df(range(11, 20)),
    4: average_df(range(71, 80)),
    8: average_df(range(131, 140)),
    16: average_df(range(191, 200))
}

c_kd = {
    2: average_df(range(21, 30)),
    4: average_df(range(81, 90)),
    8: average_df(range(141, 150)),
    16: average_df(range(201, 210))
}

fkd = {
    2: average_df(range(31, 40)),
    4: average_df(range(91, 100)),
    8: average_df(range(151, 160)),
    16: average_df(range(211, 220))
}

swc = {
    2: average_df(range(41, 50)),
    4: average_df(range(101, 110)),
    8: average_df(range(161, 170)),
    16: average_df(range(221, 230))
}

q = {
    2: average_df(range(51, 60)),
    4: average_df(range(111, 120)),
    8: average_df(range(171, 180)),
    16: average_df(range(231, 240))
}

def reset_plot():
    plt.cla()
    plt.clf()
    plt.close()

def response_time_per_query():
    dfs = [c_kd, q, fkd]

    for df_hash in dfs:
        name = ''
        
        name = df_hash[16]['algorithm'][0]
        plt.plot(
            range(len(df_hash[16]['total_time'])-50),
            df_hash[16]['total_time'][50:],
            label=translate_alg(name)
        )
        plt.legend(loc=2)

    plt.ylabel('Response time (s)')
    plt.xlabel('Query (#)')
    plt.title('Response Time per Query')
    plt.savefig('query16.pdf')
    reset_plot()

def response_time_all_columns():
    # dfs = [c_kd, fkd, q]
    dfs = [swc, c]

    cols = sorted(fs.keys())
    marker=itertools.cycle(['.', 's', '*', 'D', 'X'])
    # color = itertools.cycle([
    #     (105/255.0,105/255.0,105/255.0),
    #     (128/255.0,128/255.0,128/255.0),
    #     (169/255.0,169/255.0,169/255.0),
    #     (105/255.0,105/255.0,105/255.0),
    #     (128/255.0,128/255.0,128/255.0)
    # ])

    for df_hash in dfs:
        times = []
        name = ''
        for k in sorted(df_hash.keys()):
            times.append(df_hash[k]['total_time'].sum())
            name = df_hash[k]['algorithm'][0]
        plt.plot(
            sorted(df_hash.keys()),
            times,
            label=translate_alg(name),
            marker=marker.next(),
            linewidth=2.0
        )
        plt.legend(loc=2)
    
    plt.ylabel('Response time (s)')
    plt.xlabel('Number of Columns')
    plt.title('Total Response Time')
    plt.savefig('all_response_times.pdf')
    reset_plot()

def main():
    response_time_all_columns()
    response_time_per_query()

if __name__ == '__main__':
    main()