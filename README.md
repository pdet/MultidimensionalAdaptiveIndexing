# Multidimensional Adaptive Indexing (MDAI) ![Build](https://github.com/pdet/MultidimensionalAdaptiveIndexing/workflows/CI/badge.svg)


In this stand-alone implementation, we present various MDAI approaches and compare them to state of the art indexing strategies for multidimensional data through several different benchmarks.

## Algorithms

<!-- * [Vectorized Predicated Scans](https://pdfs.semanticscholar.org/2e84/4872e32a4a4e94e229a9a9e70ac47d710252.pdf) -->
* Full Scan
* [Sideways Cracking](http://delivery.acm.org/10.1145/1560000/1559878/p297-idreos.pdf)
* [Covered Index](http://delivery.acm.org/10.1145/2740000/2732229/p97-schuhknecht.pdf)
* [Quasii](https://openproceedings.org/2018/conf/edbt/paper-153.pdf)
* Cracking KD-Tree Broad
* Cracking KD-Tree Narrow
* [KD-Tree](http://delivery.acm.org/10.1145/370000/361007/p509-bentley.pdf)

## How to run

Run the following command on the root of the project, it will guide you through selecting the experiment and algorithms to run. Then it will provide instructions on what to do next.

```
python3 run_me.py
```

## To Do

* [X] Data Generator [Benchmark](https://www2.informatik.hu-berlin.de/~sprengsz/mdrq/#gmrqb)
* [X] Workload Generator [Benchmark](https://www2.informatik.hu-berlin.de/~sprengsz/mdrq/#gmrqb)
* [X] Implement Full Scan
* [X] Implement Testing procedures
* [X] Implement Cracking KD-Tree Broad
* [X] Make both KD-Trees update their statistics (height, number of nodes, min_height)
* [ ] Implement Cracking KD-Tree Narrow (Need to fix bugs and improve its speed)
* [X] Implement KD-Tree with median
* [X] Implement KD-Tree with average
* [X] Implement Quasii
* [ ] Implement Full Index B-Tree
* [ ] Implement Regular Cracking using B-Tree
* [ ] Implement a Row-Store Table to check if they have difference
* [ ] Check "Bitwise dimensional co-clustering for analytical workloads"

## Papers

* Cracking KD-Tree: The First Multidimensional Adaptive Indexing (Position Paper). P. Holanda, M. Nerone, E. C. de Almeida and S. Manegold @ DATA 2018
