'''Runs the uniform data and workload experiment
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
SELECTIVITY = '0.0001'
NUMBER_OF_ROWS = f'{1e7}'
NUMBER_OF_QUERIES = '1000'

REPETITIONS = '1'

ROWS = [1e7]
SELECTIVITIES = [0.0001]
COLS = [2,4,8]
EXPS_DEFAULTS = [
        {
            "name": "uniform",
            "command": "./uniform_generator"
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
                            "exp_id": f"{default['name']}-{row}-{col}-{sel}",
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

PARTITION_SIZE = '1024'
delta = 0.1

RUNS = [
        {
            "algorithm_id": "7",
            "partitions_size": PARTITION_SIZE,
            "exp_id": f"{default['name']}-{row}-{col}-{sel}",
            "name": "progressive_index",
            "extra_flags": "-c",
            "delta": str(delta),
            "result": f"{CURRENT_DIR}/results/progressive_indexcm-{delta}-{PARTITION_SIZE}"
            },
             {
            "algorithm_id": "7",
            "partitions_size": PARTITION_SIZE,
            "exp_id": f"{default['name']}-{row}-{col}-{sel}",
            "name": "progressive_index",
            "delta": str(delta),
            "result": f"{CURRENT_DIR}/results/progressive_index-{delta}-{PARTITION_SIZE}"
            },
        ]

def main():
    ''' Main method
    '''
    build_dir = "../../build/"
    bin_dir = "../../bin"

    parser = argparse.ArgumentParser(description='Run uniform benchmark.')
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
        exp['command'] += f" -f {exp['data']} -w {exp['workload']} "

    benchmark = Benchmark(EXPERIMENTS, RUNS, build_dir, bin_dir)
    if args.generate:
        benchmark.generate()
    benchmark.run()

if __name__ == "__main__":
    main()
