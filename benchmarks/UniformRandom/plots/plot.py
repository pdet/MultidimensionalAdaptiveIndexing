import os

abspath = os.path.abspath(__file__)
dname = os.path.dirname(abspath)
os.chdir(dname)

import pandas as pd
import matplotlib.pyplot as plt
import matplotlib as mpl
import numpy as np
import itertools
from scipy.interpolate import spline

# index_creation;index_lookup;scan_time;join_time;projection_time;total_time

# mpl.rcParams['hatch.linewidth'] = 2.0 # Changes hatch line width


BASE_DIR = '../../../Results/'

full_scan = {}

std_cracking = {}

cracking_kd = {}

full_kd = {}

sideways = {}

quasii = {}

FILE_TYPE = '.png'


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


def alg_abbreviation(alg):
    if alg == 'fs':
        return 'F.S.'
    if alg == 'stdavl':
        return 'S.C.'
    if alg == 'stdkd':
        return 'C.KD.'
    if alg == 'fikd':
        return 'F.KD.'
    if alg == 'swc':
        return 'S.W.'
    if alg == 'quasii':
        return 'Q.'
    return alg


def get_hash(alg):
    if alg == 'fs':
        return full_scan
    if alg == 'stdavl':
        return std_cracking
    if alg == 'stdkd':
        return cracking_kd
    if alg == 'fikd':
        return full_kd
    if alg == 'swc':
        return sideways
    if alg == 'quasii':
        return quasii


def remove_join_time(col, hash):
    if col not in hash:
        return
    df = hash[col]
    df['join_time'] = 0
    df['total_time'] = df['index_creation'] + df['index_lookup'] + df['scan_time'] + df['projection_time']
    hash[col] = df


def remove_creation_time(hash):
    for k in hash.keys():
        hash[k]['index_creation'] = 0


def remove_lookup_time(hash):
    for k in hash.keys():
        hash[k]['index_lookup'] = 0


def average_df(df1, df2):
    alg = df1['algorithm']

    df1 = df1.drop(columns=['algorithm'])
    df2 = df2.drop(columns=['algorithm'])

    df1 = df1.add(df2, fill_value=0) / 2.0
    df1['algorithm'] = alg
    return df1


def add_dataframe_to_hash(df):
    hash = get_hash(df['algorithm'][0])
    n_cols = df['number_of_columns'][0]
    if n_cols in hash:
        hash[n_cols] = average_df(hash[n_cols], df)
    else:
        hash[n_cols] = df


def read_files():
    for file_name in os.listdir(BASE_DIR):
        add_dataframe_to_hash(
            pd.read_csv(BASE_DIR + '/' + file_name + '/results.csv', sep=';')
        )
    remove_join_time(1, full_scan)
    remove_join_time(1, std_cracking)
    remove_join_time(1, cracking_kd)
    remove_join_time(1, full_kd)
    remove_join_time(1, sideways)
    remove_join_time(1, quasii)

    remove_creation_time(full_scan)
    remove_lookup_time(full_scan)


def reset_plot():
    plt.cla()
    plt.clf()
    plt.close()


def time_breakdown(dfs, column):
    # index_creation;index_lookup;scan_time;join_time;projection_time
    i_c, i_l, s_t, j_t, p_t = [], [], [], [], []  # this is ugly as hell
    names = []
    ind = np.linspace(0, 1, num=len(dfs))

    for df in dfs:
        offset = '{0:,}'.format(int(df[column]['offsets_size'].mean()))
        names.append(
            translate_alg(df[column]['algorithm'][0]) + '\n' + offset
        )
        i_c.append(df[column]['index_creation'].sum())
        i_l.append(df[column]['index_lookup'].sum())
        s_t.append(df[column]['scan_time'].sum())
        # j_t.append(df[column]['join_time'].sum())

    i_c = np.array(i_c)
    i_l = np.array(i_l)
    s_t = np.array(s_t)
    # j_t = np.array(j_t)

    p_i_c = plt.bar(ind, i_c, 0.1, zorder=3)
    p_i_l = plt.bar(ind, i_l, 0.1, bottom=i_c, zorder=3)
    p_s_t = plt.bar(ind, s_t, 0.1, bottom=i_c + i_l, zorder=3)
    # p_j_t = plt.bar(ind, j_t, 0.1, bottom=i_c + i_l + s_t)

    # plt.yticks(np.arange(0, 32.5, 2.5))
    plt.grid(axis='y', linestyle='--', zorder=0)
    plt.ylabel('Time (s)')
    plt.xticks(ind, names, rotation=-30)
    plt.title('Time Breakdown (' + str(column) + ' columns)')
    # plt.legend((p_i_c, p_i_l, p_s_t, p_j_t), ('Index Creation', 'Index Lookup', 'Scan Time', 'Join Time'))
    plt.legend((p_i_c, p_i_l, p_s_t), ('Index Creation', 'Index Lookup', 'Scan Time'))
    plt.tight_layout()
    plt.savefig('breakdown-' + str(column) + FILE_TYPE)
    reset_plot()


