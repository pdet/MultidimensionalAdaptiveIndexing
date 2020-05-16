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

DELTA_LIST = [0.005,0.01,0.05,0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0]
RUNS = [
    {
        "algorithm_id": "1",
        "name": "full_scan"
    }
]
for delta in DELTA_LIST:
    RUNS.append(   {
        "algorithm_id": "7",
        "partitions_size": "1024",
        "name": "progressive_index",
        "delta": str(delta)
    })


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
                        "-s", f"{self.CURRENT_DIR}/results/{experiment['name']}-{run['name']}-{run.get('delta', '0.0')}-{run.get('partitions_size','0')}.csv",
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
