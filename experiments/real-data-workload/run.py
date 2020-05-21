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


FEATURES_FILE = "/scratch/matheus/data/mdrq-analysis/1000genomes_import/chr22_feature.vectors"
GENOMES_FILE = "/scratch/matheus/data/mdrq-analysis/1000genomes_import/genes.txt"
POWER_FILE = "/scratch/matheus/data/mdrq-analysis/power_import/DEBS2012-ChallengeData.txt"

# General experiment info
NUMBER_OF_ROWS = f'{10e6}'
NUMBER_OF_QUERIES = '3000'

REPETITIONS = '2'

EXPERIMENTS = []

# Setup for Genomics
for i in [0, 1, 2, 3, 4, 5, 6, 7, 8]:
    exp = {
        "name": f"genomics_query_{i}",
        "data": f"{CURRENT_DIR}/data/genomics_query_{i}data",
        "workload": f"{CURRENT_DIR}/data/genomics_query_{i}queries",
        "query_type": f'{i}',
        "number_of_rows": NUMBER_OF_ROWS,
        'number_of_queries': NUMBER_OF_QUERIES,
        "repetitions": REPETITIONS,
        }
    exp['exp_id'] = f"{exp['name']}-{exp['number_of_rows']}-{exp['number_of_queries']}-0.0"
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
    exp['command'] = ' '.join(command)
    EXPERIMENTS.append(exp)

# Setup for Power
power_exp = {
    "name": f"power",
    "data": f"{CURRENT_DIR}/data/power_data",
    "workload": f"{CURRENT_DIR}/data/power_queries",
    "number_of_rows": NUMBER_OF_ROWS,
    'number_of_queries': NUMBER_OF_QUERIES,
    "repetitions": REPETITIONS,
}

power_exp['exp_id'] = f"{power_exp['name']}-{power_exp['number_of_rows']}-{power_exp['number_of_queries']}-0.0"
command = [
        "./power_generator",
        "-r", power_exp['number_of_rows'],
        "-q", power_exp['number_of_queries'],
        "-p", POWER_FILE,
        "-f", power_exp['data'],
        "-w", power_exp['workload']
        ]
power_exp['command'] = ' '.join(command)
EXPERIMENTS.append(power_exp)

# Setup for SkyServer
# TODO: add skyserver here

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
        }
    ]

progressive_index_deltas = [0.1, 0.2, 0.5]

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
                "name": "progressive_index_adaptive",
                "delta": f"{delta}",
                "extra_flags": "-t",
                "result": f"{CURRENT_DIR}/results/progressive_index_adaptive-{delta}-{1024}"
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

    benchmark = Benchmark(EXPERIMENTS, RUNS, build_dir, bin_dir)
    if args.generate:
        benchmark.generate()
    benchmark.run()
    # benchmark.clean(build_dir)


if __name__ == "__main__":
    main()
