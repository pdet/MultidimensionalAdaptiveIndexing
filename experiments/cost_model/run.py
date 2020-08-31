'''Plots the queries
'''
import os
import inspect
import argparse
import sys
import json


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

# Read configuration
with open('config.json') as json_file:
    f = json.load(json_file)
    NUMBER_OF_QUERIES = f['number_of_queries']
    REPETITIONS = f['repetitions']
    ROWS = [f['rows']]
    SELECTIVITIES = [f['selectivity']]
    COLS = f['cols']
    EXPS_DEFAULTS = f['experiments']
    PARTITION_SIZE = f['partition_size']
    PROGRESSIVE_INDEX_DELTAS = f['deltas']


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
                        "data": f"{CURRENT_DIR}/data/{default['name']}-{row}-{col}-{sel}-d",
                        "workload": f"{CURRENT_DIR}/data/{default['name']}-{row}-{col}-{sel}-w",
                        "command": default['command']
                    }
                )

RUNS = [
    {
        "algorithm_id": "111",
        "name": "full_scan_cl",
        "result": f"{CURRENT_DIR}/results/full_scan_cl-{0.0}-{0}"
    },
    {
        "algorithm_id": "21",
        "partitions_size": str(PARTITION_SIZE),
        "name": "cracking_kd_tree",
        "extra_flags": "-z 0.5",
        "result": f"{CURRENT_DIR}/results/cracking_kd_tree-{0.0}-{PARTITION_SIZE}"
    },
]

for delta in PROGRESSIVE_INDEX_DELTAS:
    RUNS.append(
        {
            "algorithm_id": "7",
            "partitions_size": str(PARTITION_SIZE),
            "name": "progressive_index_time_limit",
            "delta": f"{delta}",
            "extra_flags": "-z 0.5",
            "result": f"{CURRENT_DIR}/results/progressive_index_time_limit-{delta}-{PARTITION_SIZE}"
        }
    )
    RUNS.append(
        {
            "algorithm_id": "7",
            "partitions_size": str(PARTITION_SIZE),
            "name": "progressive_index_query_limit",
            "delta": f"{delta}",
            "extra_flags": "-z 0.5 -o 10",
            "result": f"{CURRENT_DIR}/results/progressive_index_query_limit-{delta}-{PARTITION_SIZE}"
        }
    )
    RUNS.append(
        {
            "algorithm_id": "7",
            "partitions_size": str(PARTITION_SIZE),
            "name": "progressive_index_below",
            "delta": f"{delta}",
            "extra_flags": "-y 0.5",
            "result": f"{CURRENT_DIR}/results/progressive_index_below-{delta}-{PARTITION_SIZE}"
        }
    )

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
