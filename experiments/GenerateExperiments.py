import os
import configuration as cfg
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

ROCKS_MACHINES = cfg.ROCKS_MACHINES
PATH_ON_ROCKS=cfg.PATH_ON_ROCKS

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
    if os.path.exists("generatedscripts/") != 1:
        os.system('mkdir generatedscripts')
    if os.path.isfile("generatedscripts/ExecuteExperiment.py") !=1:
        os.system('cp ExecuteExperiment.py generatedscripts/')
    if os.path.isfile("generatedscripts/configuration.py") != 1:
        os.system('cp configuration.py generatedscripts/')
    file = open("generatedscripts/"+rocks_machine+'_experiment_'+str(experiment_number)+'.py', "w")
    file.write("import ExecuteExperiment as executer \n")
    file.write("import os \n\n\n")
    file.write("os.environ[\'OPT\'] = \'true\' \n\n")
    file.write('os.chdir(\''+ PATH_ON_ROCKS +'MultidimensionalAdaptiveIndexing\') \n\n')
    file.write("os.system(\'make\')\n\n")
    for commands in commands_list:
        file.write(commands)
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
    QUERIES_PATTERN = [RANDOM,SEQUENTIAL,SKEWED]
    QUERIES_SELECTIVITY = [0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9]
    NUMBER_OF_COLUMNS = [1,2,4,8,16,32]
    QUERY_STREAM_SIZE = 1000
    COLUMN_SIZE = 100000000
    COMMANDS_LIST= []
    for experiment in EXPERIMENTS:
        for query_pattern in QUERIES_PATTERN:
            for query_selectivity in QUERIES_SELECTIVITY:
                for num_of_column in NUMBER_OF_COLUMNS:
                    experiment_id = experiment_id+1
                    COMMANDS_LIST.append(generate_command(experiment_id,experiment,NUMBER_OF_REPETITIONS,
                                                          query_pattern,query_selectivity,QUERY_STREAM_SIZE,COLUMN_SIZE,
                                                          BPTREE_ELEMENTSPERNODE,num_of_column))
    commands_per_machine = len(COMMANDS_LIST)/len(ROCKS_MACHINES)
    iterator = 0
    for machine in ROCKS_MACHINES:
        command_chunks = [COMMANDS_LIST[x:x + commands_per_machine]
                          for x in xrange(0, len(COMMANDS_LIST), commands_per_machine)]
        generate_python_file(machine,EXPERIMENT_NUMBER,command_chunks[iterator])
        iterator = iterator + 1
    return COMMANDS_LIST

experiment1()