import os

# Setting Values For Distributions
RANDOM = 1
SEQUENTIAL = 2  # Sequential only works for queries
SKEWED = 3

# Setting Values For Algorithms
FULL_SCAN_VERTICAL = 0
STANDARD_CRACKING = 1
FULL_INDEX = 2
KD_TREE = 3
FULL_KD_TREE = 4


# Select Experiments to run
experiments = [FULL_SCAN_VERTICAL]
# Main Configurations
COLUMN_FILE_PATH = "./column"
QUERIES_FILE_PATH = "./query"
NUM_QUERIES = '10'
NUMBER_OF_REPETITIONS = '1'
COLUMN_SIZE = '10000000'
NUMBER_OF_COLUMNS = '16'
KDTREE_THRESHOLD = '10'  # Only used for KDTree

SELECTIVITY_PERCENTAGE = "0.5"
ONE_SIDED_PERCENTAGE = '0.0'  # Not really using this
ZIPF_ALPHA = '2.0'  # Skewness of dataset
UPPERBOUND = COLUMN_SIZE  # Only set if you want Column Max Value > Column.size
QUERIES_PATTERN = RANDOM
COLUMN_PATTERN = RANDOM

BPTREE_ELEMENTSPERNODE = '16384'  # Only used for Full Index

# Saving Experiments
if os.path.exists("ScriptExperiments/") != 1:
    os.system('mkdir ScriptExperiments')
experimentsList = os.listdir("ScriptExperiments/")
aux = 0
for experiment in experimentsList:
    if aux < int(experiment):
        aux = int(experiment)
currentexperiment = aux + 1
PATH = "ScriptExperiments/" + str(currentexperiment) + '/'
os.system('mkdir ' + PATH)
os.system("cp Script.py " + PATH)

print("Compiling")
os.environ['OPT'] = 'true'
if os.system('make') != 0:
    print("Make Failed")
    exit()

print("Generating Data")
if os.system("./gendata") != 0:
    print("Generating Data Failed")
    exit()

if os.system("./crackingmain" + " >> " + PATH + "fsv.txt") != 0:
    print("Running Failed")