def time_breakdown_with_join(dfs, column):
    # index_creation;index_lookup;scan_time;join_time;projection_time
    i_c, i_l, s_t, j_t, p_t = [], [], [], [], []  # this is ugly as hell
    names = []

    ind = np.linspace(0, 1, num=len(dfs))

    for df in dfs:
        names.append(translate_alg(df[column]['algorithm'][0]))
        i_c.append(df[column]['index_creation'].sum())
        i_l.append(df[column]['index_lookup'].sum())
        s_t.append(df[column]['scan_time'].sum())
        j_t.append(df[column]['join_time'].sum())

    i_c = np.array(i_c)
    i_l = np.array(i_l)
    s_t = np.array(s_t)
    j_t = np.array(j_t)

    p_i_c = plt.bar(ind, i_c, 0.1, zorder=3)
    p_i_l = plt.bar(ind, i_l, 0.1, bottom=i_c, zorder=3)
    p_s_t = plt.bar(ind, s_t, 0.1, bottom=i_c + i_l, zorder=3)
    p_j_t = plt.bar(ind, j_t, 0.1, bottom=i_c + i_l + s_t)

    plt.ylabel('Time (s)')
    plt.xticks(ind, names, rotation=-30)
    plt.title('Time Breakdown (' + str(column) + ' columns)')
    plt.legend((p_i_c, p_i_l, p_s_t, p_j_t), ('Index Creation', 'Index Lookup', 'Scan Time', 'Intersection Time'))
    plt.tight_layout()
    plt.savefig('breakdown-join-' + str(column) + FILE_TYPE)
    reset_plot()


def response_time_per_query(dfs, column):
    for df_hash in dfs:
        name = ''

        name = df_hash[column]['algorithm'][0]
        times = df_hash[column]['total_time'][:1000]
        x = range(len(times))

        # x = np.linspace(x[0], x[-1], 5)
        # times = spline(x, times, x)
        plt.plot(
            x,
            times,
            label=translate_alg(name)
        )
        plt.legend(loc=0)

    # plt.ylim(0, 0.04)
    plt.ylabel('Response time (s)')
    plt.xlabel('Query (#)')
    plt.yscale('log')
    # plt.xscale('log')
    plt.grid(True)
    plt.title('Response Time per Query (' + str(column) + ' columns)')
    plt.savefig('query' + str(column) + FILE_TYPE)
    reset_plot()


def index_info_per_query(df, column):
    df = df[column]

    name = df['algorithm'][0]

    x = range(len(df['total_time']))
    n_nodes = np.array(df['n_nodes'])
    n_partitions = np.array(df['n_partitions'])
    index_height = np.array(df['index_height'])
    tuples_scanned = np.array(df['offsets_size'])
    partitions_scanned = np.array(df['n_offsets'])

    fig = plt.figure()
    ax = fig.add_subplot(111)

    ax.plot(x, n_nodes, label="Number of Nodes")
    ax.plot(x, n_partitions, label="Number of Partitions")
    ax.plot(x, index_height, label="Index Height")
    ax.plot(x, tuples_scanned, label="Tuples Scanned")
    ax.plot(x, partitions_scanned, label="Partitions Scanned")
    ax.set_ylabel('Elapsed time (seconds)')
    ax.set_xlabel('Query (#)')
    ax.set_yscale('log')
    # ax.yaxis.set_major_formatter(mpl.ticker.FormatStrFormatter('%d'))
    # plt.ylim(0, 0.05)
    ax.legend(loc='center left', bbox_to_anchor=(1, 0.5))
    ax.set_title(
        'Index Info per Query (' + translate_alg(name) + ', ' + str(column) + ' columns)')
    plt.tight_layout()
    fig.savefig(str(name) + 'index-info-' + str(column) + FILE_TYPE)
    reset_plot()


