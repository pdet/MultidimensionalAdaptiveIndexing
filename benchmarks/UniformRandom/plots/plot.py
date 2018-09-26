import os
abspath = os.path.abspath(__file__)
dname = os.path.dirname(abspath)
os.chdir(dname)

import pandas as pd
import matplotlib.pyplot as plt
import matplotlib as mpl
import numpy as np
import itertools

# index_creation;index_lookup;scan_time;join_time;projection_time;total_time

mpl.rcParams['hatch.linewidth'] = 2.0 # Changes hatch line width


BASE_DIR = '../../../Results/'
NR = 10

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
    df_fixed['total_time'] = df['index_creation'] + df['index_lookup'] + df['scan_time'] + df['join_time']
    
    return df_fixed

def average_df(dir_list):
    df = pd.read_csv(BASE_DIR + str(dir_list[0]) + '/results.csv', sep=';')
    alg = df['algorithm']
    df = df.drop(columns=['algorithm', 'projection_time'])

    for f in dir_list[1:]:
        df1 = pd.read_csv(BASE_DIR + str(f) + '/results.csv', sep=';')
        df1 = df1.drop(columns=['algorithm'])
        df = df.add(df1, fill_value = 0)/2.0
    df['algorithm'] = alg
    return fix_total_time(df)

full_scan = {
    2: average_df(range(NR * 0 + 1, NR * 1)),
    4: average_df(range(NR * 6 + 1, NR * 7)),
    8: average_df(range(NR * 12 + 1, NR * 13)),
    16: average_df(range(NR * 18 + 1, NR * 19))
}

std_cracking = {
    2: average_df(range(NR * 1 + 1, NR * 2)),
    4: average_df(range(NR * 7 + 1, NR * 8)),
    8: average_df(range(NR * 13 + 1, NR * 14)),
    16: average_df(range(NR * 19 + 1, NR * 20))
}

cracking_kd = {
    2: average_df(range(NR * 2 + 1, NR * 3)),
    4: average_df(range(NR * 8 + 1, NR * 9)),
    8: average_df(range(NR * 14 + 1, NR * 15)),
    16: average_df(range(NR * 20 + 1, NR * 21))
}

full_kd = {
    2: average_df(range(NR * 3 + 1, NR * 4)),
    4: average_df(range(NR * 9 + 1, NR * 10)),
    8: average_df(range(NR * 15 + 1, NR * 16)),
    16: average_df(range(NR * 21 + 1, NR * 22))
}

sideways = {
    2: average_df(range(NR * 4 + 1, NR * 5)),
    4: average_df(range(NR * 10 + 1, NR * 11)),
    8: average_df(range(NR * 16 + 1, NR * 17)),
    16: average_df(range(NR * 22 + 1, NR * 23))
}

quasii = {
    2: average_df(range(NR * 5 + 1, NR * 6)),
    4: average_df(range(NR * 11 + 1, NR * 12)),
    8: average_df(range(NR * 17 + 1, NR * 18)),
    16: average_df(range(NR * 23 + 1, NR * 24))
}

def reset_plot():
    plt.cla()
    plt.clf()
    plt.close()

def time_breakdown(dfs, column):
    # index_creation;index_lookup;scan_time;join_time;projection_time
    i_c, i_l, s_t, j_t, p_t = [], [], [], [], [] # this is ugly as hell
    names = []
    
    ind =  np.linspace(0, 0.25, num=len(dfs))

    for df in dfs:
        names.append(translate_alg(df[column]['algorithm'][0]))
        i_c.append(df[column]['index_creation'].sum())
        i_l.append(df[column]['index_lookup'].sum())
        s_t.append(df[column]['scan_time'].sum())
        # j_t.append(df[column]['join_time'].sum())

    i_c = np.array(i_c)
    i_l = np.array(i_l)
    s_t = np.array(s_t)
    # j_t = np.array(j_t)

    p_i_c = plt.bar(ind, i_c, 0.1)
    p_i_l = plt.bar(ind, i_l, 0.1, bottom=i_c)
    p_s_t = plt.bar(ind, s_t, 0.1, bottom=i_c + i_l)
    # p_j_t = plt.bar(ind, j_t, 0.1, bottom=i_c + i_l + s_t)

    plt.ylabel('Time (s)')
    plt.xticks(ind, names, rotation=0)
    plt.title('Time Breakdown (' + str(column) + ' columns)')
    # plt.legend((p_i_c, p_i_l, p_s_t, p_j_t), ('Index Creation', 'Index Lookup', 'Scan Time', 'Join Time'))
    plt.legend((p_i_c, p_i_l, p_s_t), ('Index Creation', 'Index Lookup', 'Scan Time'))
    plt.tight_layout()
    plt.savefig('breakdown-' + str(column) + '.pdf')
    reset_plot()

def response_time_per_query(dfs, column):
    for df_hash in dfs:
        name = ''
        
        name = df_hash[column]['algorithm'][0]
        times = df_hash[column]['total_time'][5:]
        plt.plot(
            range(len(times)),
            times,
            label=translate_alg(name)
        )
        plt.legend(loc=0)

    plt.ylabel('Response time (s)')
    plt.xlabel('Query (#)')
    plt.title('Response Time per Query (' + str(column) + ' columns)')
    plt.savefig('query' + str(column) + '.pdf')
    reset_plot()

