import os
import inspect

SCRIPT_PATH =  os.path.dirname(os.path.abspath(inspect.getfile(inspect.currentframe()))) # script directory
os.chdir(SCRIPT_PATH)

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
PARTIAL_SIDEWAYS_CRACKING = "5"
COVERED_CRACKING = "6"

# Select Experiments to run
experiments = [SIDEWAYS_CRACKING,PARTIAL_SIDEWAYS_CRACKING]
# Main Configurations
NUM_QUERIES = "10"
COLUMN_SIZE = '10000000'
NUMBER_OF_COLUMNS = '8'
KDTREE_THRESHOLD = '2000'  # Only used for KDTree

SELECTIVITY_PERCENTAGE = "0.2"
QUERIES_PATTERN =  RANDOM 
COLUMN_PATTERN = RANDOM  

os.system('rm src/util/define.h')
file = open('src/util/define.h',"w")
file.close()


print("Compiling")
os.environ['OPT'] = 'true'
if os.system('make') != 0:
    print("Make Failed")
    exit()

# print("Generating Data")
# if os.system("./gendata --num-queries=" + NUM_QUERIES + " --column-size=" + COLUMN_SIZE + " --column-number=" +  NUMBER_OF_COLUMNS
#  + " --selectivity=" +SELECTIVITY_PERCENTAGE + " --queries-pattern=" +  QUERIES_PATTERN + " --column-pattern="+ COLUMN_PATTERN) != 0:
#     print("Generating Data Failed")
#     exit()


for experiment in experiments:
        os.system("./crackingmain --num-queries=" + NUM_QUERIES + " --column-size=" + COLUMN_SIZE + " --column-number=" +  NUMBER_OF_COLUMNS + " --indexing-type="+experiment
            +" --kdtree-threshold=" + KDTREE_THRESHOLD)