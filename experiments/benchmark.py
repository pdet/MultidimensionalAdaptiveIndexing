import sys
import os
import subprocess


class CD:
    """Executes the benchmark given by experiments and runs variables"""
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
            experiments,
            runs,
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
        self.experiments = experiments
        self.runs = runs

        if not os.path.exists(self.build_dir):
            os.makedirs(self.build_dir)

        # Compile the code
        with CD(self.build_dir):
            subprocess.call(["cmake", "-DCMAKE_BUILD_TYPE=Release", ".."])
            subprocess.call(["make", '-j'])

    def generate(self):
        """Generates the data and workload for experiments
        """
        os.system(
            f'mkdir -p {self.CURRENT_DIR}/data'
        )

        with CD(self.bin_dir):
            for experiment in self.experiments:
                os.system(experiment['command'])

    def run(self):
        """Runs the benchmark, saves the results as a CSV file in results.csv
        """
        os.system(
            f'mkdir -p {self.CURRENT_DIR}/results'
        )
 
        with CD(self.bin_dir):
            for experiment in self.experiments:
                for run in self.runs:
                    command = [
                        "./main",
                        "-w", experiment['workload'],
                        "-d", experiment['data'],
                        "-i", run['algorithm_id'],
                        "-r", experiment['repetitions'],
                        "-s", f"{run['result']}-{experiment['exp_id']}.csv",
                        "-p", run.get('partitions_size', "1024"),
                        "-a", run.get('delta', "0"),
                        run.get('extra_flags', '')
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
