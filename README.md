# GNN-on-CPU
Home for protoyping GNN algorithms on CPU



## TODO
An example for scratchpad C code to be able to identify the patterns for acceleration By Julian




## Bechmarking PageRank

High-Performance GPU Implementation of PageRank with Reduced Precision Based on Mantissa Segmentation, 2018
https://ieeexplore.ieee.org/document/8638359


* Partition-based algorithms: These algorithms aim to break down the graph into smaller partitions that can be processed independently, reducing the computational complexity. Examples include:
Graph partitioning: Techniques like METIS or KaHIP can be used to divide the graph into smaller subgraphs that can be processed in parallel. Each partition can be processed independently, and the results can be combined later.
BlockRank: This algorithm divides the graph into blocks and computes PageRank within each block independently. It uses a hierarchical approach to combine the results and achieve faster convergence.
* Approximation algorithms: These algorithms provide an approximate solution to the PageRank problem with reduced computational requirements. They sacrifice accuracy for speed. Some examples are:
Linear-System Solvers: By treating the PageRank computation as a linear system, iterative solvers like Gauss-Seidel or Jacobi can be used to solve the system approximately. These methods converge faster than the standard power iteration algorithm.
Randomized algorithms: Techniques such as random walk approximation or Monte Carlo sampling can be employed to estimate PageRank values. These algorithms provide fast approximations but with some loss in accuracy.
* Graph compression techniques: These methods aim to reduce the size of the graph representation, thus speeding up PageRank computation. Examples include:
Graph pruning: Removing low-weight or less influential nodes or edges from the graph can significantly reduce the computation required.
Graph summarization: Creating a condensed summary of the graph by identifying important subgraphs or communities can help accelerate PageRank computation.
* Distributed computing frameworks: Utilizing distributed computing frameworks can effectively parallelize PageRank computation across multiple machines. Some popular frameworks are:
Apache Hadoop: This framework allows for distributed processing of large datasets using a MapReduce paradigm, which can be applied to compute PageRank.
Apache Spark: Spark provides an efficient and fault-tolerant distributed computing framework. Its graph processing library, GraphX, can be used for PageRank computation.

### Graph benchmarks 

When selecting a benchmark dataset, consider the size, density, structure, and properties of the graph, as they can impact the performance and behavior of your PageRank algorithm. It's also valuable to use a combination of synthetic and real-world datasets to gain insights into the algorithm's performance in different scenarios.

* Graph500: Graph500 is a benchmark that measures the performance of graph algorithms on large-scale, irregular graph problems. It provides a scalable synthetic graph generator that can produce graphs with billions of edges.
* Stanford Large Network Dataset Collection (SNAP): SNAP provides a collection of real-world network datasets, including social networks, web graphs, collaboration networks, and more. These datasets capture the characteristics of various complex networks and can be used to evaluate the performance of PageRank algorithm on different types of graphs.
* The SuiteSparse Matrix Collection: While primarily focused on sparse matrices, the SuiteSparse Matrix Collection also includes graph datasets. It contains a diverse set of graphs, including social networks, web graphs, and biological networks. These datasets can be used to evaluate the performance of PageRank algorithm on different graph structures.
* LDBC Social Network Benchmark: The Linked Data Benchmark Council (LDBC) provides a set of benchmarks specifically designed for evaluating graph database systems. The LDBC Social Network Benchmark includes a synthetic social network graph generator and a set of queries, including PageRank, to evaluate the performance of graph algorithms on social network graphs.
* Web Data Commons Graph Benchmark: The Web Data Commons project offers a graph benchmark derived from the Common Crawl web corpus. It provides a large-scale web graph dataset that can be used to evaluate the performance of PageRank algorithms on web graphs.





