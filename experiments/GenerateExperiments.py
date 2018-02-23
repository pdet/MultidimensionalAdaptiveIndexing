import os
import inspect
#Setting Values For Distributions
RANDOM = 1
SEQUENTIAL = 2
SKEWED = 3

#Setting Values For Algorithms
FULL_SCAN = 0
STANDARD_CRACKING_AVL = 1
FULL_INDEX_B_TREE = 2
STANDARD_CRACKING_KD = 3
FULL_INDEX_KD = 4

GENERATE_EXPERIMENTS_PATH =  os.path.dirname(os.path.abspath(inspect.getfile(inspect.currentframe()))) # script directory
os.chdir(GENERATE_EXPERIMENTS_PATH)
os.chdir("../")
ROCKS_MACHINES = "./"
PATH_ON_ROCKS= "../"

#Default Configurations
NUM_QUERIES =  '1000'
NUMBER_OF_REPETITIONS =10
COLUMN_SIZE = '100000000'
QUERIES_PATTERN = RANDOM
COLUMN_PATTERN = RANDOM

BPTREE_ELEMENTSPERNODE = '16384' #Only used for Full Index

def generate_command(experiment_id,experiment,repetitions,query_pattern,query_selectivy,query_stream_size,column_size
                     ,bptree_element_per_node,number_of_columns):
    command= 'executer.'
    if experiment == FULL_SCAN:
        command = command + 'full_scan(' + str(experiment_id) + ',' + str(repetitions) + ','\
                  + str(query_pattern) + ',' + str(query_selectivy) + ',' + str(query_stream_size) + ',' \
                  + str(column_size) + ',' +  str(number_of_columns) + ')'

    if experiment == STANDARD_CRACKING_AVL:
        command = command + 'standard_cracking_avl(' + str(experiment_id) + ',' + str(repetitions) + ','\
                  + str(query_pattern) + ',' + str(query_selectivy) + ',' + str(query_stream_size) + ',' \
                  + str(column_size) + ',' +  str(number_of_columns) + ')'

    if experiment == FULL_INDEX_B_TREE:
        command = command + 'full_index_b_tree(' + str(experiment_id) + ',' + str(repetitions) + ','\
                  + str(query_pattern) + ',' + str(query_selectivy) + ',' + str(query_stream_size) + ',' \
                  + str(column_size) + ',' + str(number_of_columns) + ',' +  str(bptree_element_per_node) + ')'

    if experiment == STANDARD_CRACKING_KD:
        command = command + 'standard_cracking_kd(' + str(experiment_id) + ',' + str(repetitions) + ','\
                  + str(query_pattern) + ',' + str(query_selectivy) + ',' + str(query_stream_size) + ',' \
                  + str(column_size)  + ',' +  str(number_of_columns) + ')'

    if experiment == FULL_INDEX_KD:
        command = command + 'full_index_kd(' + str(experiment_id) + ',' + str(repetitions) + ','\
                  + str(query_pattern) + ',' + str(query_selectivy) + ',' + str(query_stream_size) + ',' \
                  + str(column_size) + ',' + str(number_of_columns) + ')'
    return command


#Generate Python File : rocksxxx_experiment_y.py
def generate_python_file(rocks_machine,experiment_number,commands_list):
    if os.path.exists("experiments/generatedscripts/") != 1:
        os.system('mkdir experiments/generatedscripts')
    file = open("experiments/generatedscripts/experiment_"+str(experiment_number)+'.py', "w")
    file.write("import sys \n")
    file.write("sys.path.append(\"./experiments\") \n")
    file.write("import ExecuteExperiment as executer \n")
    file.write("import os \n\n\n")
    file.write("def run(): \n")
    file.write("    os.environ[\'OPT\'] = \'true\' \n\n")
    file.write("    os.system(\'make\')\n\n")


    for commands in commands_list:
        file.write("    "+commands)
        file.write('\n\n')
    file.close()


#Setting Experiment_1
#All Related Work with the exception of Progressive STC
#All Workloads (Random -> Sequential -> Skewed)
#Selectivities from 0.1 to 0.9
# Columns from 2 pow(n) :/ 0<= n < 6
# 1000 queries
# 100.000.000 tuples
experiment_id = 0
def experiment1():
    global experiment_id
    EXPERIMENT_NUMBER=1
    EXPERIMENTS = [FULL_SCAN,STANDARD_CRACKING_AVL,FULL_INDEX_B_TREE,STANDARD_CRACKING_KD,FULL_INDEX_KD]
    QUERIES_PATTERN = [RANDOM]
    QUERIES_SELECTIVITY = [0.1]
    NUMBER_OF_COLUMNS = [2,4,8]
    QUERY_STREAM_SIZE = 1000
    COLUMN_SIZE = 10000000
    COMMANDS_LIST= []
    for experiment in EXPERIMENTS:
        for query_pattern in QUERIES_PATTERN:
            for query_selectivity in QUERIES_SELECTIVITY:
                for num_of_column in NUMBER_OF_COLUMNS:
                    experiment_id = experiment_id+1
                    COMMANDS_LIST.append(generate_command(experiment_id,experiment,NUMBER_OF_REPETITIONS,
                                                          query_pattern,query_selectivity,QUERY_STREAM_SIZE,COLUMN_SIZE,
                                                          BPTREE_ELEMENTSPERNODE,num_of_column))

    generate_python_file('',EXPERIMENT_NUMBER,COMMANDS_LIST)
    return COMMANDS_LIST

def run():
  experiment1()