'''Runs the uniform data and workload experiment
'''
import os
import inspect
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


FEATURES_FILE = f"{CURRENT_DIR}/../../data/mdrq-analysis/1000genomes_import/chr22_feature.vectors"
GENOMES_FILE = f"{CURRENT_DIR}/../../data/mdrq-analysis/1000genomes_import/genes.txt"
POWER_FILE = f"{CURRENT_DIR}/../../data/mdrq-analysis/power_import/DEBS2012-ChallengeData.txt"
SKYSERVER_DATA_FILE = f"{CURRENT_DIR}/../../data/skyserver_2.csv"
SKYSERVER_QUERY_FILE = f"{CURRENT_DIR}/../../data/skyserver2_query.csv"

# Read configuration
with open('config.json') as json_file:
    f = json.load(json_file)
    NUMBER_OF_QUERIES = f['number_of_queries']
    REPETITIONS = f['repetitions']
    ROWS = f['rows']
    PARTITION_SIZE = f['partition_size']
    PROGRESSIVE_INDEX_DELTAS = f['deltas']

EXPERIMENTS = []

# Setup for Genomics
for i in [8]:
    exp = {
        "name": f"genomics_query_{i}",
        "data": f"{CURRENT_DIR}/data/genomics_query_{i}data",
        "workload": f"{CURRENT_DIR}/data/genomics_query_{i}queries",
        "query_type": f'{i}',
        "number_of_rows": ROWS,
        'number_of_queries': "100",
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
    "number_of_rows": ROWS,
    'number_of_queries': "3000",
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
skyserver_exp = {
        "name": "skyserver",
        "data": f"{CURRENT_DIR}/data/skyserver_data",
        "workload": f"{CURRENT_DIR}/data/skyserver_queries",
        "repetitions": REPETITIONS
        }

skyserver_exp["exp_id"] = f"{skyserver_exp['name']}-0-0-0.0"
command = [
        "./skyserver_generator",
        "-p", SKYSERVER_DATA_FILE,
        "-q", SKYSERVER_QUERY_FILE,
        "-f", skyserver_exp['data'],
        "-w", skyserver_exp['workload']
        ]
skyserver_exp["command"] = ' '.join(command)
EXPERIMENTS.append(skyserver_exp)

RUNS = [
    {
        "algorithm_id": "111",
        "name": "full_scan_cl",
        "result": f"{CURRENT_DIR}/results/full_scan_cl-{0.0}-{0}"
    },
    {
        "algorithm_id": "2",
        "partitions_size": str(PARTITION_SIZE),
        "name": "cracking_kd_tree",
        "result": f"{CURRENT_DIR}/results/cracking_kd_tree-{0.0}-{PARTITION_SIZE}"
    },
    {
        "algorithm_id": "3",
        "partitions_size": str(PARTITION_SIZE),
        "name": "cracking_kd_tree_pd",
        "result": f"{CURRENT_DIR}/results/cracking_kd_tree_pd-{0.0}-{PARTITION_SIZE}"
    },
    {
        "algorithm_id": "4",
        "partitions_size": str(PARTITION_SIZE),
        "name": "average_kd_tree",
        "result": f"{CURRENT_DIR}/results/average_kd_tree-{0.0}-{PARTITION_SIZE}"
    },
    {
        "algorithm_id": "5",
        "partitions_size": str(PARTITION_SIZE),
        "name": "median_kd_tree",
        "result": f"{CURRENT_DIR}/results/median_kd_tree-{0.0}-{PARTITION_SIZE}"
    },
    {
        "algorithm_id": "6",
        "partitions_size": str(PARTITION_SIZE),
        "name": "quasii",
        "result": f"{CURRENT_DIR}/results/quasii-{0.0}-{PARTITION_SIZE}"
    }
]

for delta in PROGRESSIVE_INDEX_DELTAS:
    RUNS.append(
        {
            "algorithm_id": "7",
            "partitions_size": str(PARTITION_SIZE),
            "name": "progressive_index",
            "delta": f"{delta}",
            "result": f"{CURRENT_DIR}/results/progressive_index-{delta}-{PARTITION_SIZE}"
        }
    )
    RUNS.append(
        {
            "algorithm_id": "7",
            "partitions_size": str(PARTITION_SIZE),
            "name": "progressive_index_cm",
            "delta": f"{delta}",
            "extra_flags": "-c",
            "result": f"{CURRENT_DIR}/results/progressive_index_cm-{delta}-{PARTITION_SIZE}"
        }
    )

def main():
    ''' Main method
    '''
    build_dir = "../../build/"
    bin_dir = "../../bin"

    benchmark = Benchmark(EXPERIMENTS, RUNS, build_dir, bin_dir)
    benchmark.generate()
    benchmark.run()
    # benchmark.clean(build_dir)


if __name__ == "__main__":
    main()
