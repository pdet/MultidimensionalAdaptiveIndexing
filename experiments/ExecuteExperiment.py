import os


FULL_SCAN = 0
STANDARD_CRACKING_AVL = 1
FULL_INDEX_B_TREE = 2
STANDARD_CRACKING_KD = 3
FULL_INDEX_KD = 4

#Default Configurations
COLUMN_FILE_PATH = "./column.txt"
QUERIES_FILE_PATH = "./query.txt"
ONE_SIDED_PERCENTAGE = 0.0 # Not really using this
ZIPF_ALPHA = 2.0 #Skewness of dataset
COLUMN_PATTERN = '1' #Column is always uniform random

#Empty Variables
EXPERIMENT_ID = ''
ALGORITHM = ''
QUERY_SELECTIVITY = ''
QUERY_ZIPF = ''
QUERY_RESULT = ''
QUERY_PATTERN = ''
QUERY_STREAM_SIZE = ''
COLUMN_SIZE = ''
BPTREE_ELEMENTSPERNODE = ''
NUMBER_OF_COLUMNS = ''

PATH_ON_ROCKS="./"
#Output is a csv file with:
#algorithm;repetition;query_selectivity;query_zipf;query_number;query_pattern;column_size;column_pattern;
#bptree_elementspernode;number_of_columns;query_time
def generate_output(file,query_result,repetition):
    query_result = query_result.split("\n")
    for query in range(0, len(query_result)-1):
        file.write(ALGORITHM + ';' + str(repetition) + ";" + str(QUERY_SELECTIVITY) + ";" + str(QUERY_ZIPF) +";"+ str(query)
        + ';' + str(QUERY_PATTERN) + ';' + str(COLUMN_SIZE) + ';' + str(COLUMN_PATTERN) + ';'
        + str(BPTREE_ELEMENTSPERNODE) + ';' + str(NUMBER_OF_COLUMNS) + ';' + query_result[query])
        file.write('\n')

def create_output():
    # Saving Experiments
    experiments_path = PATH_ON_ROCKS+'experimentresults/'+str(EXPERIMENT_ID)+'/'
    if os.path.exists(experiments_path) != 1:
        os.system('mkdir -p ' + experiments_path)

    header = "algorithm;repetition;query_selectivity;query_zipf;query_number;query_pattern;column_size;column_pattern;" \
    "bptree_elementspernode;number_of_columns;index_creation;index_lookup;scan_time;total_time"
    file = open(experiments_path + "results.csv", "w")
    file.write(header)
    file.write('\n')
    return file

def close_output(file):
    file.close()

def generate_data():
    UPPERBOUND = COLUMN_SIZE
    os.system("./gendata" + " " + COLUMN_FILE_PATH + " " + QUERIES_FILE_PATH + " " + str(QUERY_SELECTIVITY)
    + " " + str(ONE_SIDED_PERCENTAGE) + " " + str(ZIPF_ALPHA) + " " + str(QUERY_STREAM_SIZE) + " "
    + str(COLUMN_SIZE) + " " + str(UPPERBOUND) + " " + str(QUERY_PATTERN) + " " + str(COLUMN_PATTERN) + " " 
    + str(NUMBER_OF_COLUMNS))


def setGlobalVariables(experiment_id='', query_selectivity='', query_pattern='', column_size= '', algorithm='',
                       query_stream_size = '',number_of_columns= '', bptree_element_per_node = ''):
    global EXPERIMENT_ID
    EXPERIMENT_ID = experiment_id
    global QUERY_PATTERN
    QUERY_PATTERN = query_pattern
    global QUERY_STREAM_SIZE
    QUERY_STREAM_SIZE = query_stream_size
    global QUERY_SELECTIVITY
    QUERY_SELECTIVITY = query_selectivity
    global COLUMN_SIZE
    COLUMN_SIZE = column_size
    global ALGORITHM
    ALGORITHM = algorithm
    global BPTREE_ELEMENT_PER_NODE
    BPTREE_ELEMENT_PER_NODE = bptree_element_per_node
    global NUMBER_OF_COLUMNS
    NUMBER_OF_COLUMNS = number_of_columns

