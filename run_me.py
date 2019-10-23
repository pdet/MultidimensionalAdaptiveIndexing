import datetime
from collections import OrderedDict
import os
import json
import shutil
import pprint


class Benchmarks:
    """Class to keep track of the benchmarks"""
    Normal = 0
    Clustered = 1
    UniformDistribution = 2
    GMRQB = 3
    Power = 4
    Our_benchmark = 5
    Medians = 6

    def benchmark_string(benchmark):
        if benchmark == Benchmarks.Normal:
            return 'Normal'
        if benchmark == Benchmarks.Clustered:
            return 'Clustered (Synthetic)'
        if benchmark == Benchmarks.UniformDistribution:
            return 'Uniform Distribution (Synthetic)'
        if benchmark == Benchmarks.GMRQB:
            return 'GMRQB'
        if benchmark == Benchmarks.Power:
            return 'Power'
        if benchmark == Benchmarks.Our_benchmark:
            return 'Our Benchmark'
        if benchmark == Benchmarks.Medians:
            return 'Medians'
        exit('Error: Benchmark not identified')


class Algorithms:
    CRACKING_KD_TREE_BROAD = 0
    QUASII = 1
    KD_TREE_MEDIAN = 2
    KD_TREE_AVERAGE = 3
    FULL_SCAN = 4

    def to_hash():
        return {
            Algorithms.CRACKING_KD_TREE_BROAD: "Cracking KD-Tree Broad",
            Algorithms.QUASII: "Quasii",
            Algorithms.KD_TREE_MEDIAN: "KDTree-Median",
            Algorithms.KD_TREE_AVERAGE: "KDTree-Average",
            Algorithms.FULL_SCAN: "Full-Scan"
        }

    def pp_hash(h, prefix):
        k = list(h.keys())
        k.sort()
        for key in k:
            print(f"{prefix}{key} -> {h[key]}")

    def number_to_algorithm(n):
        if(int(n) in Algorithms.to_hash()):
            return Algorithms.to_hash()[int(n)]
        raise Exception(
                f"Cannot translate algorithm\n\
                Received: {n}\n\
                Available: {Algorithms.to_hash()}"
                )


class MDAIWizard:
    """Wizard to help generate the scripts for running experiments"""
    def __init__(self):
        """MDAIWizard Constructor"""
        self.config = {}
        self.pp = pprint.PrettyPrinter(indent=4)

    def start(self):
        """Starts the wizard"""

        # Create experiments dir
        if not os.path.exists('./experiments/'):
            os.makedirs('./experiments/')

        # Name of experiment
        while(True):
            date_now = datetime.datetime.now().strftime("%Y-%m-%d-%H-%M-%S")
            experiment_name = self.__input_or_default(
                "Please name your experiment:",
                date_now
            )
            if os.path.exists('./experiments/' + experiment_name):
                print("Experiment already exists, please choose another name.")
            else:
                os.makedirs('./experiments/' + experiment_name)
                break

        config_path = './experiments/' + experiment_name + '/config.json'

        self.config['experiments'] = []

        while(True):
            self.config['experiments'].append(
                    self.generate_config()
                )
            # Save config file to keep what the user already did
            with open(config_path, 'w') as file:
                file.write(
                        json.dumps(
                            self.config,
                            indent=4,
                            separators=(',', ': ')
                            )
                    )
            print("Configuration until now:")
            self.pp.pprint(self.config['experiments'])
            print()
            cont = self.__input_or_default(
                "Do you want to generate another experiment(y/n)?",
                'n'
            )
            if cont.lower() == 'n':
                break

        shutil.copy2('benchmark.py', './experiments/' + experiment_name)
        print(f"""
                You can find your experiment in:
                    -experiments/{experiment_name}
                    '- benchmark.py -- python script to run the experiment
                    '- config.json  -- configuration file

                To run the program:
                    $ cd ./experiments/{experiment_name}
                    $ python benchmark.py

                For each experiment a directory with its name will be created
                with a CSV file with the measurements, and the plots.

                If you wish to move the script to some other place
                    just make sure to fix the paths.
              """)

    def generate_config(self):
        """Generates a hash with the configuration for the experiment"""
        config = {}
        # Choose which benchmark to run
        benchmark = self.__input_or_default(
            """*- Which benchmark do you want to execute?
            [0] - Normal
            [1] - Clustered (Synthetic)
            [2] - Uniform Distribution (Synthetic)
            [3] - GMRQB
            [4] - Power
            [5] - Our Benchmark
            [6] - Medians
            Please type a number:""",
            Benchmarks.Normal
        )
        benchmark = int(benchmark)
        # All benchmarks have variable number of attributes
        # Except for Genome, which has always the same number
        if benchmark != Benchmarks.GMRQB:
            number_of_attributes = self.__input_or_default(
                    "*- How many attributes?", '5'
            )
            number_of_attributes = ''.join(
                    number_of_attributes.split('.')
            )
        else:
            number_of_attributes = 19
        number_of_attributes = int(number_of_attributes)

        # Number of tuples in the dataset
        number_of_tuples = self.__input_or_default(
                "*- How many tuples?", '1000'
        )
        number_of_tuples = ''.join(number_of_tuples.split('.'))
        number_of_tuples = int(number_of_tuples)

        # Number of queries to be executed
        number_of_queries = self.__input_or_default(
                "*- How many queries?", '1000'
        )
        number_of_queries = ''.join(number_of_queries.split('.'))
        number_of_queries = int(number_of_queries)

        # Query selectivity
        if benchmark == Benchmarks.GMRQB:
            query_type = self.__input_or_default(
                    "*- Query type. For a mixed workload choose 8 or more:",
                    '7'
            )
            query_type = float(query_type)
        else:
            query_type = 7

        # Query selectivity
        if benchmark != Benchmarks.GMRQB:
            selectivity = self.__input_or_default(
                    "*- Query selectivity, range (0, 1)?",
                    '0.001'
            )
            selectivity = float(selectivity)
            if not (0 < selectivity < 1):
                raise Exception(
                    f"""Selectivity should be in the following range (0, 1).
                    Received: {selectivity}"""
                )
        else:
            selectivity = 0.5

        # Which algorithms to execute
        print("*- Which Algorithms do you want to compare?")
        Algorithms.pp_hash(
                Algorithms.to_hash(), "\t")
        algorithms = self.__input_or_default(
            "Please specify splitting by spaces:",
            '0 1 2 3 4'
        ).split(' ')
        algorithms = list(
                map(
                    Algorithms.number_to_algorithm,
                    OrderedDict.fromkeys(algorithms)
                )
        )

        repetitions = self.__input_or_default(
            "*- How many times each algorithm should run?",
            "3"
        )

        min_partition_size = self.__input_or_default(
            "*- What is the minimum partition size?",
            "1000"
        )

        n_conf = "config" + str(len(self.config['experiments']))

        config_name = self.__input_or_default(
            f"*- Please name this configuration:",
            n_conf
        )

        # Create config dictionary
        config = {
            'name': config_name,
            'benchmark': benchmark,
            'number_of_attributes': number_of_attributes,
            'number_of_queries': number_of_queries,
            'number_of_tuples': number_of_tuples,
            'algorithms': algorithms,
            'query_type': query_type,
            'selectivity': selectivity,
            'repetitions': repetitions,
            'minimum_partition_size': min_partition_size
        }

        return config

    def __input_or_default(self, message, default):
        """Input function but with a default value if input is empty"""
        got = input(f"{message}[Default: {default}]")
        if len(got) == 0:
            return default
        else:
            return got


if __name__ == '__main__':
    wizard = MDAIWizard()
    wizard.start()
