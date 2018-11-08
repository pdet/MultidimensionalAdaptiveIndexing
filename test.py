import os
import inspect

os.system('rm src/util/define.h')
file = open('src/util/define.h',"w")
file.write('#define test')
file.close()

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

# Main Configurations
NUM_QUERIES = "1000"
COLUMN_SIZE = '100000'
UPPERBOUND = COLUMN_SIZE
NUMBER_OF_COLUMNS = '8'
KDTREE_THRESHOLD = '2000'  # Only used for KDTree
QUERY_SELECTIVITY = '0.8'

ONE_SIDED_PERCENTAGE = '0'

SELECTIVITY_PERCENTAGE = str(float(QUERY_SELECTIVITY) ** (1/float(NUMBER_OF_COLUMNS)))
QUERIES_PATTERN =  RANDOM
COLUMN_PATTERN = RANDOM

print("Compiling")
os.environ['OPT'] = 'true'
if os.system('make') != 0:
    print("Make Failed")
    exit()

print("Generating Data")
if os.system("./gendata --num-queries=" + NUM_QUERIES + " --column-size=" + COLUMN_SIZE + " --column-number=" +  NUMBER_OF_COLUMNS
 + " --selectivity=" +SELECTIVITY_PERCENTAGE + " --queries-pattern=" +  QUERIES_PATTERN + " --column-pattern="+ COLUMN_PATTERN + " --one-side-ranges=" +ONE_SIDED_PERCENTAGE + " --upperbound=" + UPPERBOUND) != 0:
    print("Generating Data Failed")
    exit()


print("Testing Algorithms")

os.system("./crackingmain --num-queries=" + NUM_QUERIES + " --column-size=" + COLUMN_SIZE + " --column-number=" +  NUMBER_OF_COLUMNS +" --kdtree-threshold=" + KDTREE_THRESHOLD)
