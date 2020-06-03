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
NUMBER_OF_QUERIES = '3000'
REPETITIONS = '1'
ROWS = [10**7]
SELECTIVITIES = [0.1]
COLS = [2, 4, 8]
#NUMBER_OF_QUERIES = '5'
#REPETITIONS = '1'
#ROWS = [10e2, 10e2]
#SELECTIVITIES = [0.1, 0.01, 0.001]
#COLS = [2, 4]
EXPS_DEFAULTS = [
        {
            "name": "uniform",
            "command": "./uniform_generator"
            },
        {
            "name": "skewed",
            "command": "./skewed_generator"
            },
        {
            "name": "sequential",
            "command": "./sequential_generator"
            },
        {
            "name": "periodic",
            "command": "./periodic_generator"
            },
        {
            "name": "zoom_in",
            "command": "./zoom_generator"
            },
        {
            "name": "zoom_out",
            "command": "./zoom_generator -b"
            },
        {
            "name": "sequential_zoom_in",
            "command": "./sequential_zoom_generator"
            },
        {
            "name": "sequential_zoom_out",
            "command": "./sequential_zoom_generator -b"
            },
        {
            "name": "alternating_zoom_in",
            "command": "./alternating_zoom_generator"
            },
        {
            "name": "alternating_zoom_out",
            "command": "./alternating_zoom_generator -b"
            },
        ]


EXPERIMENTS = []

for row in ROWS:
    for sel in SELECTIVITIES:
        for col in COLS:
            for default in EXPS_DEFAULTS:
                EXPERIMENTS.append(
                        {
                            "name": default['name'],
                            "exp_id": f"{default['name']}-{row}-{NUMBER_OF_QUERIES}-{col}-{sel}",
                            "number_of_rows": str(row),
                            "number_of_columns": str(col),
                            "selectivity": str(sel),
                            "repetitions": str(REPETITIONS),
                            "number_of_queries": str(NUMBER_OF_QUERIES),
                            "data": f"/scratch/matheus/data/{default['name']}-{row}-{col}-{sel}-d",
                            "workload": f"/scratch/matheus/data/{default['name']}-{row}-{col}-{sel}-w",
                            "command": default['command']
                            }
                        )


            

RUNS = [
    {
        "algorithm_id": "1",
        "name": "full_scan",
        "result": f"{CURRENT_DIR}/results/full_scan_bv-{0.0}-{0}"
    },
    {
        "algorithm_id": "111",
        "name": "full_scan_cl",
        "result": f"{CURRENT_DIR}/results/full_scan_cl-{0.0}-{0}"
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
        }
    ]

progressive_index_deltas = [0.1]

for delta in progressive_index_deltas:
    RUNS.append(
            {
                "algorithm_id": "7",
                "partitions_size": "1024",
                "name": "progressive_index",
                "delta": f"{delta}",
                "result": f"{CURRENT_DIR}/results/progressive_index-{delta}-{1024}"
                }
            )
    RUNS.append(
            {
                "algorithm_id": "7",
                "partitions_size": "1024",
                "name": "progressive_index_cm",
                "delta": f"{delta}",
                "extra_flags": "-c",
                "result": f"{CURRENT_DIR}/results/progressive_index_cm-{delta}-{1024}"
                }
            )


def main():
    ''' Main method
    '''
    build_dir = "../../build/"
    bin_dir = "../../bin"

    parser = argparse.ArgumentParser(description='Run uniforn benchmark.')
    parser.add_argument(
        '--generate',
        dest='generate',
        action='store_true',
        help='if this flag is setted then generate experiment data (DESTRUCTIVE ACTION, can overwrite what is inside data folder)'
    )

    args = parser.parse_args()

    for exp in EXPERIMENTS:
        exp['command'] += f" -r {exp['number_of_rows']} -d {exp['number_of_columns']}"
        exp['command'] += f" -s {exp['selectivity']} -q {exp['number_of_queries']}"
        exp['command'] += f" -f {exp['data']} -w {exp['workload']}"

    benchmark = Benchmark(EXPERIMENTS, RUNS, build_dir, bin_dir)
    if args.generate:
        benchmark.generate()
    benchmark.run()

if __name__ == "__main__":
    main()