def response_time_all_columns(dfs, file_name, attribute):
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
            times.append(df_hash[k][attribute].sum())
            name = df_hash[k]['algorithm'][0]
        plt.plot(
            sorted(df_hash.keys()),
            times,
            label=translate_alg(name),
            marker=marker.next(),
            linewidth=2.0
        )
        plt.legend(loc=0)
    
    plt.ylabel('Response time (s)')
    plt.xlabel('Number of Columns')
    plt.title('Total Response Time (' + attribute + ')')
    plt.savefig(file_name + '-' + attribute + '.pdf')
    reset_plot()

def accumulated_response_time_with_prediction(dfs, column):
    for df_hash in dfs:
        name = ''

        name = df_hash[column]['algorithm'][0]
        avg = np.average(df_hash[column]['total_time'][900:])
        times = np.concatenate(
            (np.array(df_hash[column]['total_time']), np.full(3000, avg))
        )
        times = np.cumsum(times)
        plt.plot(
            range(len(times)),
            times,
            label=translate_alg(name)
        )
        plt.legend(loc=0)

    plt.ylabel('Acc. Response time (s)')
    plt.xlabel('Query (#)')
    plt.title('Accumulated Response Time (' + str(column) + ' columns)')
    plt.axvline(x=1000, linestyle='dashed', color='grey')
    plt.savefig('pred-acc-query' + str(column) + '.pdf')
    reset_plot()

def accumulated_response_time(dfs, column):
    for df_hash in dfs:
        name = ''

        name = df_hash[column]['algorithm'][0]
        times = np.array(df_hash[column]['total_time'])
        times = np.cumsum(times)
        plt.plot(
            range(len(times)),
            times,
            label=translate_alg(name)
        )
        plt.legend(loc=0)

    plt.ylabel('Acc. Response time (s)')
    plt.xlabel('Query (#)')
    plt.title('Accumulated Response Time (' + str(column) + ' columns)')
    plt.savefig('acc-query' + str(column) + '.pdf')
    reset_plot()

def stackplot_per_query(df, column):
    df = df[column]

    name = df['algorithm'][0]

    x = range(len(df['total_time'][5:]))
    y1 = np.array(df['index_creation'][5:])
    y2 = np.array(df['index_lookup'][5:])
    y3 = np.array(df['scan_time'][5:])

    labels = ["Index Creation ", "Index Lookup", "Scan Time"]

    plt.stackplot(x, y1, y2, y3, labels=labels)
    plt.legend(loc='upper left')
    plt.savefig(str(name) + 'stack' + str(column) + '.pdf')
    reset_plot()

def values(dfs):
    for df_hash in dfs:
        for k in sorted(df_hash.keys()):
            print(
                translate_alg(df_hash[k]['algorithm'][0]),
                k,
                df_hash[k]['total_time'].sum()
            )

def experiment1():
    # response_time_all_columns([cracking_kd, full_kd, quasii, full_scan, std_cracking, sideways], 'all_r_s', 'total_time')
    # response_time_all_columns([cracking_kd, full_kd, quasii], 'r_s', 'total_time')
    # response_time_all_columns([cracking_kd, full_kd, quasii], 'r_s', 'index_creation')
    # response_time_all_columns([cracking_kd, full_kd, quasii], 'r_s', 'scan_time')
    
    # time_breakdown([cracking_kd, full_kd, quasii], 2)
    # time_breakdown([cracking_kd, full_kd, quasii], 4)
    # time_breakdown([cracking_kd, full_kd, quasii], 8)
    # time_breakdown([cracking_kd, full_kd, quasii], 16)

    # response_time_per_query([cracking_kd,  quasii], 2)
    # response_time_per_query([cracking_kd,  quasii], 4)
    # response_time_per_query([cracking_kd,  quasii], 8)
    # response_time_per_query([cracking_kd,  quasii], 16)

    # accumulated_response_time([cracking_kd,  quasii], 2)
    # accumulated_response_time([cracking_kd,  quasii], 4)
    # accumulated_response_time([cracking_kd,  quasii], 8)
    # accumulated_response_time([cracking_kd,  quasii], 16)

    # accumulated_response_time_with_prediction([cracking_kd,  quasii], 2)
    # accumulated_response_time_with_prediction([cracking_kd,  quasii], 4)
    # accumulated_response_time_with_prediction([cracking_kd,  quasii], 8)
    # accumulated_response_time_with_prediction([cracking_kd,  quasii], 16)

    stackplot_per_query(cracking_kd, 2)
    stackplot_per_query(cracking_kd, 4)
    stackplot_per_query(cracking_kd, 8)
    stackplot_per_query(cracking_kd, 16)

    stackplot_per_query(quasii, 2)
    stackplot_per_query(quasii, 4)
    stackplot_per_query(quasii, 8)
    stackplot_per_query(quasii, 16)

def main():
    experiment1()
    # values([cracking_kd, full_kd, quasii, sideways])
    


if __name__ == '__main__':
    main()