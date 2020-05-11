'''Runs the uniform data and workload experiment
'''
import sys
import os
import subprocess
import inspect


REPETITIONS = 1
PARTITION_SIZE = 1024
ALGORITHM_IDS = [1, 2, 3, 4, 5, 6, 111]

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
            query_types = [0]
            for query_type in query_types:
                for algorithm_id in ALGORITHM_IDS:
                    command = [
                        "./main",
                        "-w", f"{self.CURRENT_DIR}/data/queries{query_type}",
                        "-d", f"{self.CURRENT_DIR}/data/data{query_type}",
                        "-i", str(algorithm_id),
                        "-r", str(REPETITIONS),
                        "-s", f"{self.CURRENT_DIR}/results/",
                        "-p", str(PARTITION_SIZE)
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
