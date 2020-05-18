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


FEATURES_FILE = "/scratch/matheus/mdrq-analysis/1000genomes_import/chr22_feature.vectors"
GENOMES_FILE = "/scratch/matheus/mdrq-analysis/1000genomes_import/genes.txt"

# General experiment info
NUMBER_OF_ROWS = f'{10e7}'
NUMBER_OF_QUERIES = '10'

REPETITIONS = '3'

EXPERIMENTS = []
for i in [0, 1, 2, 3, 4, 5, 6, 7, 8]:
    EXPERIMENTS.append(
            {
                "name": f"{i}query",
                "data": f"{CURRENT_DIR}/data/{i}data",
                "workload": f"{CURRENT_DIR}/data/{i}queries",
                "query_type": f'{i}',
                "number_of_rows": NUMBER_OF_ROWS, 'number_of_queries': NUMBER_OF_QUERIES,
                "repetitions": REPETITIONS
                }
            )

RUNS = [
    {
        "algorithm_id": "1",
        "name": "full_scan",
        "result": f"{CURRENT_DIR}/results/full_scan-{0.0}-{0}"
    },
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
        command = [
            "./genome_generator",
            "-r", exp['number_of_rows'],
            "-q", exp['number_of_queries'],
            "-t", exp['query_type'],
            "-e", FEATURES_FILE,
            "-g", GENOMES_FILE,
            "-f", exp['data'],
            "-w", exp['workload']
            ]
        exp["command"] = ' '.join(command)

    benchmark = Benchmark(EXPERIMENTS, RUNS, build_dir, bin_dir)
    if args.generate:
        benchmark.generate()
    benchmark.run()
    # benchmark.clean(build_dir)


if __name__ == "__main__":
    main()
