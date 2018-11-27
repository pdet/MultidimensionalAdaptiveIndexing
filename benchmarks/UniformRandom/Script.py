import os
import inspect

SCRIPT_PATH =  os.path.dirname(os.path.abspath(inspect.getfile(inspect.currentframe()))) # script directory
os.chdir(SCRIPT_PATH)
os.chdir("../../")

# Setting Values For Distributions
RANDOM = "1"
SEQUENTIAL = "2"  # Sequential only works for queries
SKEWED = "3"

# Setting Values For Algorithms
FULL_SCAN = "0"
STANDARD_CRACKING = "1"
CRACKING_KD_TREE = "2"
FULL_KD_TREE = "3"
SIDEWAYS_CRACKING = "4"
QUASII = "7"

PATH = ""

# Select Experiments to run
experiments = [FULL_SCAN, STANDARD_CRACKING, CRACKING_KD_TREE, FULL_KD_TREE, SIDEWAYS_CRACKING, QUASII]
# Main Configurations
NUM_QUERIES = "1000"
NUMBER_OF_REPETITIONS = 3
COLUMN_SIZE = '1000000'
UPPERBOUND = COLUMN_SIZE
NUMBER_OF_COLUMNS = ['1', '2', '4', '8', '16']
KDTREE_THRESHOLD = '2000'  # Only used for KDTree
QUERY_SELECTIVITY = '0.01'
SEED = '1098'

ONE_SIDED_PERCENTAGE = '0'

QUERIES_PATTERN =  RANDOM
COLUMN_PATTERN = RANDOM
# Saving Experiments
if os.path.exists("Results/") != 1:
    os.system('mkdir Results')

def getFolderToSaveExperiments():
    global PATH
    experimentsList = os.listdir("Results/")
    aux = 0
    for experiment in experimentsList:
        if aux < int(experiment):
            aux = int(experiment)
    currentexperiment = aux + 1
    PATH = "Results/" + str(currentexperiment) + '/'
    os.system('mkdir ' + PATH)


def translate_alg(alg):
    if alg == FULL_SCAN:
        return 'fs'
    if alg == STANDARD_CRACKING:
        return 'stdavl'
    if alg == CRACKING_KD_TREE:
        return 'stdkd'
    if alg == FULL_KD_TREE:
        return 'fikd'
    if alg == SIDEWAYS_CRACKING:
        return 'swc'
    if alg == QUASII:
        return 'quasii'
    return alg

#Output is a csv file with:
#"algorithm;repetition;column_size;column_pattern;number_of_columns;index_creation;index_lookup;scan_time;join_time;total_time"
def generate_output(file,query_result,repetition,ALGORITHM, N_COLUMN, selectivity):
    query_result = query_result.split("\n")
    for query in range(0, len(query_result)-1):
        file.write(translate_alg(ALGORITHM) + ';' + str(repetition) + ";" + selectivity +";"+ str(query)
        + ';' + COLUMN_SIZE + ';' + COLUMN_PATTERN  + ';' + N_COLUMN + ';' + query_result[query])
        file.write('\n')
    file.close()

def create_output():
    # Saving Experiments
    header = "algorithm;repetition;query_selectivity;query_number;column_size;column_pattern;number_of_columns;index_creation;index_lookup;scan_time;join_time;projection_time;total_time;n_offsets;offsets_size"
    file = open(PATH + "results.csv", "w")
    file.write(header)
    file.write('\n')
    return file

def generateExperimentDefine():
    os.system('rm src/util/define.h')
    file = open('src/util/define.h',"w")
    # file.write('\n')
    file.close()

generateExperimentDefine()

print("Compiling")
os.environ['OPT'] = 'true'
if os.system('make') != 0:
    print("Make Failed")
    exit()

for N_COLUMN in NUMBER_OF_COLUMNS:
    SELECTIVITY_PERCENTAGE = str(float(QUERY_SELECTIVITY) ** (1/float(N_COLUMN)))
    print("Generating Data")
    if os.system("./gendata --num-queries=" + NUM_QUERIES + " --column-size=" + COLUMN_SIZE + " --column-number=" +  N_COLUMN
    + " --selectivity=" +SELECTIVITY_PERCENTAGE + " --queries-pattern=" +  QUERIES_PATTERN + " --column-pattern="+ COLUMN_PATTERN
    + " --one-side-ranges=" +ONE_SIDED_PERCENTAGE + " --upperbound=" + UPPERBOUND + " --seed=" + SEED) != 0:
        print("Generating Data Failed")
        exit()

    for experiment in experiments:
        for repetition in range(NUMBER_OF_REPETITIONS):
            getFolderToSaveExperiments()
            result = os.popen("./crackingmain --num-queries=" + NUM_QUERIES + " --column-size=" + COLUMN_SIZE + " --column-number=" +  N_COLUMN + " --indexing-type="+experiment
                +" --kdtree-threshold=" + KDTREE_THRESHOLD).read()
            file = create_output()
            generate_output(file,result,repetition,experiment, N_COLUMN, SELECTIVITY_PERCENTAGE)
