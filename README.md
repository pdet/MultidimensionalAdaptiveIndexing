# Multidimensional Adaptive Indexing (MDAI)
In this stand-alone implementation, we present various MDAI approaches and compare them to state of the art indexing strategies for multidimensional data through several different benchmarks.

## Algorithms
* [Vectorized Predicated Scans](https://pdfs.semanticscholar.org/2e84/4872e32a4a4e94e229a9a9e70ac47d710252.pdf)
* [Unidimensional Standard Cracking](http://stratos.seas.harvard.edu/files/IKM_CIDR07.pdf)
* [Sideways Cracking](http://delivery.acm.org/10.1145/1560000/1559878/p297-idreos.pdf)
* [Covered Index](http://delivery.acm.org/10.1145/2740000/2732229/p97-schuhknecht.pdf)
* [Quasii TBD](https://openproceedings.org/2018/conf/edbt/paper-153.pdf)
* Cracking KD-Tree
* [KD-Tree](http://delivery.acm.org/10.1145/370000/361007/p509-bentley.pdf)

## Benchmarks
We break down the time spent into query response time as the following:

* Index Creation: Time spent creating the index.
* Index Lookup: Time spent traversing the index.
* Scan: Time spent scanning the data.
* Intersection: For the unidimensional index, we need an extra step to intersect the values from the index to the table/other indexes since they are not aligned. We currently perform this intersection through the use of bitmaps.

### Uniform Random
This experiment generates multiple uniformly random columns and generates queries with fixed selectivities per column.
To edit the configurations of the script:
```bash
vi benchmarks/UniformRandom/Script.py
```
To execute the scrypt:
```bash
python benchmarks/UniformRandom/Script.py
```

### TPC-H
This experiment generates ...

```sql
 SELECT
    sum(l_extendedprice * l_discount) as revenue
FROM
    lineitem
WHERE
    l_shipdate >= date '1994-01-01'
    AND l_shipdate < date '1994-01-01' + interval '1' year
    AND l_discount between 0.06 - 0.01 AND 0.06 + 0.01
    AND l_quantity < 24;
```


### Genomes

### GIS

### Machine Learning Pipeline

## Tests
The following script test if all implemented algorithms are returning the correct result:
```bash
python benchmarks/test.py
```

##Third Parties Code
### Standard Cracking
...
### TPC-H DB-GEN
...

## Papers
### Cracking KD-Tree: The First Multidimensional Adaptive Indexing (Position Paper). P. Holanda, M. Nerone, E. C. de Almeida and S. Manegold @ DATA 2018