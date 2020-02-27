import os
import subprocess
import inspect


REPETITIONS = 1
PARTITION_SIZE = 1000

# script directory
SCRIPT_PATH = os.path.dirname(
        os.path.abspath(
            inspect.getfile(inspect.currentframe())
        )
    )
os.chdir(SCRIPT_PATH)


class cd:
    """Context manager for changing the current working directory"""
    def __init__(self, newPath):
        self.newPath = os.path.expanduser(newPath)

    def __enter__(self):
        self.savedPath = os.getcwd()
        os.chdir(self.newPath)

    def __exit__(self, etype, value, traceback):
        os.chdir(self.savedPath)


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

        self.BUILD_DIR = build_dir
        self.BIN_DIR = bin_dir

    def run(self):
        """Runs the benchmark, saves the results as a CSV file in results.csv
        Arguments:
            - results_file (string): file to save the results
        """
        if os.path.exists(f'{SCRIPT_PATH}/results.csv'):
            os.system(
                f'mv {SCRIPT_PATH}/results.csv {SCRIPT_PATH}/results.csv.old'
            )

        if not os.path.exists(self.BUILD_DIR):
            os.makedirs(self.BUILD_DIR)

        # Compile the code
        with cd(self.BUILD_DIR):
            subprocess.call(["cmake", "-DCMAKE_BUILD_TYPE=Release", ".."])
            subprocess.call(["make", '-j'])

        with cd(self.BIN_DIR):
            subprocess.call([
                "./main",
                "-w", f"{SCRIPT_PATH}/queries",
                "-d", f"{SCRIPT_PATH}/data",
                "-r", str(REPETITIONS),
                "-s", self.CURRENT_DIR,
                "-p", str(PARTITION_SIZE)
            ])

    def clean(build_dir):
        # Run make clean
        with cd(build_dir):
            subprocess.call(["make", "clean"])


if __name__ == "__main__":
    BUILD_DIR = "../../build/"
    BIN_DIR = "../../bin"

    benchmark = Benchmark(BUILD_DIR, BIN_DIR)
    benchmark.run()
    # Benchmark.clean(BUILD_DIR)

    exit(0)
