import os
import subprocess
import json

class cd:
    """Context manager for changing the current working directory"""
    def __init__(self, newPath):
        self.newPath = os.path.expanduser(newPath)

    def __enter__(self):
        self.savedPath = os.getcwd()
        os.chdir(self.newPath)

    def __exit__(self, etype, value, traceback):
        os.chdir(self.savedPath)

BUILD_DIR = "../../build/"
BIN_DIR = "../../bin/"

with open('config.json') as json_file:
    config = json.load(json_file)

if not os.path.exists(BUILD_DIR):
    os.makedirs(BUILD_DIR)

# Compile the code
with cd(BUILD_DIR):
    subprocess.call(["cmake", ".."])
    subprocess.call(["make"])

with cd(BIN_DIR):
    # Generate data.sql and queries.sql
    subprocess.call([
        "./generator",
        "-w", str(config['benchmark']),
        "-d", str(config['number_of_attributes']),
        "-q", str(config['number_of_queries']),
        "-r", str(config['number_of_tuples']),
        "-s", str(config['selectivity']),
        "-t", str(config['query_type'])
    ])

    # Run all the algorithms, one at a time
    for algorithm in config['algorithms']:
        subprocess.call([
            "./main",
            "-w", "queries",
            "-d", "data",
            "-i", algorithm
        ])

# Run make clean
with cd(BUILD_DIR):
    subprocess.call(["make", "clean"])

