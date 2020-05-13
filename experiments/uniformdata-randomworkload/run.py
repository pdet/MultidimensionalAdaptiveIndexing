'''Runs the uniform data and workload experiment
'''
import sys
import os
import subprocess
import inspect


REPETITIONS = 1

EXPERIMENTS = [
        {"data": "2data", "workload": "2queries", "name": "2cols"},
        {"data": "4data", "workload": "4queries", "name": "4cols"},
        {"data": "8data", "workload": "8queries", "name": "8cols"},
        {"data": "16data", "workload": "16queries", "name": "16cols"},
]

RUNS = [
    {
        "algorithm_id": "1",
        "results_file": "full_scan"
    },
    {
        "algorithm_id": "111",
        "results_file": "full_scan_cl"
    },
    {
        "algorithm_id": "2",
        "partitions_size": "1024",
        "results_file": "cracking_kd_tree-1024"
    },
    {
        "algorithm_id": "3",
        "partitions_size": "1024",
        "results_file": "cracking_kd_tree_pd-1024"
    },
    {
        "algorithm_id": "4",
        "partitions_size": "1024",
        "results_file": "average_kd_tree-1024"
    },
    {
        "algorithm_id": "5",
        "partitions_size": "1024",
        "results_file": "median_kd_tree-1024"
    },
    {
        "algorithm_id": "6",
        "partitions_size": "1024",
        "results_file": "quasii-1024"
    },
    {
        "algorithm_id": "7",
        "partitions_size": "1024",
        "results_file": "progressive_index-0-2-1024",
        "delta": "0.2"
    },
    {
        "algorithm_id": "7",
        "partitions_size": "1024",
        "results_file": "progressive_index-0-3-1024",
        "delta": "0.3"
    },
    {
        "algorithm_id": "7",
        "partitions_size": "1024",
        "results_file": "progressive_index-0-5-1024",
        "delta": "0.5"
    },
    # Increased partition size to 1048576
    {
        "algorithm_id": "2",
        "partitions_size": "1048576",
        "results_file": "cracking_kd_tree-1048576"
    },
    {
        "algorithm_id": "3",
        "partitions_size": "1048576",
        "results_file": "cracking_kd_tree_pd-1048576"
    },
    {
        "algorithm_id": "4",
        "partitions_size": "1048576",
        "results_file": "average_kd_tree-1048576"
    },
    {
        "algorithm_id": "5",
        "partitions_size": "1048576",
        "results_file": "median_kd_tree-1048576"
    },
    {
        "algorithm_id": "6",
        "partitions_size": "1048576",
        "results_file": "quasii-1048576"
    },
    {
        "algorithm_id": "7",
        "partitions_size": "1048576",
        "results_file": "progressive_index-0-2-1048576",
        "delta": "0.2"
    },
    {
        "algorithm_id": "7",
        "partitions_size": "1048576",
        "results_file": "progressive_index-0-3-1048576",
        "delta": "0.3"
    },
    {
        "algorithm_id": "7",
        "partitions_size": "1048576",
        "results_file": "progressive_index-0-5-1048576",
        "delta": "0.5"
    },
]


# script directory
SCRIPT_PATH = os.path.dirname(
        os.path.abspath(
            inspect.getfile(inspect.currentframe())
        )
    )
os.chdir(SCRIPT_PATH)


class CD:
    """Context manager for changing the current working directory"""
    def __init__(self, new_path):
        self.new_path = os.path.expanduser(new_path)
        self.saved_path = ''

    def __enter__(self):
        self.saved_path = os.getcwd()
        os.chdir(self.new_path)

    def __exit__(self, etype, value, traceback):
        os.chdir(self.saved_path)


class Benchmark:
    """Executes the benchmark given by the config file"""
    CURRENT_DIR = os.getcwd()

    def __init__(
            self,
            build_dir="../../build",
            bin_dir="../../bin"
    ):
        """Benchmark Constructor
        Arguments:
            - build_dir (string): path to MDAI build dir
            - bin_dir (string): path to MDAI bin dir
        """

        self.build_dir = build_dir
        self.bin_dir = bin_dir

    def run(self):
        """Runs the benchmark, saves the results as a CSV file in results.csv
        Arguments:
            - results_file (string): file to save the results
        """
        os.system(
            f'mkdir -p {self.CURRENT_DIR}/results'
        )

        if not os.path.exists(self.build_dir):
            os.makedirs(self.build_dir)

        # Compile the code
        with CD(self.build_dir):
            subprocess.call(["cmake", "-DCMAKE_BUILD_TYPE=Release", ".."])
            subprocess.call(["make", '-j'])

        with CD(self.bin_dir):
            for experiment in EXPERIMENTS:
                for run in RUNS:
                    command = [
                        "./main",
                        "-w", f"{self.CURRENT_DIR}/data/{experiment['workload']}",
                        "-d", f"{self.CURRENT_DIR}/data/{experiment['data']}",
                        "-i", run['algorithm_id'],
                        "-r", str(REPETITIONS),
                        "-s", f"{self.CURRENT_DIR}/results/{experiment['name']}-{run['results_file']}.csv",
                        "-p", run.get('partitions_size', "1024"),
                        "-a", run.get('delta', "0")
                        ]
                    command = ' '.join(command)
                    os.system(command)

    @staticmethod
    def clean(build_dir):
        '''Cleans the build directory
        '''
        # Run make clean
        with CD(build_dir):
            subprocess.call(["make", "clean"])


def main():
    ''' Main method
    '''
    build_dir = "../../build/"
    bin_dir = "../../bin"

    benchmark = Benchmark(build_dir, bin_dir)
    benchmark.run()
    # benchmark.clean(build_dir)

    sys.exit(0)


if __name__ == "__main__":
    main()
