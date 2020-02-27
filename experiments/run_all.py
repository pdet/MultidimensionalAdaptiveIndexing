import os
import subprocess
import pandas as pd
from matplotlib import pyplot as plt
import seaborn as sns


def save_fig(path):
    plt.tight_layout()
    plt.savefig(path)
    plt.cla()
    plt.clf()
    plt.close()


def plot_data_distribution(experiment_folder):
    ''' Plot the data pattern
    '''
    data = pd.read_csv(experiment_folder + "/data", sep=" ", header=None)
    os.system(f"mkdir -p {experiment_folder}/data_distribution")
    for column in data:
        bins = 100
        sns.distplot(data[column], bins=bins)
        plt.ticklabel_format(style='plain')
        plt.title(
            f"Distribution of Column {column}\
             ({bins} bins, {len(data.index)} rows)"
        )
        save_fig(f"{experiment_folder}/data_distribution/data-{column}")


def plot_cummulative_response_time(experiment_folder, df):
    algorithms = df['NAME'].unique()
    for algorithm in algorithms:
        cumsum = df[df['NAME'] == algorithm]['TOTAL_TIME'].cumsum()
        plt.plot(
            range(len(cumsum)),
            cumsum,
            label=algorithm
        )
    plt.xlabel('Query Number')
    plt.ylabel('Cumulative Response Time (s)')
    plt.legend(
        loc='center left', bbox_to_anchor=(1, 0.5), fancybox=True
    )
    save_fig(f'{experiment_folder}/cumsum.pdf')


def plot_response_time_breakdown(experiment_folder, df):
    algorithms = df['NAME'].unique()
    init_t = [df[df['NAME'] == alg]['INITIALIZATION_TIME'].sum() for alg in algorithms]
    adapt_t = [df[df['NAME'] == alg]['ADAPTATION_TIME'].sum() for alg in algorithms]
    query_t = [df[df['NAME'] == alg]['QUERY_TIME'].sum() for alg in algorithms]
    total_t = [df[df['NAME'] == alg]['TOTAL_TIME'].sum() for alg in algorithms]

    initBars = [i / j * 100 for i, j in zip(init_t, total_t)]
    adaptBars = [i / j * 100 for i, j in zip(adapt_t, total_t)]
    queryBars = [i / j * 100 for i, j in zip(query_t, total_t)]

    r = range(len(algorithms))
    # plot
    barWidth = 0.85
    # Create init Bars
    plt.bar(
        r, initBars, color='#b5ffb9',
        edgecolor='white', width=barWidth,
        label='Initialization Time'
    )
    # Create adapt Bars
    plt.bar(
        r, adaptBars, bottom=initBars,
        color='#f9bc86', edgecolor='white', width=barWidth,
        label='Adaptation Time'

    )
    # Create query Bars
    plt.bar(
        r, queryBars, bottom=[i+j for i, j in zip(initBars, adaptBars)],
        color='#a3acff', edgecolor='white', width=barWidth,
        label='Query Time'
    )

    plt.xticks(r, algorithms, rotation=45)
    plt.xlabel("Algorithms")
    plt.ylabel("Time Percentage")

    plt.legend(loc='upper left', bbox_to_anchor=(1, 1), ncol=1)

    save_fig(f'{experiment_folder}/response_time_breakdown.pdf')


def plot_results(experiment_folder):
    ''' Plots the results.csv that can be found inside experiment_folder
        Example:
        experiments_folder
          |- run.py
          |- data
          |- queries
          |- results.csv
        Will plot results.csv
    '''
    csv = pd.read_csv(experiment_folder + "/results.csv")
    # Group all the repetitions
    # TODO
    # Some pre-processing from the results file
    csv['TOTAL_TIME'] = csv['INITIALIZATION_TIME']
    csv['TOTAL_TIME'] += csv['ADAPTATION_TIME']
    csv['TOTAL_TIME'] += csv['QUERY_TIME']

    plot_cummulative_response_time(experiment_folder, csv)
    plot_response_time_breakdown(experiment_folder, csv)


def plots(experiment_folder):
    '''General plots
    '''
    plot_data_distribution(experiment_folder)

    plot_results(experiment_folder)


def main():
    # Get the list of experiments
    experiment_directories = list(os.walk('.'))[0][1]

    for experiment in experiment_directories:
        # Run only the ones with run.py inside of it
        if('run.py' in os.listdir(experiment)):
            process = subprocess.run([
                "python",
                "./" + experiment + "/run.py"
            ])
            # Plot the results (if all went okay)
            if process.returncode == 0:
                plots("./" + experiment)


if __name__ == "__main__":
    main()
