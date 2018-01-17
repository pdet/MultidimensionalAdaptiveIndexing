import os

# Setting Values For Distributions
RANDOM = 1
SEQUENTIAL = 2  # Sequential only works for queries
SKEWED = 3

# Setting Values For Algorithms
FULL_SCAN = 0
STANDARD_CRACKING = 1
FULL_INDEX = 2
KD_TREE = 3
FULL_KD_TREE = 4


# Select Experiments to run
experiments = [FULL_KD_TREE, FULL_SCAN]
# Main Configurations
COLUMN_FILE_PATH = "./column.txt"
QUERIES_FILE_PATH = "./query.txt"
NUM_QUERIES = '1000'
NUMBER_OF_REPETITIONS = '5'
COLUMN_SIZE = '100000'
NUMBER_OF_COLUMNS = '5'

SELECTIVITY_PERCENTAGE = "0.5"
ONE_SIDED_PERCENTAGE = '0.0'  # Not really using this
ZIPF_ALPHA = '2.0'  # Skewness of dataset
UPPERBOUND = COLUMN_SIZE  # Only set if you want Column Max Value > Column.size
QUERIES_PATTERN = RANDOM
COLUMN_PATTERN = RANDOM

BPTREE_ELEMENTSPERNODE = '16384'  # Only used for Full Index

# Saving Experiments
if os.path.exists("experiments/") != 1:
    os.system('mkdir experiments')
experimentsList = os.listdir("experiments/")
aux = 0
for experiment in experimentsList:
    if aux < int(experiment):
        aux = int(experiment)
currentexperiment = aux + 1
PATH = "experiments/" + str(currentexperiment) + '/'
os.system('mkdir ' + PATH)
os.system("cp Script.py " + PATH)

print("Compiling")
os.environ['OPT'] = 'true'
if os.system('make') != 0:
    print("Make Failed")
    exit()

print("Generating Data")
if os.system("./gendata" + " " + COLUMN_FILE_PATH + " " + QUERIES_FILE_PATH + " " + SELECTIVITY_PERCENTAGE
             + " " + ONE_SIDED_PERCENTAGE + " " + ZIPF_ALPHA + " " +
             NUM_QUERIES + " " + COLUMN_SIZE + " " + UPPERBOUND
             + " " + str(QUERIES_PATTERN) + " " + str(COLUMN_PATTERN) + " " + str(NUMBER_OF_COLUMNS)) != 0:
    print("Generating Data Failed")
    exit()


if FULL_SCAN in experiments:
    print("Running Full Scan")
    if os.system("./crackingmain" + " " + COLUMN_FILE_PATH + " " + QUERIES_FILE_PATH + " " + NUM_QUERIES
                 + " " + NUMBER_OF_REPETITIONS + " " + COLUMN_SIZE + " " + str(FULL_SCAN) + " " + str(NUMBER_OF_COLUMNS) + " >> " + PATH + "fs.txt") != 0:
        print("Running Failed")
        exit()

if STANDARD_CRACKING in experiments:
    print("Running Standard Cracking")
    if os.system("./crackingmain" + " " + COLUMN_FILE_PATH + " " + QUERIES_FILE_PATH + " " + NUM_QUERIES
                 + " " + NUMBER_OF_REPETITIONS + " " + COLUMN_SIZE + " " + str(STANDARD_CRACKING) + " " + str(NUMBER_OF_COLUMNS) + " >> " + PATH + "std.txt") != 0:
        print("Running Failed")
        exit()

if FULL_INDEX in experiments:
    print("Running Full Index")
    if os.system("./crackingmain" + " " + COLUMN_FILE_PATH + " " + QUERIES_FILE_PATH + " " + NUM_QUERIES
                 + " " + NUMBER_OF_REPETITIONS + " " + COLUMN_SIZE + " " + str(FULL_INDEX) + " " + str(NUMBER_OF_COLUMNS) + " " + BPTREE_ELEMENTSPERNODE + " >> " + PATH + "fi.txt") != 0:
        print("Running Failed")
        exit()

if KD_TREE in experiments:
    print("Running KDTree")
    print("./crackingmain" + " " + COLUMN_FILE_PATH + " " + QUERIES_FILE_PATH + " " + NUM_QUERIES
          + " " + NUMBER_OF_REPETITIONS + " " + COLUMN_SIZE + " " + str(KD_TREE) + " " + str(NUMBER_OF_COLUMNS))
    if os.system("./crackingmain" + " " + COLUMN_FILE_PATH + " " + QUERIES_FILE_PATH + " " + NUM_QUERIES
                 + " " + NUMBER_OF_REPETITIONS + " " + COLUMN_SIZE + " " + str(KD_TREE) + " " + str(NUMBER_OF_COLUMNS) + " >> " + PATH + "kd.txt") != 0:
        print("Running Failed")
        exit()

if FULL_KD_TREE in experiments:
    print("Running Full KDTree")
    if os.system("./crackingmain" + " " + COLUMN_FILE_PATH + " " + QUERIES_FILE_PATH + " " + NUM_QUERIES
                 + " " + NUMBER_OF_REPETITIONS + " " + COLUMN_SIZE + " " + str(FULL_KD_TREE) + " " + str(NUMBER_OF_COLUMNS) + " >> " + PATH + "kdf.txt") != 0:
        print("Running Failed")
        exit()
