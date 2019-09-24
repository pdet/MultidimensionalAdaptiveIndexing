import os
import subprocess
import json
import sqlite3
from plotly.subplots import make_subplots
import pandas as pd


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
    def __init__(self, db_path, config_path):
        self.conn = sqlite3.connect(db_path)
        self.df = pd.read_sql_query("SELECT * FROM RESULTS;", self.conn)

        with open(config_path) as json_file:
            self.config = json.load(json_file)

    def average_each_query(self, list_of_values):
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
        fig = make_subplots(rows=1, cols=1)
        algorithms = self.df['NAME'].unique()
        for alg in algorithms:
            temp_df = self.df.loc[self.df['NAME'] == alg].copy()
            temp_df['TOTAL_TIME'] = temp_df['INITIALIZATION_TIME']
            temp_df['TOTAL_TIME'] += temp_df['ADAPTATION_TIME']
            temp_df['TOTAL_TIME'] += temp_df['QUERY_TIME']
            avg = self.average_each_query(list(temp_df['TOTAL_TIME']))
            temp_df = pd.DataFrame({'TOTAL_TIME': avg})
            fig.add_scatter(
                y=temp_df['TOTAL_TIME'].cumsum(),
                name=alg
            )
        fig.update_xaxes(title_text="Query Number")
        fig.update_yaxes(title_text="Time (seconds)")
        fig.update_layout(title_text=f"""Cumulative Sum
                         {self.config['number_of_attributes']} column(s)
                         {self.config['number_of_tuples']} tuples
                         {self.config['selectivity']} selectivity""")
        fig.write_image(file_name)

    def per_query_plot(self, file_name):
        fig = make_subplots(rows=1, cols=1)
        algorithms = self.df['NAME'].unique()
        for alg in algorithms:
            temp_df = self.df.loc[self.df['NAME'] == alg].copy()
            temp_df['TOTAL_TIME'] = temp_df['INITIALIZATION_TIME']
            temp_df['TOTAL_TIME'] += temp_df['ADAPTATION_TIME']
            temp_df['TOTAL_TIME'] += temp_df['QUERY_TIME']
            fig.add_scatter(
                y=self.average_each_query(
                    list(temp_df['TOTAL_TIME'])
                ),
                name=alg
            )
        fig.update_xaxes(title_text="Query Number")
        fig.update_yaxes(title_text="Time (seconds)")
        fig.update_layout(title_text=f"""Per Query
                         {self.config['number_of_attributes']} column(s)
                         {self.config['number_of_tuples']} tuples
                         {self.config['selectivity']} selectivity""")
        fig.write_image(file_name)

    def tuples_scanned(self, file_name):
        fig = make_subplots(rows=1, cols=1)
        algorithms = self.df['NAME'].unique()
        for alg in algorithms:
            temp_df = self.df.loc[self.df['NAME'] == alg].copy()
            fig.add_scatter(
                y=self.average_each_query(
                    list(temp_df['TUPLES_SCANNED'])
                ),
                name=alg
            )
        fig.update_xaxes(title_text="Query Number")
        fig.update_yaxes(title_text="Number of Tuples Scanned")
        fig.update_layout(title_text=f"""Per Query
                         {self.config['number_of_attributes']} column(s)
                         {self.config['number_of_tuples']} tuples
                         {self.config['selectivity']} selectivity""")
        fig.write_image(file_name)


class Benchmark:
    BUILD_DIR = "../../build/"
    BIN_DIR = "../../bin/"
    CURRENT_DIR = os.getcwd()

    def __init__(
            self,
            config_file,
            build_dir="../../build",
            bin_dir="../../bin"
            ):
        with open(config_file) as json_file:
            self.config = json.load(json_file)

        self.BUILD_DIR = build_dir
        self.BIN_DIR = bin_dir

    def run(self, results_file="results"):
        if not os.path.exists(self.BUILD_DIR):
            os.makedirs(self.BUILD_DIR)

        # Compile the code
        with cd(self.BUILD_DIR):
            subprocess.call(["cmake", ".."])
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
                    "-s", self.CURRENT_DIR + "/" + results_file
                ])

        # Run make clean
        with cd(self.BUILD_DIR):
            subprocess.call(["make", "clean"])


benchmark = Benchmark("config.json")
benchmark.run()

plotter = Plots("results", "config.json")
plotter.per_query_plot("per_query.png")
plotter.cum_sum_plot("cum_sum.png")
plotter.tuples_scanned("tuples_scanned.png")
