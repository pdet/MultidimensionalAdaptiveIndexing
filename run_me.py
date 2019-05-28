# Wizard to help generate the scripts for running experiments
import subprocess
from datetime import datetime
from collections import OrderedDict

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

benchmark = input(
"""1) Which benchmark do you want to execute?
    [0] - Genome
    [1] - Sensor Data
    [2] - Uniform Distribution (Synthetic)
    [3] - Clustered (Synthetic)
Please type a number: """
)
benchmark = int(benchmark)
# All benchmarks have variable number of attributes
# Except for Genome, which has always the same number
if benchmark != 0:
    number_of_attributes = input("""`-1.1) How many attributes? """)
    number_of_attributes = ''.join(number_of_attributes.split('.'))
else:
    number_of_attributes = 16
number_of_attributes = int(number_of_attributes)

# Number of tuples in the dataset
number_of_tuples = input("""2) How many tuples? """)
number_of_tuples = ''.join(number_of_tuples.split('.'))
number_of_attributes = int(number_of_tuples)

# Which algorithms to execute
algorithms = input(
"""3) Which Algorithms do you want to compare?
    [0] - Cracking KD-Tree Broad
    [1] - Cracking KD-Tree Narrow (To Be Implemented)
    [2] - Quasii
    [3] - Sideways Cracking
    [4] - Standard Cracking with AVL
    [5] - Standard Cracking/AVL using Space Filling Curves (To Be Implemented)
    [6] - BB-Tree
    [7] - ELF
Please specify splitting by spaces: """
).split(' ')
algorithms = list(map(int, OrderedDict.fromkeys(algorithms)))

# Name of script

file_name = input("""Please name your Python script (without .py at the end):
""")

# Create script file
with open(file_name + '.py', 'w') as file:
    file.write(
    f"""\
# Script generated using Wizard

# Git hash: {subprocess.check_output(["git", "rev-parse", "HEAD"]).decode('ascii').strip()}
# Generated at: {datetime.now().strftime("%Y-%m-%d %H:%M")}

# Beggining of script

import subprocess

if not subprocess.run("make"):
    exit("Failed to run make")

if subprocess.run("./generator -b {benchmark} -a {number_of_attributes} -t {number_of_tuples}"):
    subprocess.run("./simulator -d data -q query")

if not subprocess.run("make clean"):
    exit("Failed to run make clean")
""")

# Summary of what was selected
print(
f"""
--- Summary ---

Benchmark
`- Benchmark: {benchmark} [{benchmark_string(benchmark)}]
`- Number of Attributes: {number_of_attributes}
`- Number of Tuples: {number_of_tuples}

Algorithms: {algorithms}

You can find your script in: {file_name}.py
---------------

To run the program call:
    python {file_name}.py

A SQLite file called '{file_name}' will be created, with all the measurements stored inside.
If you wish to move the script to some other place just make sure to fix the paths.
Do notice that the Genome Benchmark depends on other files, so move those files as well.

If you wish to visualize the results, call:
    python visualizer.py $path to SQLite file$
"""
)