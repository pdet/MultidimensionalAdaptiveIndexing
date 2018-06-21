import pandas as pd
import matplotlib.pyplot as plt


BASE_DIR = '../experimentresults/'


def reset_plot():
    plt.cla()
    plt.clf()
    plt.close()

def translate_alg(alg):
    if alg == 'fs':
        return 'Full Scan'
    if alg == 'stdavl':
        return 'Standard Cracking AVL'
    if alg == 'fibpt':
        return 'Full Index B+Tree'
    if alg == 'stdkd':
        return 'Cracking KDTree'
    if alg == 'fikd':
        return 'Full Index KDTree'
    return alg


def response_time_all_cols():
    algs = ['fs', 'stdavl', 'fibpt', 'stdkd', 'fikd']
    directories = [
        [1,2,3,4,5]
    ]
    cols = [2, 4, 8, 16]

    for alg, dirs in zip(algs, directories):
        results = []
        for dir in dirs:
            df1 = pd.read_csv(BASE_DIR + str(dir) + '/results.csv', sep=';')
            df1 = df1.groupby(['query_number']).mean()
            results.append(df1['total_time'].sum())
        plt.plot(cols, results, label=translate_alg(alg), marker='.', linewidth=2.0, markersize=20.0)
        plt.legend(loc=2)
    plt.ylabel('Response Time (s)')
    plt.xlabel('Number of Columns')
    plt.title('Total Response Time')
    plt.yticks(range(0, 1300, 100))
    plt.grid()
    plt.savefig('response_time_all_cols.pdf')
    reset_plot()


def response_time_accumulated_2_cols():
    dirs = [1, 5, 9, 13, 17]
    markers=['.', 's', '+', 'D', 'p']
    markers_every = [100, 120, 140, 160, 180]

    for dir, mark, me in zip(dirs, markers, markers_every):
        df1 = pd.read_csv(BASE_DIR + str(dir) + '/results.csv', sep=';')
        alg = translate_alg(df1['algorithm'][1])
        df1 = df1.groupby(['query_number']).mean()
        ts = df1['total_time'].cumsum().plot(
            label=alg, legend=True,
            marker=mark, markevery=me
        )
        ts.legend(loc=2)
    ts.set_xlabel('Query (#)')
    ts.set_ylabel('Cumulative Time (s)')
    ts.set_title('Accumulated Response Time (2 columns)')
    plt.savefig('cum_2.pdf')
    reset_plot()


def response_time_accumulated_4_cols():
    dirs = [2, 6, 10, 14, 18]
    markers=['.', 's', '+', 'D', 'p']
    markers_every = [100, 120, 140, 160, 180]

    for dir, mark, me in zip(dirs, markers, markers_every):
        df1 = pd.read_csv(BASE_DIR + str(dir) + '/results.csv', sep=';')
        alg = translate_alg(df1['algorithm'][1])
        df1 = df1.groupby(['query_number']).mean()
        ts = df1['total_time'].cumsum().plot(
            label=alg, legend=True,
            marker=mark, markevery=me
        )
        ts.legend(loc=2)
    ts.set_xlabel('Query (#)')
    ts.set_ylabel('Cumulative Time (s)')
    ts.set_title('Accumulated Response Time (4 columns)')
    plt.savefig('cum_4.pdf')
    reset_plot()


def response_time_accumulated_8_cols():
    dirs = [3, 7, 11, 15, 19]
    markers=['.', 's', '+', 'D', 'p']
    markers_every = [100, 120, 140, 160, 180]

    for dir, mark, me in zip(dirs, markers, markers_every):
        df1 = pd.read_csv(BASE_DIR + str(dir) + '/results.csv', sep=';')
        alg = translate_alg(df1['algorithm'][1])
        df1 = df1.groupby(['query_number']).mean()
        ts = df1['total_time'].cumsum().plot(
            label=alg, legend=True,
            marker=mark, markevery=me
        )
        ts.legend(loc=2)
    ts.set_xlabel('Query (#)')
    ts.set_ylabel('Cumulative Time (s)')
    ts.set_title('Accumulated Response Time (8 columns)')
    plt.savefig('cum_8.pdf')
    reset_plot()
    

def response_time_accumulated_16_cols():
    dirs = [1,2,3,4,5]
    markers=['.', 's', '+', 'D', 'p']
    markers_every = [100, 120, 140, 160, 180]

    for dir, mark, me in zip(dirs, markers, markers_every):
        df1 = pd.read_csv(BASE_DIR + str(dir) + '/results.csv', sep=';')
        alg = translate_alg(df1['algorithm'][1])
        df1 = df1.groupby(['query_number']).mean()
        ts = df1['total_time'].cumsum().plot(
            label=alg, legend=True,
            marker=mark, markevery=me
        )
        ts.legend(loc=2)
    ts.set_xlabel('Query (#)')
    ts.set_ylabel('Cumulative Time (s)')
    ts.set_title('Accumulated Response Time (16 columns)')
    plt.savefig('cum_16.pdf')
    reset_plot()


