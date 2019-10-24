import os
import subprocess
import json
import pandas as pd
import matplotlib.pyplot as plt
from textwrap import wrap
import inspect

# script directory
SCRIPT_PATH = os.path.dirname(
        os.path.abspath(
            inspect.getfile(inspect.currentframe())
        )
    )
os.chdir(SCRIPT_PATH)


class cd:
    """Context manager for changing the current working directory"""
    def __init__(self, newPath):
        self.newPath = os.path.expanduser(newPath)

    def __enter__(self):
        self.savedPath = os.getcwd()
        os.chdir(self.newPath)

    def __exit__(self, etype, value, traceback):
        os.chdir(self.savedPath)


class Plots:
    """Plots the results from the experiments"""
    def __init__(self, db_path, config):
        """Plots Constructor

        Arguments:
            - db_path (string): path to csv file
            - config (dict): dictionary with configuration
        """
        self.df = pd.read_csv(db_path)

        self.config = config

    def average_each_query(self, list_of_values):
        """Each experiment can be ran multiple times,
        this method gathers the information from the config file,
        and averages the given list.

        Example:
            list_of_values = [2, 3, 4, 5, 2, 3]
            repetitions = 2

            Result: [(2 + 4)/2, (3 + 2)/2, (4 + 3)/2]

        Returns:
            - List of floats
        """
        repetitions = int(self.config['repetitions'])
        final = []
        length = len(list_of_values)/repetitions
        length = int(length)

        for i in range(length):
            final.append(0.0)
            for r in range(repetitions):
                final[i] += list_of_values[i + r * length]
            final[i] = final[i]/float(repetitions)
        return final

    def cum_sum_plot(self, file_name):
        """Cummulative sum of each query

        Arguments:
            - file_name (string): output file
        """

        fig, ax = plt.subplots(nrows=1, ncols=1)
        algorithms = self.df['NAME'].unique()
        avgs = {}
        for alg in algorithms:
            temp_df = self.df.loc[self.df['NAME'] == alg].copy()
            temp_df['TOTAL_TIME'] = temp_df['INITIALIZATION_TIME']
            temp_df['TOTAL_TIME'] += temp_df['ADAPTATION_TIME']
            temp_df['TOTAL_TIME'] += temp_df['QUERY_TIME']
            avg = self.average_each_query(list(temp_df['TOTAL_TIME']))
            temp_df = pd.DataFrame({'TOTAL_TIME': avg})
            avgs[alg] = list(temp_df['TOTAL_TIME'].cumsum())

        sorted_avgs = sorted(avgs, key=lambda x: avgs[x][-1], reverse=True)
        handles = []

        for alg in sorted_avgs:
            handle, = ax.plot(avgs[alg])
            handles.append(handle)
        ax.set_xlabel("Query Number")
        ax.set_ylabel("Time (seconds)")
        ax.legend(handles, sorted_avgs, bbox_to_anchor=(1.0, 1.0))
        title = f"Cumulative Sum\
                {self.config['number_of_attributes']}-column(s)\
                {self.config['number_of_tuples']}-tuples\
                {self.config['selectivity']}-selectivity"
        ax.set_title("\n".join(wrap(title, 30, break_long_words=False)))
        fig.savefig(file_name, bbox_inches='tight')

    def per_query_plot(self, file_name):
        """Per query time

        Arguments:
            - file_name (string): output file
        """
        fig, ax = plt.subplots(nrows=1, ncols=1)
        algorithms = self.df['NAME'].unique()
        avgs = {}
        for alg in algorithms:
            temp_df = self.df.loc[self.df['NAME'] == alg].copy()
            temp_df['TOTAL_TIME'] = temp_df['INITIALIZATION_TIME']
            temp_df['TOTAL_TIME'] += temp_df['ADAPTATION_TIME']
            temp_df['TOTAL_TIME'] += temp_df['QUERY_TIME']
            avg = self.average_each_query(list(temp_df['TOTAL_TIME']))
            avgs[alg] = avg

        for alg in sorted(avgs, key=lambda x: avgs[x][-1], reverse=True):
            ax.plot(
                avgs[alg],
                label=alg
            )

        ax.set_xlabel("Query Number")
        ax.set_ylabel("Time (seconds)")
        ax.legend(bbox_to_anchor=(1.0, 1.0))
        title = f"Per Query Time\
                {self.config['number_of_attributes']}-column(s)\
                {self.config['number_of_tuples']}-tuples\
                {self.config['selectivity']}-selectivity"
        ax.set_title("\n".join(wrap(title, 30, break_long_words=False)))
        fig.savefig(file_name, bbox_inches='tight')

    def per_query_cost_breakdown(self, name):
        """Per algorithm query cost breakdown
        """
        algorithms = self.df['NAME'].unique()
        for alg in algorithms:
            temp_df = self.df.loc[self.df['NAME'] == alg].copy()
            temp_df['INITIALIZATION_TIME'] = self.average_each_query(
                    list(temp_df['INITIALIZATION_TIME'])
            )
            temp_df['ADAPTATION_TIME'] = self.average_each_query(
                    list(temp_df['ADAPTATION_TIME'])
            )
            temp_df['QUERY_TIME'] = self.average_each_query(
                    list(temp_df['QUERY_TIME'])
            )
            fig = temp_df[
                    ['INITIALIZATION_TIME', 'ADAPTATION_TIME', 'QUERY_TIME']
                         ].plot.area()
            title = f"Per Query Time Breakdown {alg}\
                {self.config['number_of_attributes']}-column(s)\
                {self.config['number_of_tuples']}-tuples\
                {self.config['selectivity']}-selectivity"
            fig = fig.get_figure()
            fig.suptitle("\n".join(wrap(title, 30, break_long_words=False)))
            fig.savefig(name + "/breakdown_" + alg, bbox_inches='tight')

    def tuples_scanned(self, file_name):
        """Number of tuples scanned per query

        Arguments:
            - file_name (string): output file
        """

        fig, ax = plt.subplots(nrows=1, ncols=1)
        algorithms = self.df['NAME'].unique()
        avgs = {}
        for alg in algorithms:
            temp_df = self.df.loc[self.df['NAME'] == alg].copy()
            avg = self.average_each_query(list(temp_df['TUPLES_SCANNED']))
            avgs[alg] = avg

        for alg in sorted(avgs, key=lambda x: avgs[x][-1], reverse=True):
            ax.plot(
                avgs[alg],
                label=alg
            )

        ax.set_xlabel("Query Number")
        ax.set_ylabel("Number of Tuples Scanned")
        ax.legend(bbox_to_anchor=(1.0, 1.0))
        title = f"Per Query Tuples Scanned\
                {self.config['number_of_attributes']}-column(s)\
                {self.config['number_of_tuples']}-tuples\
                {self.config['selectivity']}-selectivity"
        ax.set_title("\n".join(wrap(title, 30, break_long_words=False)))
        fig.savefig(file_name, bbox_inches='tight')

    def size_per_query(self, file_name):
        """Per query index size 
        Arguments:
            - file_name (string): output file
        """

        fig, ax = plt.subplots(nrows=1, ncols=1)
        algorithms = self.df['NAME'].unique()
        avgs = {}
        for alg in algorithms:
            temp_df = self.df.loc[self.df['NAME'] == alg].copy()
            avg = self.average_each_query(list(temp_df['MEMORY_FOOTPRINT']))
            avgs[alg] = avg

        for alg in sorted(avgs, key=lambda x: avgs[x][-1], reverse=True):
            ax.plot(
                avgs[alg],
                label=alg
            )

        ax.set_xlabel("Query Number")
        ax.set_ylabel("Memory Footprint (Bytes)")
        ax.legend(bbox_to_anchor=(1.0, 1.0))
        title = f"Memory Footprint per Query\
                {self.config['number_of_attributes']}-column(s)\
                {self.config['number_of_tuples']}-tuples\
                {self.config['selectivity']}-selectivity"
        ax.set_title("\n".join(wrap(title, 30, break_long_words=False)))
        fig.savefig(file_name, bbox_inches='tight')

    def first_query_time(self, file_name):
        """First query response time

        Arguments:
            - file_name (string): output file
        """
        fig, ax = plt.subplots(nrows=1, ncols=1)
        algorithms = list(self.df['NAME'].unique())
        first_query_times = []
        for alg in algorithms:
            temp_df = self.df.loc[self.df['NAME'] == alg].copy()
            temp_df['TOTAL_TIME'] = temp_df['INITIALIZATION_TIME']
            temp_df['TOTAL_TIME'] += temp_df['ADAPTATION_TIME']
            temp_df['TOTAL_TIME'] += temp_df['QUERY_TIME']
            avg = self.average_each_query(list(temp_df['TOTAL_TIME']))
            first_query_times.append(avg[0])

        algs_pos = [i for i, _ in enumerate(algorithms)]
        ax.bar(algs_pos, first_query_times)
        ax.set_xticks(algs_pos)
        ax.set_xticklabels(algorithms, rotation='vertical')

        ax.set_xlabel("Algorithm")
        ax.set_ylabel("Time (seconds)")
        title = f"First QueryTime\
                {self.config['number_of_attributes']}-column(s)\
                {self.config['number_of_tuples']}-tuples\
                {self.config['selectivity']}-selectivity"
        ax.set_title("\n".join(wrap(title, 30, break_long_words=False)))
        fig.savefig(file_name, bbox_inches='tight')