def response_time_all_columns(dfs, file_name, cols=[1, 2, 4, 8, 16]):
    marker = itertools.cycle(['.', 's', '*', 'D', 'X'])
    # color = itertools.cycle([
    #     (105/255.0,105/255.0,105/255.0),
    #     (128/255.0,128/255.0,128/255.0),
    #     (169/255.0,169/255.0,169/255.0),
    #     (105/255.0,105/255.0,105/255.0),
    #     (128/255.0,128/255.0,128/255.0)
    # ])

    small_step = 0.1
    width = 0.25
    step = 0.0

    xticks = []
    algs = []

    sel = dfs[0][cols[0]]['query_selectivity'][0]

    for col in cols:
        for df in dfs:
            index_creation = df[col]['index_creation'].sum()
            index_lookup = df[col]['index_lookup'].sum()
            scan_time = df[col]['scan_time'].sum()
            join_time = df[col]['join_time'].sum()

            p_i_c = plt.bar(step, index_creation, width, zorder=3, color='blue')
            p_i_l = plt.bar(step, index_lookup, width, bottom=index_creation, zorder=3, color='orange')
            p_s_t = plt.bar(step, scan_time, width, bottom=index_creation + index_lookup, zorder=3, color='green')
            p_j_t = plt.bar(step, join_time, width, bottom=index_creation + index_lookup + scan_time, zorder=3, color='red')

            plt.text(
                y=int((index_creation + index_lookup + scan_time + join_time) + 2),
                x=step,
                s=str(int(index_creation + index_lookup + scan_time + join_time)),
                verticalalignment='bottom',
                horizontalalignment='center'
            )

            xticks.append(step)
            algs.append(alg_abbreviation(df[col]['algorithm'][0]) + ' (' + str(col) + ')')
            plt.legend((p_i_c, p_i_l, p_s_t, p_j_t), ('Index Creation', 'Index Lookup', 'Scan Time', 'Intersection Time'))

            step += width + small_step
        step += small_step * 2

    plt.ylabel('Response time (s)')
    plt.grid(axis='y', which='both')
    # plt.xlabel('Number of Columns')
    plt.xticks(xticks, algs, rotation=270)
    plt.title('Total Response Time ( ' + str(sel * 100) + '% selectivity)')
    plt.tight_layout()
    plt.savefig(file_name + FILE_TYPE)
    reset_plot()


def accumulated_response_time(dfs, column):
    fig = plt.figure()
    ax = fig.add_subplot(111)

    for df_hash in dfs:
        name = ''

        name = df_hash[column]['algorithm'][0]
        times = np.array(df_hash[column]['total_time'])
        times = np.cumsum(times)
        ax.plot(
            range(len(times)),
            times,
            label=translate_alg(name)
        )
        ax.legend(loc=0)

    ax.set_ylabel('Acc. Response time (s)')
    ax.set_xlabel('Query (#)')
    # ax.set_yscale('log')
    ax.yaxis.set_major_formatter(mpl.ticker.FormatStrFormatter('%d'))
    ax.grid(True)
    ax.set_title('Accumulated Response Time (' + str(column) + ' columns)')
    fig.savefig('acc-query' + str(column) + FILE_TYPE)
    reset_plot()


def stackplot_per_query(df, column):
    df = df[column]

    name = df['algorithm'][0]

    x = range(len(df['total_time']))
    y1 = np.array(df['index_creation'])
    y2 = np.array(df['index_lookup'])
    y3 = np.array(df['scan_time'])
    y4 = np.array(df['join_time'])

    labels = ["Index Creation ", "Index Lookup", "Scan Time", "Intersection Time"]

    fig = plt.figure()
    ax = fig.add_subplot(111)

    ax.stackplot(x, y1, y2, y3, y4, labels=labels)
    ax.set_ylabel('Elapsed time (seconds)')
    ax.set_xlabel('Query (#)')
    ax.set_yscale('log')
    # ax.yaxis.set_major_formatter(mpl.ticker.FormatStrFormatter('%d'))
    # plt.ylim(0, 0.05)
    ax.legend(loc=0)
    ax.set_title(
        'Time Breakdown per Query (' + translate_alg(name) + ', ' + str(column) + ' columns)')
    fig.savefig(str(name) + 'stack' + str(column) + FILE_TYPE)
    reset_plot()


def response_time_bars(dfs, column, title):
    times = []
    names = []
    for df in dfs:
        names.append(
            translate_alg(df[column]['algorithm'][0])
        )
        times.append(
            df[column]['total_time'].sum()
        )
    plt.bar(range(len(names)), times)
    plt.xticks(range(len(names)), names)
    plt.ylabel('Elapsed time (seconds)')
    plt.title('Total response time (' + str(column) + ' columns)')
    plt.savefig(title + FILE_TYPE)
    reset_plot()


def values(dfs):
    for df_hash in dfs:
        for k in sorted(df_hash.keys()):
            print(
                translate_alg(df_hash[k]['algorithm'][0]),
                k,
                df_hash[k]['total_time'].sum()
            )


def average_offsets(dfs):
    for df_hash in dfs:
        for k in sorted(df_hash.keys()):
            print(
                translate_alg(df_hash[k]['algorithm'][0]),
                k,
                df_hash[k]['n_offsets'].mean(),
                df_hash[k]['offsets_size'].mean()
            )


