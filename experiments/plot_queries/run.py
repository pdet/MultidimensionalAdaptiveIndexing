'''Plots the queries
'''
import os
import inspect
import argparse
import sys


# script directory
SCRIPT_PATH = os.path.dirname(
        os.path.abspath(
            inspect.getfile(inspect.currentframe())
            )
        )
os.chdir(SCRIPT_PATH)

CURRENT_DIR = os.getcwd()

# magic to be able to import benchmark from parent dir
sys.path.append(os.getcwd() + '/..')

from benchmark import Benchmark


# General experiment info
SELECTIVITY = '0.001'
NUMBER_OF_ROWS = f'{10e5}'
NUMBER_OF_QUERIES = '100'

EXPERIMENTS = [
        {
            "command": f"./uniform_generator -r {NUMBER_OF_ROWS} -d 2 -s {SELECTIVITY} -q {NUMBER_OF_QUERIES} -f /tmp/data -w {CURRENT_DIR}/data/uniform"
            },
        {
            "command": f"./skewed_generator -r {NUMBER_OF_ROWS} -d 2 -s {SELECTIVITY} -q {NUMBER_OF_QUERIES} -f /tmp/data -w {CURRENT_DIR}/data/skewed"
            },
        {
            "command": f"./sequential_generator -r {NUMBER_OF_ROWS} -d 2 -s {SELECTIVITY} -q {NUMBER_OF_QUERIES} -f /tmp/data -w {CURRENT_DIR}/data/sequential"
            },
        {
            "command": f"./periodic_generator -r {NUMBER_OF_ROWS} -d 2 -s {SELECTIVITY} -q {NUMBER_OF_QUERIES} -f /tmp/data -w {CURRENT_DIR}/data/periodic"
            },
        {
            "command": f"./zoom_in_generator -r {NUMBER_OF_ROWS} -d 2 -s {SELECTIVITY} -q {NUMBER_OF_QUERIES} -f /tmp/data -w {CURRENT_DIR}/data/zoom_in"
            },
        {
            "command": f"./zoom_out_generator -r {NUMBER_OF_ROWS} -d 2 -s {SELECTIVITY} -q {NUMBER_OF_QUERIES} -f /tmp/data -w {CURRENT_DIR}/data/zoom_out"
            },

        ]


RUNS = []


def main():
    ''' Main method
    '''
    build_dir = "../../build/"
    bin_dir = "../../bin"

    benchmark = Benchmark(EXPERIMENTS, RUNS, build_dir, bin_dir)
    benchmark.generate()

if __name__ == "__main__":
    main()