def total_time_2_cols():
    dirs = [1, 5, 9, 13, 17]
    i_c = []
    i_l = []
    s_t = []
    j_t = []
    algs = []
    for dir in dirs:
        df1 = pd.read_csv(BASE_DIR + str(dir) + '/results.csv', sep=';')
        alg = translate_alg(df1['algorithm'][1])
        df1 = df1.groupby(['query_number']).mean()
        df1 = df1[['index_creation',  'index_lookup',  'scan_time', 'join_time']].sum()
        i_c.append(df1['index_creation'])
        i_l.append(df1['index_lookup'])
        s_t.append(df1['scan_time'])
        j_t.append(df1['join_time'])
        algs.append(alg)
    
    ind = range(len(algs))
    width = 0.35
    i_c_p = plt.bar(ind, i_c, width= width, color='red')
    i_l_p = plt.bar(ind, i_l, width= width, bottom=i_c, color='green')
    s_t_p = plt.bar(ind, s_t, width= width, bottom=i_l, color='yellow')
    j_t_p = plt.bar(ind, j_t, width= width, bottom=s_t)

    plt.ylabel('Total Time (s)')
    plt.title('Breakdown of response time (2 columns)') 
    plt.xticks([x + width/2. for x in ind], algs, rotation=-10)
    plt.legend((i_c_p, i_l_p, s_t_p, j_t_p), ('Index Creation', 'Index Lookup', 'Scan Time', 'Join Time'))

    plt.savefig('time_break_down_2.pdf')
    reset_plot()


def total_time_16_cols():
    dirs = [1,2,3,4,5]
    i_c = []
    i_l = []
    s_t = []
    j_t = []
    algs = []
    for dir in dirs:
        df1 = pd.read_csv(BASE_DIR + str(dir) + '/results.csv', sep=';')
        alg = translate_alg(df1['algorithm'][1])
        df1 = df1.groupby(['query_number']).mean()
        df1 = df1[['index_creation',  'index_lookup',  'scan_time', 'join_time']].sum()
        i_c.append(df1['index_creation'])
        i_l.append(df1['index_lookup'])
        s_t.append(df1['scan_time'])
        j_t.append(df1['join_time'])
        algs.append(alg)
    
    ind = range(len(algs))
    width = 0.35
    i_c_p = plt.bar(ind, i_c, width= width, color='red')
    i_l_p = plt.bar(ind, i_l, width= width, bottom=i_c, color='green')
    s_t_p = plt.bar(ind, s_t, width= width, bottom=i_l, color='yellow')
    j_t_p = plt.bar(ind, j_t, width= width, bottom=s_t)

    plt.ylabel('Total Time (s)')
    plt.title('Breakdown of response time (16 columns)')
    plt.xticks([x + width/2. for x in ind], algs, rotation=-10)
    plt.yticks(range(100, 1300, 100))
    plt.legend((i_c_p, i_l_p, s_t_p, j_t_p), ('Index Creation', 'Index Lookup', 'Scan Time', 'Join Time'))

    plt.savefig('time_break_down_16.pdf')
    reset_plot()


def variance_2_cols():
    dirs = [5, 13, 17]
    for dir in dirs:
        df1 = pd.read_csv(BASE_DIR + str(dir) + '/results.csv', sep=';')
        alg = translate_alg(df1['algorithm'][1])
        df1 = df1.groupby(['query_number']).mean()
        plt.plot(df1['index_lookup'].head(100), label=alg, linewidth=2.0)

    plt.ylabel('Index lookup (s)')
    plt.xlabel('Query (#)')
    plt.legend()
    plt.title('Query response time (16 columns)')
    plt.savefig('variance_2.pdf')
    reset_plot()


def variance_16_cols():
    dirs = [8, 16, 20]
    for dir in dirs:
        df1 = pd.read_csv(BASE_DIR + str(dir) + '/results.csv', sep=';')
        alg = translate_alg(df1['algorithm'][1])
        df1 = df1.groupby(['query_number']).mean()
        plt.plot(df1['index_lookup'].head(100), label=alg, linewidth=2.0)

    plt.ylabel('Index lookup (s)')
    plt.xlabel('Query (#)')
    plt.legend()
    plt.title('Query response time (16 columns)')
    plt.savefig('variance_16.pdf')
    reset_plot()


def config():
    # plt.rc('font', family='serif', serif='Times')
    # plt.rc('xtick', labelsize=8)
    # plt.rc('ytick', labelsize=8)
    # plt.rc('axes', labelsize=8)
    return

def main():
    # variance_2_cols()
    # variance_16_cols()
    # response_time_all_cols()
    # response_time_accumulated_2_cols()
    # response_time_accumulated_4_cols()
    # response_time_accumulated_8_cols()
    response_time_accumulated_16_cols()
    # total_time_2_cols()
    # total_time_16_cols()

if __name__ == '__main__':
    config()
    main()