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
FULL_SCAN_HORIZONTAL = 5
FULL_KD_TREE_USING_AVERAGE = 6


# Select Experiments to run
# experiments = [FULL_SCAN_VERTICAL, STANDARD_CRACKING, FULL_INDEX, KD_TREE, FULL_KD_TREE, FULL_SCAN_HORIZONTAL, FULL_KD_TREE_USING_AVERAGE]
experiments = [FULL_KD_TREE_USING_AVERAGE]
# Main Configurations
COLUMN_FILE_PATH = "./column.txt"
QUERIES_FILE_PATH = "./query.txt"
NUM_QUERIES = '5000'
NUMBER_OF_REPETITIONS = '1'
COLUMN_SIZE = '100000'
NUMBER_OF_COLUMNS = '5'
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
if os.system("./gendata" + " " + COLUMN_FILE_PATH + " " + QUERIES_FILE_PATH + " " + SELECTIVITY_PERCENTAGE
             + " " + ONE_SIDED_PERCENTAGE + " " + ZIPF_ALPHA + " " +
             NUM_QUERIES + " " + COLUMN_SIZE + " " + UPPERBOUND
             + " " + str(QUERIES_PATTERN) + " " + str(COLUMN_PATTERN) + " " + str(NUMBER_OF_COLUMNS)) != 0:
    print("Generating Data Failed")
    exit()


if FULL_SCAN_VERTICAL in experiments:
    print("Running Full Scan Vertical")
    if os.system("./crackingmain" + " " + COLUMN_FILE_PATH + " " + QUERIES_FILE_PATH + " " + NUM_QUERIES
                 + " " + COLUMN_SIZE + " " + str(FULL_SCAN_VERTICAL) + " " + str(NUMBER_OF_COLUMNS) + " >> " + PATH + "fsv.txt") != 0:
        print("Running Failed")
        exit()

if FULL_SCAN_HORIZONTAL in experiments:
    print("Running Full Scan Horizontal")
    if os.system("./crackingmain" + " " + COLUMN_FILE_PATH + " " + QUERIES_FILE_PATH + " " + NUM_QUERIES
                 + " " + COLUMN_SIZE + " " + str(FULL_SCAN_HORIZONTAL) + " " + str(NUMBER_OF_COLUMNS) + " >> " + PATH + "fsh.txt") != 0:
        print("Running Failed")
        exit()

if STANDARD_CRACKING in experiments:
    print("Running Standard Cracking")
    if os.system("./crackingmain" + " " + COLUMN_FILE_PATH + " " + QUERIES_FILE_PATH + " " + NUM_QUERIES
                 + " " + COLUMN_SIZE + " " + str(STANDARD_CRACKING) + " " + str(NUMBER_OF_COLUMNS) + " >> " + PATH + "std.txt") != 0:
        print("Running Failed")
        exit()

if FULL_INDEX in experiments:
    print("Running Full Index")
    if os.system("./crackingmain" + " " + COLUMN_FILE_PATH + " " + QUERIES_FILE_PATH + " " + NUM_QUERIES
                 + " " + COLUMN_SIZE + " " + str(FULL_INDEX) + " " + str(NUMBER_OF_COLUMNS) + " " + BPTREE_ELEMENTSPERNODE + " >> " + PATH + "fi.txt") != 0:
        print("Running Failed")
        exit()

if KD_TREE in experiments:
    print("Running KDTree")
    print("./crackingmain" + " " + COLUMN_FILE_PATH + " " + QUERIES_FILE_PATH + " " + NUM_QUERIES
          + " " + COLUMN_SIZE + " " + str(KD_TREE) + " " + str(NUMBER_OF_COLUMNS))
    if os.system("./crackingmain" + " " + COLUMN_FILE_PATH + " " + QUERIES_FILE_PATH + " " + NUM_QUERIES
                 + " " + COLUMN_SIZE + " " + str(KD_TREE) + " " + str(NUMBER_OF_COLUMNS) + " " + str(KDTREE_THRESHOLD) + " >> " + PATH + "kd.txt") != 0:
        print("Running Failed")
        exit()

if FULL_KD_TREE in experiments:
    print("Running Full KDTree")
    if os.system("./crackingmain" + " " + COLUMN_FILE_PATH + " " + QUERIES_FILE_PATH + " " + NUM_QUERIES
                 + " " + COLUMN_SIZE + " " + str(FULL_KD_TREE) + " " + str(NUMBER_OF_COLUMNS) + " " + str(KDTREE_THRESHOLD) + " >> " + PATH + "kdf.txt") != 0:
        print("Running Failed")
        exit()

if FULL_KD_TREE_USING_AVERAGE in experiments:
    print("Running Full KDTree Using Average")
    if os.system("./crackingmain" + " " + COLUMN_FILE_PATH + " " + QUERIES_FILE_PATH + " " + NUM_QUERIES
                 + " " + COLUMN_SIZE + " " + str(FULL_KD_TREE_USING_AVERAGE) + " " + str(NUMBER_OF_COLUMNS) + " " + str(KDTREE_THRESHOLD) + " >> " + PATH + "kdf_avg.txt") != 0:
        print("Running Failed")
        exit()
