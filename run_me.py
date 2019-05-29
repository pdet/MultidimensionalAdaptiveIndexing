# Wizard to help generate the scripts for running experiments
import subprocess
from datetime import datetime
from collections import OrderedDict
import os
import json

def input_or_default(message, default):
    got = input(message)
    if len(got) == 0:
        return default
    else:
        return got

def benchmark_string(benchmark):
    if benchmark == 0:
        return 'Genome'
    if benchmark == 1:
        return 'Sensor Data'
    if benchmark == 2:
        return 'Uniform Distribution (Synthetic)'
    if benchmark == 3:
        return 'Clustered (Synthetic)'
    exit('Error: Benchmark not identified')

# Choose which benchmark to run
benchmark = input_or_default(
"""1) Which benchmark do you want to execute?
    [0] - Genome
    [1] - Sensor Data
    [2] - Uniform Distribution (Synthetic)
    [3] - Clustered (Synthetic)
Please type a number [default: 0]: """,
0
)
benchmark = int(benchmark)

# All benchmarks have variable number of attributes
# Except for Genome, which has always the same number
if benchmark != 0:
    number_of_attributes = input_or_default("""'-1.1) How many attributes [default: 5]? """, '5')
    number_of_attributes = ''.join(number_of_attributes.split('.'))
else:
    number_of_attributes = 16
number_of_attributes = int(number_of_attributes)

# Number of tuples in the dataset
number_of_tuples = input_or_default("""2) How many tuples [default: 1000]? """, '1000')
number_of_tuples = ''.join(number_of_tuples.split('.'))
number_of_tuples = int(number_of_tuples)

# Which algorithms to execute
algorithms = input_or_default(
"""3) Which Algorithms do you want to compare?
    [0] - Cracking KD-Tree Broad
    [1] - Cracking KD-Tree Narrow (To Be Implemented)
    [2] - Quasii
    [3] - Sideways Cracking
    [4] - Standard Cracking with AVL
    [5] - Standard Cracking/AVL using Space Filling Curves (To Be Implemented)
    [6] - BB-Tree
    [7] - ELF
Please specify splitting by spaces [default: 0 1 2 3 4 5 6 7]: """,
'0 1 2 3 4 5 6 7'
).split(' ')
algorithms = list(map(int, OrderedDict.fromkeys(algorithms)))

# Create runs dir
if not os.path.exists('./runs/'):
    os.makedirs('./runs/')

# Name of experiment
while(True):
    experiment_name = input("""Please name your experiment: """)
    if os.path.exists('./runs/' + experiment_name):
        print("Experiment already exists, please choose another name.")
    else:
        os.makedirs('./runs/' + experiment_name)
        break

# Create config dictionary
config = {
    'benchmark': benchmark,
    'number_of_attributes': number_of_attributes,
    'number_of_tuples': number_of_tuples,
    'algorithms': algorithms
}

# Create config file
with open('./runs/' + experiment_name + '/config.json', 'w') as file:
    file.write(json.dumps(config, indent=4, separators=(',', ': ')))

# Summary of what was selected
print(
f"""
--- Summary ---

Benchmark
'- Benchmark: {benchmark} [{benchmark_string(benchmark)}]
'- Number of Attributes: {number_of_attributes}
'- Number of Tuples: {number_of_tuples}

Algorithms: {algorithms}

You can find your experiment in:
-runs/{experiment_name}
  '- run.py               # python script to run the experiment
  '- config.json          # configuration file

---------------

To run the program:
    $ cd ./runs/{experiment_name}
    $ python run.py

A SQLite file called 'results' will be created, with all the measurements stored inside.
If you wish to move the script to some other place just make sure to fix the paths.

If you wish to visualize the results, call:
    python visualizer.py $path to SQLite file$
"""
)