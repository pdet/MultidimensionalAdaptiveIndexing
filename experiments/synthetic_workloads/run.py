'''Plots the queries
'''
import os
import inspect
import argparse
import sys


# script directory
SCRIPT_PATH = os.path.dirname(
        os.path.abspath(
            inspect.getfile(inspect.currentframe())
            )
        )
os.chdir(SCRIPT_PATH)

CURRENT_DIR = os.getcwd()

# magic to be able to import benchmark from parent dir
sys.path.append(os.getcwd() + '/..')

from benchmark import Benchmark


# General experiment info
SELECTIVITY = '0.001'
NUMBER_OF_ROWS = f'{10e7}'
NUMBER_OF_QUERIES = '1000'
REPETITIONS = '1'

EXPERIMENTS = [
        {
            "name": "uniform",
            "number_of_rows": NUMBER_OF_ROWS,
            "number_of_columns": '2',
            "selectivity": SELECTIVITY,
            "repetitions": REPETITIONS,
            "number_of_queries": NUMBER_OF_QUERIES,
            "data": f"{CURRENT_DIR}/data/uniform_d",
            "workload": f"{CURRENT_DIR}/data/uniform",
            "command": "./uniform_generator"
            },
        {
            "name": "skewed",
            "number_of_rows": NUMBER_OF_ROWS,
            "number_of_columns": '2',
            "selectivity": SELECTIVITY,
            "repetitions": REPETITIONS,
            "number_of_queries": NUMBER_OF_QUERIES,
            "data": f"{CURRENT_DIR}/data/skewed_d",
            "workload": f"{CURRENT_DIR}/data/skewed",
            "command": "./skewed_generator"
            },
        {
            "name": "sequential",
            "number_of_rows": NUMBER_OF_ROWS,
            "number_of_columns": '2',
            "selectivity": SELECTIVITY,
            "repetitions": REPETITIONS,
            "number_of_queries": NUMBER_OF_QUERIES,
            "data": f"{CURRENT_DIR}/data/sequential_d",
            "workload": f"{CURRENT_DIR}/data/sequential",
            "command": "./sequential_generator"
            },
        {
            "name": "periodic",
            "number_of_rows": NUMBER_OF_ROWS,
            "number_of_columns": '2',
            "selectivity": SELECTIVITY,
            "repetitions": REPETITIONS,
            "number_of_queries": NUMBER_OF_QUERIES,
            "data": f"{CURRENT_DIR}/data/periodic_d",
            "workload": f"{CURRENT_DIR}/data/periodic",
            "command": "./periodic_generator"
            },
        {
            "name": "zoom_in",
            "number_of_rows": NUMBER_OF_ROWS,
            "number_of_columns": '2',
            "selectivity": SELECTIVITY,
            "repetitions": REPETITIONS,
            "number_of_queries": NUMBER_OF_QUERIES,
            "data": f"{CURRENT_DIR}/data/zoom_in_d",
            "workload": f"{CURRENT_DIR}/data/zoom_in",
            "command": "./zoom_generator"
            },
        {
            "name": "zoom_out",
            "number_of_rows": NUMBER_OF_ROWS,
            "number_of_columns": '2',
            "selectivity": SELECTIVITY,
            "repetitions": REPETITIONS,
            "number_of_queries": NUMBER_OF_QUERIES,
            "data": f"{CURRENT_DIR}/data/zoom_out_d",
            "workload": f"{CURRENT_DIR}/data/zoom_out",
            "command": "./zoom_generator -b"
            },
        {
            "name": "sequential_zoom_in",
            "number_of_rows": NUMBER_OF_ROWS,
            "number_of_columns": '2',
            "selectivity": SELECTIVITY,
            "repetitions": REPETITIONS,
            "number_of_queries": NUMBER_OF_QUERIES,
            "data": f"{CURRENT_DIR}/data/sequential_zoom_in_d",
            "workload": f"{CURRENT_DIR}/data/sequential_zoom_in",
            "command": "./sequential_zoom_generator"
            },
        {
            "name": "sequential_zoom_out",
            "number_of_rows": NUMBER_OF_ROWS,
            "number_of_columns": '2',
            "selectivity": SELECTIVITY,
            "repetitions": REPETITIONS,
            "number_of_queries": NUMBER_OF_QUERIES,
            "data": f"{CURRENT_DIR}/data/sequential_zoom_out_d",
            "workload": f"{CURRENT_DIR}/data/sequential_zoom_out",
            "command": "./sequential_zoom_generator -b"
            },
        {
            "name": "alternating_zoom_in",
            "number_of_rows": NUMBER_OF_ROWS,
            "number_of_columns": '2',
            "selectivity": SELECTIVITY,
            "repetitions": REPETITIONS,
            "number_of_queries": NUMBER_OF_QUERIES,
            "data": f"{CURRENT_DIR}/data/alternating_zoom_in_d",
            "workload": f"{CURRENT_DIR}/data/alternating_zoom_in",
            "command": "./alternating_zoom_generator"
            },
        {
            "name": "alternating_zoom_out",
            "number_of_rows": NUMBER_OF_ROWS,
            "number_of_columns": '2',
            "selectivity": SELECTIVITY,
            "repetitions": REPETITIONS,
            "number_of_queries": NUMBER_OF_QUERIES,
            "data": f"{CURRENT_DIR}/data/alternating_zoom_out_d",
            "workload": f"{CURRENT_DIR}/data/alternating_zoom_out",
            "command": "./alternating_zoom_generator -b"
            },
        ]


