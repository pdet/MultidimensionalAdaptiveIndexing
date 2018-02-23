import os
import sys

output = open('output.csv', 'w+')

output.write('algorithm;repetition;query_selectivity;query_zipf;query_number;query_pattern;column_size;column_pattern;bptree_elementspernode;number_of_columns;index_creation;index_lookup;scan_time;total_time\n')

if len(sys.argv) > 1:
	os.chdir(sys.argv[1])

def combine_results(directory = '../experimentresults/', base = ''):
	global first_file
	current_directory = os.path.join(base, directory)
	files = os.listdir(current_directory)
	for f in files:
		fullpath = os.path.join(current_directory, f)
		if os.path.isdir(fullpath):
			combine_results(f, current_directory)
		elif f == 'results.csv':
			with open(fullpath, 'r') as new_file:
				for line in new_file:
					if line[-1] != '\n':
						continue
					if line.count(';') == 14:
						splits = line.split(';')
						splits.insert(len(splits)-1, '')
						line = ';'.join(splits)
					if line.count(';') != 15:
						continue
					try:
						float(line.split(';')[-1])
						output.write(line)
					except:
						pass
			output.flush()


combine_results()