def full_scan(experiment_id,repetitions,query_pattern,query_selectivity,query_stream_size,column_size,number_of_columns):
    setGlobalVariables(experiment_id=experiment_id,query_selectivity=query_selectivity,query_pattern=query_pattern,
    query_stream_size = query_stream_size,column_size=column_size,algorithm='fs',number_of_columns=number_of_columns)
    file = create_output()
    generate_data()
    for repetition in range (0,repetitions):
        result = os.popen("./crackingmain" + " " + COLUMN_FILE_PATH + " " + QUERIES_FILE_PATH + " "
        + str(QUERY_STREAM_SIZE) + " " + str(COLUMN_SIZE) + " " + str(FULL_SCAN) + " " + str(NUMBER_OF_COLUMNS)).read()
        generate_output(file,result,repetition)
    close_output(file)

def standard_cracking_avl(experiment_id,repetitions,query_pattern,query_selectivity,query_stream_size,column_size,number_of_columns):
    setGlobalVariables(experiment_id=experiment_id,query_selectivity=query_selectivity,query_pattern=query_pattern,
    query_stream_size = query_stream_size,column_size=column_size,algorithm='stdavl',number_of_columns=number_of_columns)
    file = create_output()
    generate_data()
    for repetition in range (0,repetitions):
        result = os.popen("./crackingmain" + " " + COLUMN_FILE_PATH + " " + QUERIES_FILE_PATH + " " + str(QUERY_STREAM_SIZE)
        + " " + str(column_size) + " " + str(STANDARD_CRACKING_AVL)+ " " + str(NUMBER_OF_COLUMNS)).read()
        generate_output(file, result, repetition)
    close_output(file)

def standard_cracking_kd(experiment_id,repetitions,query_pattern,query_selectivity,query_stream_size,column_size,number_of_columns):
    setGlobalVariables(experiment_id=experiment_id,query_selectivity=query_selectivity,query_pattern=query_pattern,
    query_stream_size = query_stream_size,column_size=column_size,algorithm='stdkd',number_of_columns=number_of_columns)
    file = create_output()
    generate_data()
    for repetition in range (0,repetitions):
        result = os.popen("./crackingmain" + " " + COLUMN_FILE_PATH + " " + QUERIES_FILE_PATH + " " + str(QUERY_STREAM_SIZE)
        + " " + str(column_size) + " " + str(STANDARD_CRACKING_KD)+ " " + str(NUMBER_OF_COLUMNS)).read()
        generate_output(file, result, repetition)
    close_output(file)

def full_index_b_tree(experiment_id,repetitions,query_pattern,query_selectivity,query_stream_size,column_size,number_of_columns,
bptree_element_per_node):
    setGlobalVariables(experiment_id=experiment_id,query_selectivity=query_selectivity,query_pattern=query_pattern,
    query_stream_size = query_stream_size,column_size=column_size,algorithm='fibpt'
    ,bptree_element_per_node = bptree_element_per_node,number_of_columns=number_of_columns)
    file = create_output()
    generate_data()
    for repetition in range (0,repetitions):
        result = os.popen(
            "./crackingmain" + " " + COLUMN_FILE_PATH + " " + QUERIES_FILE_PATH + " " + str(QUERY_STREAM_SIZE)
            + " " + str(column_size) + " " + str(FULL_INDEX_B_TREE)+ " " + str(NUMBER_OF_COLUMNS) + " " + str(BPTREE_ELEMENT_PER_NODE)).read()
        generate_output(file, result, repetition)
    close_output(file)

def full_index_kd(experiment_id,repetitions,query_pattern,query_selectivity,query_stream_size,column_size,number_of_columns):
    setGlobalVariables(experiment_id=experiment_id,query_selectivity=query_selectivity,query_pattern=query_pattern,
    query_stream_size = query_stream_size,column_size=column_size,algorithm='fikd',number_of_columns=number_of_columns)
    file = create_output()
    generate_data()
    for repetition in range (0,repetitions):
        result = os.popen(
            "./crackingmain" + " " + COLUMN_FILE_PATH + " " + QUERIES_FILE_PATH + " " + str(QUERY_STREAM_SIZE)
            + " " + str(column_size) + " " + str(FULL_INDEX_KD) + " " + str(NUMBER_OF_COLUMNS)).read()
        generate_output(file, result, repetition)
    close_output(file)