def experiment1():
    response_time_all_columns([full_scan, std_cracking], 'full-vs-cracking')
    response_time_all_columns([full_scan, std_cracking, sideways], 'full-vs-cracking-vs-sideways')

    response_time_all_columns([full_scan, sideways, cracking_kd, full_kd, quasii], 'all_algs')
    response_time_all_columns([cracking_kd, full_kd, quasii, full_scan], 'few_algs')

    time_breakdown([cracking_kd, full_kd, quasii, sideways, full_scan], 1)
    time_breakdown([cracking_kd, full_kd, quasii, sideways, full_scan], 2)
    time_breakdown([cracking_kd, full_kd, quasii, sideways, full_scan], 4)
    time_breakdown([cracking_kd, full_kd, quasii, sideways, full_scan], 8)
    time_breakdown([cracking_kd, full_kd, quasii, sideways, full_scan], 16)

    time_breakdown_with_join([cracking_kd, full_kd, quasii, full_scan, sideways, std_cracking], 1)
    time_breakdown_with_join([cracking_kd, full_kd, quasii, full_scan, sideways, std_cracking], 2)
    time_breakdown_with_join([cracking_kd, full_kd, quasii, full_scan, sideways, std_cracking], 4)
    time_breakdown_with_join([cracking_kd, full_kd, quasii, full_scan, sideways, std_cracking], 8)
    time_breakdown_with_join([cracking_kd, full_kd, quasii, full_scan, sideways, std_cracking], 16)

    response_time_per_query([cracking_kd, full_kd, quasii, full_scan, std_cracking, sideways], 1)
    response_time_per_query([cracking_kd, full_kd, quasii, full_scan, std_cracking, sideways], 2)
    response_time_per_query([cracking_kd, full_kd, quasii, full_scan, std_cracking, sideways], 4)
    response_time_per_query([cracking_kd, full_kd, quasii, full_scan, std_cracking, sideways], 8)
    response_time_per_query([cracking_kd,  quasii, full_kd], 16)

    accumulated_response_time([cracking_kd, full_kd, quasii, full_scan, std_cracking, sideways], 1)
    accumulated_response_time([cracking_kd, full_kd, quasii, full_scan, std_cracking, sideways], 2)
    accumulated_response_time([cracking_kd, full_kd, quasii, full_scan, std_cracking, sideways], 4)
    accumulated_response_time([cracking_kd, full_kd, quasii, full_scan, std_cracking, sideways], 8)
    accumulated_response_time([cracking_kd, full_kd, quasii, full_scan, std_cracking, sideways], 16)

    index_info_per_query(cracking_kd, 1)
    index_info_per_query(cracking_kd, 2)
    index_info_per_query(cracking_kd, 4)
    index_info_per_query(cracking_kd, 8)
    index_info_per_query(cracking_kd, 16)

    index_info_per_query(full_kd, 1)
    index_info_per_query(full_kd, 2)
    index_info_per_query(full_kd, 4)
    index_info_per_query(full_kd, 8)
    index_info_per_query(full_kd, 16)

    index_info_per_query(quasii, 1)
    index_info_per_query(quasii, 2)
    index_info_per_query(quasii, 4)
    index_info_per_query(quasii, 8)
    index_info_per_query(quasii, 16)

    stackplot_per_query(cracking_kd, 1)
    stackplot_per_query(cracking_kd, 2)
    stackplot_per_query(cracking_kd, 4)
    stackplot_per_query(cracking_kd, 8)
    stackplot_per_query(cracking_kd, 16)

    stackplot_per_query(quasii, 1)
    stackplot_per_query(quasii, 2)
    stackplot_per_query(quasii, 4)
    stackplot_per_query(quasii, 8)
    stackplot_per_query(quasii, 16)

    stackplot_per_query(sideways, 1)
    stackplot_per_query(sideways, 2)
    stackplot_per_query(sideways, 4)
    stackplot_per_query(sideways, 8)
    stackplot_per_query(sideways, 16)

    stackplot_per_query(std_cracking, 1)
    stackplot_per_query(std_cracking, 2)
    stackplot_per_query(std_cracking, 4)
    stackplot_per_query(std_cracking, 8)
    stackplot_per_query(std_cracking, 16)

    stackplot_per_query(std_cracking, 1)
    stackplot_per_query(std_cracking, 2)
    stackplot_per_query(std_cracking, 4)
    stackplot_per_query(std_cracking, 8)
    stackplot_per_query(std_cracking, 16)


def main():
    read_files()
    experiment1()
    values([full_scan, std_cracking, cracking_kd, full_kd, quasii, sideways])
    average_offsets([full_scan, std_cracking, cracking_kd, full_kd, quasii, sideways])


if __name__ == '__main__':
    main()