RUNS = [
    {
        "algorithm_id": "111",
        "name": "full_scan_cl",
        "result": f"{CURRENT_DIR}/results/full_scan-{0.0}-{0}"
    },
    {
        "algorithm_id": "2",
        "partitions_size": "1024",
        "name": "cracking_kd_tree",
        "result": f"{CURRENT_DIR}/results/cracking_kd_tree-{0.0}-{1024}"
    },
    {
        "algorithm_id": "3",
        "partitions_size": "1024",
        "name": "cracking_kd_tree_pd",
        "result": f"{CURRENT_DIR}/results/cracking_kd_tree_pd-{0.0}-{1024}"
    },
    {
        "algorithm_id": "4",
        "partitions_size": "1024",
        "name": "average_kd_tree",
        "result": f"{CURRENT_DIR}/results/average_kd_tree-{0.0}-{1024}"
    },
    {
        "algorithm_id": "5",
        "partitions_size": "1024",
        "name": "median_kd_tree",
        "result": f"{CURRENT_DIR}/results/median_kd_tree-{0.0}-{1024}"
    },
    {
        "algorithm_id": "6",
        "partitions_size": "1024",
        "name": "quasii",
        "result": f"{CURRENT_DIR}/results/quasii-{0.0}-{1024}"
    },
    {
        "algorithm_id": "7",
        "partitions_size": "1024",
        "name": "progressive_index",
        "delta": "0.2",
        "result": f"{CURRENT_DIR}/results/progressive_index-{0.2}-{1024}"
    },
    {
        "algorithm_id": "7",
        "partitions_size": "1024",
        "name": "progressive_index",
        "delta": "0.3",
        "result": f"{CURRENT_DIR}/results/progressive_index-{0.3}-{1024}"
    },
    {
        "algorithm_id": "7",
        "partitions_size": "1024",
        "name": "progressive_index",
        "delta": "0.5",
        "result": f"{CURRENT_DIR}/results/progressive_index-{0.5}-{1024}"
    },
    {
        "algorithm_id": "7",
        "partitions_size": "1024",
        "name": "progressive_index",
        "delta": "0.8",
        "result": f"{CURRENT_DIR}/results/progressive_index-{0.8}-{1024}"
    }
]


def main():
    ''' Main method
    '''
    build_dir = "../../build/"
    bin_dir = "../../bin"

    for exp in EXPERIMENTS:
        exp['command'] += f" -r {exp['number_of_rows']} -d {exp['number_of_columns']}"
        exp['command'] += f" -s {exp['selectivity']} -q {exp['number_of_queries']}"
        exp['command'] += f" -f {exp['data']} -w {exp['workload']}"

    benchmark = Benchmark(EXPERIMENTS, RUNS, build_dir, bin_dir)
    benchmark.generate()
    benchmark.run()

if __name__ == "__main__":
    main()
