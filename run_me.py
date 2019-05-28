# Wizard to help generate the scripts for running experiments
import subprocess
from datetime import datetime

def benchmark_string(benchmark):
    if benchmark == '0':
        return 'Genome'
    if benchmark == '1':
        return 'Sensor Data'
    if benchmark == '2':
        return 'Uniform Distribution (Synthetic)'
    if benchmark == '3':
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

# All benchmarks have variable number of attributes
# Except for Genome, which has always the same number
if benchmark != '0':
    number_of_attributes = input("""`-1.1) How many attributes? """)
    number_of_attributes = ''.join(number_of_attributes.split('.'))
else:
    number_of_attributes = '16'

number_of_tuples = input("""2) How many tuples? """)
number_of_tuples = ''.join(number_of_tuples.split('.'))

file_name = 'script.py'

with open(file_name, 'w') as file:
    file.write(
    f"""
        # Git hash: {subprocess.check_output(["git", "rev-parse", "HEAD"]).strip()}
        # Current date: {datetime.now()}

        # Beggining of script
        import subprocess

        if subprocess.run("./generator -b {benchmark} -a {number_of_attributes} -t {number_of_tuples}"):
            subprocess.run("./simulator -d data -q query")
    """)

print(
f"""
@@@ You selected @@@
Benchmark: {benchmark} [{benchmark_string(benchmark)}]
Number of Attributes: {number_of_attributes}
Number of Tuples: {number_of_tuples}
@@@@@@@@@@@@@@@@@@@@
You can find your script in: {file_name}
"""
)