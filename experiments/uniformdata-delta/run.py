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
NUMBER_OF_ROWS = f'{10e7}'
NUMBER_OF_QUERIES = '1000'

REPETITIONS = '1'

EXPERIMENTS = [
        {
            "name": "2cols",
            "data": f"{CURRENT_DIR}/data/2data",
            "workload": f"{CURRENT_DIR}/data/2queries",
            "number_of_cols": '2', "selectivity": SELECTIVITY,
            "number_of_rows": NUMBER_OF_ROWS, 'number_of_queries': NUMBER_OF_QUERIES,
            "repetitions": REPETITIONS
            },
        {
            "name": "4cols",
            "data": f"{CURRENT_DIR}/data/4data",
            "workload": f"{CURRENT_DIR}/data/4queries",
            "number_of_cols": '4', "selectivity": SELECTIVITY,
            "number_of_rows": NUMBER_OF_ROWS, 'number_of_queries': NUMBER_OF_QUERIES,
            "repetitions": REPETITIONS

            },
        {
            "name": "8cols",
            "data": f"{CURRENT_DIR}/data/8data",
            "workload": f"{CURRENT_DIR}/data/8queries",
            "number_of_cols": '8', "selectivity": SELECTIVITY,
            "number_of_rows": NUMBER_OF_ROWS, 'number_of_queries': NUMBER_OF_QUERIES,
            "repetitions": REPETITIONS

            },
        ]

DELTA_LIST = [0.005, 0.01, 0.05, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0]
RUNS = [
        {
            "algorithm_id": "111",
            "name": "full_scan_cl",
            "result" : f"{CURRENT_DIR}/results/full_scan_cl"
            }
        ]

PARTITION_SIZE = '1024'
for delta in DELTA_LIST:
    RUNS.append(   {
        "algorithm_id": "7",
        "partitions_size": PARTITION_SIZE,
        "name": "progressive_index",
        "delta": str(delta),
        "result": f"{CURRENT_DIR}/results/progressive_index-wa-{delta}-{PARTITION_SIZE}"
        })
        RUNS.append(   {
        "algorithm_id": "7",
        "partitions_size": PARTITION_SIZE,
        "name": "progressive_index",
        "workload_adaptive":"1",
        "delta": str(delta),
        "result": f"{CURRENT_DIR}/results/progressive_index-wd-{delta}-{PARTITION_SIZE}"
        })


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

    # fix command to generate experiments
    for exp in EXPERIMENTS:
        exp["command"] = f"./uniform_generator -r {exp['number_of_rows']} -d {exp['number_of_cols']} -q {exp['number_of_queries']} -f {exp['data']} -w {exp['workload']} -s{exp['selectivity']}"

    benchmark = Benchmark(EXPERIMENTS, RUNS, build_dir, bin_dir)
    if args.generate:
        benchmark.generate()
    benchmark.run()
    # benchmark.clean(build_dir)


if __name__ == "__main__":
    main()
