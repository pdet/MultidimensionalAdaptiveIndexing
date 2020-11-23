# Multidimensional Adaptive/Progressive Indexing

This project is a stand-alone implementation of all the current adaptive and progressive multidimensional indexing algorithms.

# Requirements
[CMake](https://cmake.org) to be installed and a `C++11` compliant compiler. Python 3.7 and Jupyter Notebooks are necessary to run the scripted experiments and their respective plots.

# Available Indexing Algorithms
* Adaptive KD-Tree
* Progressive KD-Tree
* Greedy Progressive KD-Tree
* [Quasii](https://openproceedings.org/2018/conf/edbt/paper-153.pdf)
* Full KD-Tree (Median Pivoting)
* Full KD-Tree (Average Pivoting)
* Full Scan (Candidate List)

# Available Datasets & Workloads
We use three real datasets, each with their own respective workload, and one synthetic dataset following a uniform random distribution. For the synthetic dataset, we also provide 8 different synthetic workloads
## Real
All real datasets and workloads were made publically available [here](https://zenodo.org/record/3835562).
### Skyserver
The Sloan Digital Sky Survey is a project to map the universe. Their data and queries are publicly available at their [website](http://skyserver.sdss.org). The data set we use here consists of two columns, era, and dec, from the photoobjall table with approximately 69 million tuples. The workload consists of 100,000 real range queries executed on those two attributes.
### Power
The power benchmark consists of sensor data collected from a manufacturing installation, obtained from the [DEBS 2012 challenge](https://debs.org/grand-challenges/2012/). The data set has three dimensions and 10 million tuples. The workload consists of random close-range queries on each dimension.
### Genomics 
The 1000 Genomes Project collects data regarding human genomes. It consists of 10 million genomes, described in 19 dimensions. The workload consists of queries performed by bio-informaticians.

## Synthetic 
The synthetic data set follows a uniform data distribution for each attribute in the table, consisting of 4-byte floating-point numbers in the range of \[0, 3 * 10^7), where 3 * 10^7 is the number of tuples. We use eight different synthetic workloads in our performance comparison. All workloads consist of queries with filters in all available dimensions, the follow the below form:
```sql
SELECT SUM(R.A) FROM R WHERE (R.A BETWEEN a_low AND a_high) and (R.B BETWEEN b_low AND b_high) ...
```
The values for low and high are chosen based on the workload pattern. The different workload patterns and their mathematical description are depicted below.
<img src="https://github.com/pholanda/MultidimensionalAdaptiveIndexing/blob/master/img/workloads.png" />

# Running the experiments
### Compile
First, we compile the code using release (-O3) mode
```bash
cmake -DCMAKE_BUILD_TYPE=Release && make
```

### Download Data
For the real data, you must execute the download_datasets.py script in the root directory to automatically download all datasets and workloads. You need approximately 50Gb of free disk space.
```bash
./python3 download_datasets.py
```

### Running Experiments
All experiments are inside the experiments folder. There are 3 main sets of experiments. Note that unless stated otherwise, all algorithms run with min_partition_size = 1024, and all progressive indexing algorithms run with delta = 0.2.
* real-data-workload: Runs all algorithms on the three real datasets and their respective workloads.
* synthetic_workloads: Runs all algorithms on the uniform random dataset with 7 different synthetic workloads (shifting workload is the first 10 queries of uniform repeated multiple times).
* uniformdata-delta: Compares how the delta parameter influences the Progressive KD-Tree on a uniform random dataset & workload (delta ranges from 0.1 to 1).
Every folder contains three main files, run.py, config.json, and plots.ipynb. If you want to change how certain experiments run, change the config.json file.
To run an experiment, you must cd to its folder and execute the run.py file.
Example:
```bash
cd experiments/real-data-workload
python3 run.py
```

Beforing plotting, make sure you have the necessary packages installed:
First make sure you have pip installed:

``` bash
pip --version
```

Should return version information about pip.

Next, install the necessary packages to plot the figures and tables:

``` bash
pip install jupyterlab numpy pandas plotly matplotlib
```

After running each experiment, each folder contains a python notebook you can use to plot and analyze the results. For that, you must initiate the jupyter notebook server at the desired folder
```bash
jupyter notebook
```


# Papers
* [Cracking KD-Tree: The First Multidimensional Adaptive Indexing (Position Paper) @ DATA 2018](https://pdet.github.io/assets/papers/MultCracking.pdf)
