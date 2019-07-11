# Multidimensional Adaptive Indexing (MDAI)

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

To be written

## To Do

* [X] Data Generator [Benchmark](https://www2.informatik.hu-berlin.de/~sprengsz/mdrq/#gmrqb)
* [X] Workload Generator [Benchmark](https://www2.informatik.hu-berlin.de/~sprengsz/mdrq/#gmrqb)
* [X] Implement Full Scan
* [X] Implement Testing procedures
* [X] Implement Cracking KD-Tree Broad
* [ ] Implement Cracking KD-Tree Narrow (Working on it)
* [ ] Implement KD-Tree
* [ ] Implement Quasii
* [ ] Implement Full Index B-Tree
* [ ] Implement a Row-Store Table to check if they have difference

## Papers

* Cracking KD-Tree: The First Multidimensional Adaptive Indexing (Position Paper). P. Holanda, M. Nerone, E. C. de Almeida and S. Manegold @ DATA 2018
