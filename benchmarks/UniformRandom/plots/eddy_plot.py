import pandas as pd
import matplotlib.pyplot as plt
import matplotlib as mpl
import numpy as np
import operator


mpl.rcParams['hatch.linewidth'] = 2.0 # Changes hatch line width


BASE_DIR = '../ResultsEddy/'
# (1-50 = 8col) (50-100 = 2col) (100 a 150 = 4 col) (150 a 200 = 16col)

# 1-10 full scan
# 11 - 20 cracking avl
# 21 - 30 b+ tree
# 31 - 40 cracking kd
# 41 - 50 kd-tree

def reset_plot():
    plt.cla()
    plt.clf()
    plt.close()

def get_me_the_average(column, folders):
    average = 0.0
    for f in folders:
        df1 = pd.read_csv(BASE_DIR + str(f) + '/results.csv', sep=';')
        average += df1[column].sum()
    return average/len(folders)

def total_time_8_cols():
    full_scan = xrange(1, 11)
    db_cracking = xrange(11, 21)
    full_kd = xrange(41, 51)

    algs = ('Full Scan', 'Database Cracking', 'KD-Tree')
    hatchs = ['//', '\\\\', '|']
    pos = [0, 1, 2]

    performances = [
        get_me_the_average('total_time', full_scan),
        get_me_the_average('total_time', db_cracking),
        get_me_the_average('total_time', full_kd)
    ]

    for p, perf, hatch in zip(pos, performances, hatchs):
        plt.bar(p, perf, align='center', alpha = 0.5, hatch=hatch, color='grey')
    plt.xticks(pos, algs)
    plt.ylabel('Total time (s)')
    plt.title('Total Time (8 columns)')
    plt.savefig('totaltime_8.pdf')
    reset_plot()

def cumulative_time(column, folders):
    results = []
    for f in folders:
        results.append(np.array(pd.read_csv(BASE_DIR + str(f) + '/results.csv', sep=';')[column]))
    temp = results.pop(0)
    for r in results:
        temp = temp + r
    temp = temp/len(folders)
    return np.cumsum(temp)

def cumulative_time_8_cols():
    full_scan = xrange(1, 11)
    db_cracking = xrange(11, 21)
    b_tree = xrange(21, 31)
    cracking_kd = xrange(31, 41)
    full_kd = xrange(41, 51)

    markers=['.', 's', '*', 'D', 'X']
    markers_every = [100, 120, 140, 160, 180]

    times = [
        cumulative_time('total_time', full_scan),
        cumulative_time('total_time', db_cracking),
        cumulative_time('total_time', b_tree),
        cumulative_time('total_time', cracking_kd),
        cumulative_time('total_time', full_kd)
    ]

    colors = [
        (105/255.0,105/255.0,105/255.0),
        (128/255.0,128/255.0,128/255.0),
        (169/255.0,169/255.0,169/255.0),
        (105/255.0,105/255.0,105/255.0),
        (128/255.0,128/255.0,128/255.0)
    ]

    algs = ('Full Scan', 'Database Cracking', 'B+ Tree', 'Cracking KD-Tree', 'KD-Tree')

    for t, m, m_e, alg, c in zip(times, markers, markers_every, algs, colors):
        plt.plot(t, marker=m, markevery=m_e, label=alg, color=c)
        plt.legend(loc=2)

    plt.ylabel('Cumulative time (s)')
    plt.xlabel('Query (#)')
    plt.title('Accumulated Response Time 8 columns')
    plt.savefig('cumulative_8.pdf')
    reset_plot()

def time_breakdown_8_cols():
    full_scan = xrange(1, 11)
    db_cracking = xrange(11, 21)
    b_tree = xrange(21, 31)
    cracking_kd = xrange(31, 41)
    full_kd = xrange(41, 51)
    dirs = [full_scan, db_cracking, b_tree, cracking_kd, full_kd]
    algs = ('Full Scan', 'Database Cracking', 'B+ Tree', 'Cracking KD-Tree', 'KD-Tree')
    i_c = []
    i_l = []
    s_t = []
    j_t = []
    for d in dirs:
        i_c.append(get_me_the_average('index_creation', d))
        i_l.append(get_me_the_average('index_lookup', d))
        s_t.append(get_me_the_average('scan_time', d))
        j_t.append(get_me_the_average('join_time', d))

    # Fix for Full Scan
    s_t[0] = get_me_the_average('total_time', full_scan)

    i_c = np.array(i_c)
    i_l = np.array(i_l)
    s_t = np.array(s_t)
    j_t = np.array(j_t)

    ind = np.arange(len(algs))
    width = 0.35
    i_c_p = plt.bar(ind, i_c, width= width, hatch='//', color=(105/255.0,105/255.0,105/255.0))
    i_l_p = plt.bar(ind, i_l, width= width, bottom=i_c, hatch='||', color=(128/255.0,128/255.0,128/255.0))
    s_t_p = plt.bar(ind, s_t, width= width, bottom=i_c + i_l, hatch='--', color=(128/255.0,128/255.0,128/255.0))
    j_t_p = plt.bar(ind, j_t, width= width, bottom=i_c + i_l + s_t, hatch='\\\\', color=(169/255.0,169/255.0,169/255.0))

    plt.ylabel('Total Time (s)')
    plt.title('Breakdown of Response Time (8 columns)') 
    plt.xticks([x + width/2. for x in ind], algs, rotation=-10)
    plt.legend((i_c_p, i_l_p, s_t_p, j_t_p), ('Index Creation', 'Index Lookup', 'Scan Time', 'Join Time'))

    plt.savefig('time_break_down_8.pdf')
    reset_plot()

def response_time_all_columns():
    full_scan = [xrange(51, 61), xrange(101, 111), xrange(1, 11), xrange(151, 161)]
    db_cracking = [xrange(61, 71), xrange(111, 121), xrange(11, 21), xrange(161, 171)]
    b_tree = [xrange(71, 81), xrange(121, 131), xrange(21, 31), xrange(171, 181)]
    cracking_kd = [xrange(81, 91), xrange(131, 141), xrange(31, 41), xrange(181, 191)]
    full_kd = [xrange(91, 101), xrange(141, 151), xrange(41, 51), xrange(191, 201)]

    algs = [full_scan, db_cracking, b_tree, cracking_kd, full_kd]
    alg_names = ('Full Scan', 'Database Cracking', 'B+ Tree', 'Cracking KD-Tree', 'KD-Tree')
    cols = ('2', '4', '8', '16')
    markers=['.', 's', '*', 'D', 'X']
    colors = [
        (105/255.0,105/255.0,105/255.0),
        (128/255.0,128/255.0,128/255.0),
        (169/255.0,169/255.0,169/255.0),
        (105/255.0,105/255.0,105/255.0),
        (128/255.0,128/255.0,128/255.0)
    ]
    times = []

    for x in algs:
        temp = []
        for d in x:
            temp.append(get_me_the_average('total_time', d))
        times.append(temp)

    for time, name, marker, color in zip(times, alg_names, markers, colors):
        plt.plot(cols, time, label=name, marker=marker, linewidth=2.0, color=color)
        plt.legend(loc=2)
    
    plt.ylabel('Response time (s)')
    plt.xlabel('Number of Columns')
    plt.title('Total Response Time')
    plt.savefig('all_response_times.pdf')
    reset_plot()

def main():
    # total_time_8_cols()
    # cumulative_time_8_cols()
    # time_breakdown_8_cols()
    response_time_all_columns()

if __name__ == '__main__':
    main()