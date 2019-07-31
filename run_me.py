# Wizard to help generate the scripts for running experiments
import subprocess
from datetime import datetime
from collections import OrderedDict
import os
import json
import shutil
import datetime

class Benchmarks:
    Normal = 0
    Clustered = 1
    UniformDistribution  = 2
    GMRQB = 3
    Power = 4


def input_or_default(message, default):
    got = input(message)
    if len(got) == 0:
        return default
    else:
        return got

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
    exit('Error: Benchmark not identified')

def number_to_algorithm(n):
    if n == '0':
        return "Cracking KD-Tree Broad"
    if n == '1':
        return "Cracking KD-Tree Narrow"
    if n == '2':
        return "Quasii"
    if n == '3':
        return "Sideways Cracking"
    if n == '4':
        return "Standard Cracking with AVL"
    if n == '5':
        return "Standard Cracking/AVL using Space Filling Curves"
    if n == '6':
        return "BB-Tree"
    if n == '7':
        return "ELF"
    raise Exception(
        f"""Cannot translate algorithm.
        Received: {n}, should be between 0 and 7 """
    )

########## WIZARD START ##########

# Choose which benchmark to run
benchmark = input_or_default(
f"""*- Which benchmark do you want to execute?
    [0] - Normal
    [1] - Clustered (Synthetic)
    [2] - Uniform Distribution (Synthetic)
    [3] - GMRQB
    [4] - Power
Please type a number [default: {Benchmarks.Normal}]: """,
Benchmarks.Normal
)
benchmark = int(benchmark)

# All benchmarks have variable number of attributes
# Except for Genome, which has always the same number
if benchmark != Benchmarks.GMRQB:
    number_of_attributes = input_or_default("""*- How many attributes [default: 5]? """, '5')
    number_of_attributes = ''.join(number_of_attributes.split('.'))
else:
    number_of_attributes = 19
number_of_attributes = int(number_of_attributes)

# Number of tuples in the dataset
number_of_tuples = input_or_default("""*- How many tuples [default: 1000]? """, '1000')
number_of_tuples = ''.join(number_of_tuples.split('.'))
number_of_tuples = int(number_of_tuples)

# Number of queries to be executed
number_of_queries = input_or_default("""*- How many queries [default: 1000]? """, '1000')
number_of_queries = ''.join(number_of_queries.split('.'))
number_of_queries = int(number_of_queries)

# Query selectivity
if benchmark == Benchmarks.GMRQB:
    query_type = input_or_default("""*- Query type [default: 7]? For a mixed workload choose 8 or more:""", '7')
    query_type = float(query_type)
else:
    query_type = 7

# Query selectivity
if benchmark != Benchmarks.GMRQB:
    selectivity = input_or_default("""*- Query selectivity [range (0, 1), default: 0.001]? """, '0.001')
    selectivity = float(selectivity)
    if not (0 < selectivity < 1):
        raise Exception(
            f"""Selectivity should be in the following range (0, 1).
            Received: {selectivity}"""
        )
else:
    selectivity = 0.5


# Which algorithms to execute
algorithms = input_or_default(
"""*- Which Algorithms do you want to compare?
    [0] - Cracking KD-Tree Broad
    [1] - Cracking KD-Tree Narrow (To Be Implemented)
    [2] - Quasii
    [3] - Sideways Cracking (To Be Implemented)
    [4] - Standard Cracking with AVL (To Be Implemented)
    [5] - Standard Cracking/AVL using Space Filling Curves (To Be Implemented)
    [6] - BB-Tree (To Be Implemented)
    [7] - ELF (To Be Implemented)
Please specify splitting by spaces [default: 0]: """,
'0'
).split(' ')
algorithms = list(map(number_to_algorithm, OrderedDict.fromkeys(algorithms)))

repetitions = input_or_default("How many times each algorithm should run [default: 3]?", "3")

# Create runs dir
if not os.path.exists('./runs/'):
    os.makedirs('./runs/')

# Name of experiment
while(True):
    date_now = datetime.datetime.now().strftime("%Y-%m-%d-%H-%M-%S")
    experiment_name = input_or_default(
        f"""Please name your experiment [default: {date_now}]: """,
        date_now
    )
    if os.path.exists('./runs/' + experiment_name):
        print("Experiment already exists, please choose another name.")
    else:
        os.makedirs('./runs/' + experiment_name)
        break

# Create config dictionary
config = {
    'benchmark': benchmark,
    'number_of_attributes': number_of_attributes,
    'number_of_queries': number_of_queries,
    'number_of_tuples': number_of_tuples,
    'algorithms': algorithms,
    'query_type': query_type,
    'selectivity': selectivity,
    'repetitions': repetitions
}

# Create config file
with open('./runs/' + experiment_name + '/config.json', 'w') as file:
    file.write(json.dumps(config, indent=4, separators=(',', ': ')))

shutil.copy2('benchmark.py', './runs/' + experiment_name)

# Summary of what was selected
print(
f"""
--- Summary ---

Benchmark
'- Benchmark: {benchmark} [{benchmark_string(benchmark)}]
'- Number of Attributes: {number_of_attributes}
'- Number of Tuples: {number_of_tuples}
'- Number of Queries: {number_of_queries}
'- Selectivity: {selectivity}
'- Algorithms: {algorithms}
'- Repetitions: {repetitions}

You can find your experiment in:
-runs/{experiment_name}
  '- benchmark.py               # python script to run the experiment
  '- config.json          # configuration file

---------------

To run the program:
    $ cd ./runs/{experiment_name}
    $ python benchmark.py

A SQLite file called 'results' will be created, with all the measurements stored inside.
If you wish to move the script to some other place just make sure to fix the paths.

If you wish to visualize the results, call:
    python visualizer.py $path to SQLite file$
"""
)