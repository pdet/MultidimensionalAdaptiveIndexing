import os
import pandas as pd
import matplotlib.pyplot as plt


def cumulative(list1):
    result = []
    value = 0.0
    for x in list1:
        value += x
        result.append(value)
    return result


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


def generate_plot(directory, extra_label=''):
    directory = os.path.abspath(os.path.join(
        os.path.dirname(__file__), directory))
    csvs = [x for x in os.listdir(directory) if x.endswith('.csv')]

    for csv in csvs:
        df = pd.read_csv(
            os.path.join(directory, csv),
            sep=';'
        )
        alg = translate_alg(str(df['algorithm'][0])) + str(extra_label)
        grouped = df.groupby('query_number').mean()
        label_key, = plt.plot(cumulative(grouped['query_time']), label=alg)
        return label_key


def query_response_time(list_of_dirs, title, save_path):
    keys = []
    for directory in list_of_dirs:
        keys.append(generate_plot(directory))

    plt.legend(handles=keys, loc=(1.04, 0), title='Algorithm')
    plt.xlabel('Query Sequence')
    plt.ylabel('Response Time (secs)')
    plt.yscale('log')
    plt.title(title)
    plt.savefig(str(save_path) + '.png', bbox_inches='tight')
    plt.clf()
    plt.cla()
    plt.close()


def main():
    if not os.path.exists('charts'):
        os.makedirs('charts')

    numbers = [0, 36, 72, 108, 144]

    # RANDOM

    dirs = [x + 1 for x in numbers]
    directories = ['experimentresults/' + str(x) for x in dirs]
    query_response_time(
        directories, 'Random | # Columns = 1 | Selectivity = 0.1', 'charts/random1col01sel')

    dirs = [x + 4 for x in numbers]
    directories = ['experimentresults/' + str(x) for x in dirs]
    query_response_time(
        directories, 'Random | # Columns = 8 | Selectivity = 0.1', 'charts/random8col01sel')

    dirs = [x + 5 for x in numbers]
    directories = ['experimentresults/' + str(x) for x in dirs]
    query_response_time(
        directories, 'Random | # Columns = 1 | Selectivity = 0.2', 'charts/random1col02sel')

    dirs = [x + 8 for x in numbers]
    directories = ['experimentresults/' + str(x) for x in dirs]
    query_response_time(
        directories, 'Random | # Columns = 8 | Selectivity = 0.2', 'charts/random8col02sel')

    dirs = [x + 9 for x in numbers]
    directories = ['experimentresults/' + str(x) for x in dirs]
    query_response_time(
        directories, 'Random | # Columns = 1 | Selectivity = 0.3', 'charts/random1col03sel')

    dirs = [x + 12 for x in numbers]
    directories = ['experimentresults/' + str(x) for x in dirs]
    query_response_time(
        directories, 'Random | # Columns = 8 | Selectivity = 0.3', 'charts/random8col03sel')

    # SKEWED

    dirs = [x + 1 + 24 for x in numbers]
    directories = ['experimentresults/' + str(x) for x in dirs]
    query_response_time(
        directories, 'Skewed | # Columns = 1 | Selectivity = 0.1', 'charts/skewed1col01sel')

    dirs = [x + 4 + 24 for x in numbers]
    directories = ['experimentresults/' + str(x) for x in dirs]
    query_response_time(
        directories, 'Skewed | # Columns = 8 | Selectivity = 0.1', 'charts/skewed8col01sel')

    dirs = [x + 5 + 24 for x in numbers]
    directories = ['experimentresults/' + str(x) for x in dirs]
    query_response_time(
        directories, 'Skewed | # Columns = 1 | Selectivity = 0.2', 'charts/skewed1col02sel')

    dirs = [x + 8 + 24 for x in numbers]
    directories = ['experimentresults/' + str(x) for x in dirs]
    query_response_time(
        directories, 'Skewed | # Columns = 8 | Selectivity = 0.2', 'charts/skewed8col02sel')

    dirs = [x + 9 + 24 for x in numbers]
    directories = ['experimentresults/' + str(x) for x in dirs]
    query_response_time(
        directories, 'Skewed | # Columns = 1 | Selectivity = 0.3', 'charts/skewed1col03sel')

    dirs = [x + 12 + 24 for x in numbers]
    directories = ['experimentresults/' + str(x) for x in dirs]
    query_response_time(
        directories, 'Skewed | # Columns = 8 | Selectivity = 0.3', 'charts/skewed8col03sel')


if __name__ == '__main__':
    main()
