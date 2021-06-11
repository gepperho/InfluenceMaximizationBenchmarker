# InfluenceMaximizationBenchmarker

The InfluenceMaximizationBenchmarker (IMB) is a benchmark suite first published at GRADES-NDA 2021:

> Heiko Geppert, Sukanya Bhowmik, and Kurt Rothermel. 2021. Large-scale Influence Maximization with the Influence Maximization Benchmarker Suite. In 4th Joint International Workshop on Graph Data Management Experiences & Systems (GRADES) and Network Data Analytics (NDA) (GRADES-NDA’21), June 20–25, 2021, Virtual Event, China. ACM, New York, NY, USA, 11 pages. https://doi.org/10.1145/3461837.3464510

IMB provides easy to use implementation of the Influence Maximization (IM) problem.




## Building

### Dependencies

*  cmake (3.14 or higher)
*  clang (version?) or GCC (9.2 or higher)

### How to build

```
mkdir build
cd build
cmake -G "Unix Makefiles" -DBUILD_TESTS=<1|0> -DCMAKE_BUILD_TYPE=<Release|Debug> -DUSE_CLANG=<1|0> ..
make -j<number of cores>
```

## Running

Running Degree Discount and Update Approximation (UA) with a hop distance of 3 on the epinions vertex list. 

`./InfluenceMaximizationBenchmarker --graph ./data/epinions.txt --format 0 --algorithm degree,ua`

### Parameters
| short parameter | long parameter | type | description |
|---|---|---|---|
|-g | --graph | string | path to the graph file |
|-f| --format | int | format of the graph file. 0 = vertex list, 1 = edge list. Default=0 |
|-o | --output | string | path to an (optional) output file, where the result set is written |
|-s | --simulations | int | number of influence propagations simulations that are performed to evaluate the result set (only affects the evaluation). Default=20,000|
|-t | --threads | int | number of threads to be used (affects only some algrithms). Default=#CPU-cores|
|-k | --seeds | int | number of seed nodes to be searched (result set size) |
|-a | --algorithm | string | algorithms to be used. Multiple possible (comma separated). See table below for possible options.|
|-r | --raw | None | Flag to produce non pretty (raw) outputs. Great for machine parsing. |
|-i | --inverse | None | Flag, if set, the edge directions will be inversed. |

#### Algorithms
| original name | parameter name | arguments | 
|---|---|---|
| PageRank | pagerank | int: iterations, if none is given PR is performed until it converges (0.0001) |
| High Degree | highdegree | none |
| Degree Discount | degree | none |
| Random | random | none |
| CELF-Greedy | celf | int: number of MC simulations per vertex calculation, Default=10,000 |
| Influence Path Algorithm (IPA) | ipa | int: divider of the IPA threshold, Default=320 |
| Weighted Degree | wd | none |
| EaSyIM | easyim | int: hop distance, Default=3|
| EaSyIM-Delta | easyim-delta | int: hop distance, Default=3 |
| Update Approximation | ua | int: hop distance, Default=3 |
| IMM | imm | none |

Note: IMM is not sufficiently tested yet and should be used with caution.

#### original papers:
**CELF-Greedy:**
Jure Leskovec, Andreas Krause, Carlos Guestrin, Christos Faloutsos, Jeanne VanBriesen, and Natalie Glance. 2007. Cost-effective outbreak detection in networks. In <i>Proceedings of the 13th ACM SIGKDD international conference on Knowledge discovery and data mining</i> (<i>KDD '07</i>). Association for Computing Machinery, New York, NY, USA, 420–429. DOI:https://doi.org/10.1145/1281192.1281239

**Influence Path Algorithm (IPA):**
J. Kim, S. Kim and H. Yu, "Scalable and parallelizable processing of influence maximization for large-scale social networks?," 2013 IEEE 29th International Conference on Data Engineering (ICDE), 2013, pp. 266-277, doi: 10.1109/ICDE.2013.6544831.

**EaSyIM:** 
Sainyam Galhotra, Akhil Arora, and Shourya Roy. 2016. Holistic Influence Maximization: Combining Scalability and Efficiency with Opinion-Aware Models. In <i>Proceedings of the 2016 International Conference on Management of Data</i> (<i>SIGMOD '16</i>). Association for Computing Machinery, New York, NY, USA, 743–758. DOI:https://doi.org/10.1145/2882903.2882929

**Update Approximation:** Heiko Geppert, Sukanya Bhowmik, and Kurt Rothermel. 2021. Large-scale Influence Maximization with the Influence Maximization Benchmarker Suite. In 4th Joint International Workshop on Graph Data Management Experiences & Systems (GRADES) and Network Data Analytics (NDA) (GRADES-NDA’21), June 20–25, 2021, Virtual Event, China. ACM, New York, NY, USA, 11 pages. https://doi.org/10.1145/3461837.3464510


**IMM:** 
Youze Tang, Yanchen Shi, and Xiaokui Xiao. 2015. Influence Maximization in Near-Linear Time: A Martingale Approach. In <i>Proceedings of the 2015 ACM SIGMOD International Conference on Management of Data</i> (<i>SIGMOD '15</i>). Association for Computing Machinery, New York, NY, USA, 1539–1554. DOI:https://doi.org/10.1145/2723372.2723734





## Input Format

The input graphs can be provided in two formats: vertex lists and edge lists.
Examples can be found in the 'data' directory.

For details see [INPUT DOC](./doc/input.md).



# InfluenceMaximizationEvaluator

The InfluenceMaximizationEvaluator is a little extra tool to run the influence diffusion process on a given graph, using a given seed set.
Hence, the influence maximization strategy is skipped.
This can be used, if you want to verify or calculate the influence of a third party IM strategy using our diffusion implementation.

## Building

The InfluenceMaximizationEvaluator is build when the InfluenceMaximizationBenchmarker is build.
No additional cmmands needed.

## Running 

`./InfluenceMaximizationEvaluator --graph ../data/epinions.txt --format 0 -s 10000 --algorithm ua -p ../data/epinions_seed_set.txt`

The parameters are mostly equal to the InfluenceMaximizationBenchmarker.
Note the following new arguments:

| short parameter | long parameter | type | description |
|---|---|---|---|
|-p | --seedSetPath | string | path to the seed set file |
|-m | --metaData | None | Flag, if set the first line in the graph file will be skipped. Use if the first line (without escape signs like '#' or '%' holds the number of nodes and edges in the data set|

Also note, that the InfluenceMaximizationEvaluator uses the algorithm (-a) parameter only for documentation.
Hence, you can pass whatever name the algorihm that created the seed set had.