class Benchmark:
    """Executes the benchmark given by the config file"""
    CURRENT_DIR = os.getcwd()

    def __init__(
            self,
            config,
            build_dir="../../build",
            bin_dir="../../bin"
            ):
        """Benchmark Constructor

        Arguments:
            - config (dict): dict with configuration
            - build_dir (string): path to MDAI build dir
            - bin_dir (string): path to MDAI bin dir
        """

        self.config = config
        self.BUILD_DIR = build_dir
        self.BIN_DIR = bin_dir

    def run(self, results_file="results", compile_code=True):
        """Runs the benchmark, saves the results as CSV file in results_file

        Arguments:
            - results_file (string): file to save the results
        """
        if not os.path.exists(self.BUILD_DIR):
            os.makedirs(self.BUILD_DIR)

        # Compile the code
        if(compile_code):
            with cd(self.BUILD_DIR):
                subprocess.call(["cmake", "-DCMAKE_BUILD_TYPE=Release", ".."])
                subprocess.call(["make"])

        with cd(self.BIN_DIR):
            # Generate data.sql and queries.sql
            subprocess.call([
                "./generator",
                "-w", str(self.config['benchmark']),
                "-d", str(self.config['number_of_attributes']),
                "-q", str(self.config['number_of_queries']),
                "-r", str(self.config['number_of_tuples']),
                "-s", str(self.config['selectivity']),
                "-t", str(self.config['query_type'])
            ])

            # Run all the algorithms, one at a time
            for algorithm in self.config['algorithms']:
                subprocess.call([
                    "./main",
                    "-w", "queries",
                    "-d", "data",
                    "-i", algorithm,
                    "-r", str(self.config['repetitions']),
                    "-s", self.CURRENT_DIR + "/" + results_file,
                    "-p", str(self.config["minimum_partition_size"])
                ])

    def clean(build_dir):
        # Run make clean
        with cd(build_dir):
            subprocess.call(["make", "clean"])


if __name__ == "__main__":
    BUILD_DIR = "../../build/"
    BIN_DIR = "../../bin/"

    with open("config.json") as json_file:
        config = json.load(json_file)

    compile_code = True

    for exp in config['experiments']:
        name = exp['name']
        os.makedirs(name)
        benchmark = Benchmark(exp, BUILD_DIR, BIN_DIR)
        benchmark.run(
                results_file=name + '/results.csv',
                compile_code=compile_code
        )
        compile_code = False
    Benchmark.clean(BUILD_DIR)

    for exp in config['experiments']:
        name = exp['name']
        plotter = Plots(name + '/results.csv', exp)
        plotter.per_query_plot(name + "/per_query.png")
        plotter.first_query_time(name + "/first_query.png")
        plotter.cum_sum_plot(name + "/cum_sum.png")
        plotter.tuples_scanned(name + "/tuples_scanned.png")
        plotter.per_query_cost_breakdown(name)
        plotter.size_per_query(name + "/footprint.png")
