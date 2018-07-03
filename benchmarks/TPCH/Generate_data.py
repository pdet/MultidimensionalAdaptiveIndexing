import os
import pandas as pd
import numpy as np
import inspect

SCRIPT_PATH =  os.path.dirname(os.path.abspath(inspect.getfile(inspect.currentframe()))) # script directory
os.chdir(SCRIPT_PATH) # setting current dir as script path

# CONFIGURATIONS
SCALE_FACTOR = 1

def generate_lineitem():
    print("#### Generating lineitem table ####")
    os.chdir("tpch-dbgen")
    os.system("./dbgen -T L -f -s " + str(SCALE_FACTOR))
    os.chdir(SCRIPT_PATH)
    os.system("mv tpch-dbgen/lineitem.tbl .")

def fix_table_and_save_to_csv():
    print("#### Reading table ####")
    dateparse = lambda dates: [pd.datetime.strptime(d, '%Y-%m-%d') for d in dates]
    table = pd.read_csv('lineitem.tbl', sep='|', header=None, parse_dates=[10, 11, 12], date_parser=dateparse)
    # Columns 10, 11 and 12 are dates
    print("#### Transforming dates into integers ####")
    table[10] = table[10].values.astype(np.int64)
    table[11] = table[11].values.astype(np.int64)
    table[12] = table[12].values.astype(np.int64)
    print("#### Saving to CSV ####")
    table.to_csv('lineitem.csv', sep=';', header=False, index=False)

# SCRIPT START

generate_lineitem()
fix_table_and_save_to_csv()
print("#### Data generation complete ####")