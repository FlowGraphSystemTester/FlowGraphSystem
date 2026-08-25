# FlowGraphSystem
FlowGraphSystem is a system designed in C++ for flow analysis in flow networks than can be used to generate test paths.

Before any flow calculations can be done a series of graph transformations take place. 
Using the initial input graph (either program's functions or non-orthogonal states in UML states machine) a list of prime paths is generated.
Then a new graph is generated that uses prime paths as vertices and all possible transitions between them
as new edges. After that all cycles in the graph are removed and replaced by a new vertex thereby obtaining DAG.
Lastly on each edge from DAG the additional information (such as lower bound, current/initial flow and upper bound) is added for flow calculations.

The flow calculation module implements Dinitz's algorithm, feasible flow algorithms, the minimum flow and maximum flow algorithms. 
The complete system is described in the accompanying [scientific publication]().

# Generating graphs from article
The input data for the graph in the article is located in the [test/data/graf-testowy-artykul.txt](./test/data/graf-testowy-artykul.txt) file. 
Instructions on how to pass data (input to stdin) to the program are described in [how to pass input section](#how-to-pass-input) below.
To generate graphs run `test` target in CMake.
The output images will be located in `test/data/article`
## Mathematical definition of the flow network
The system processes capacitated flow networks (*capacitated networks*), defined as the following tuple:

$$
N_c = (V, E, l, c, s, t),
$$

where:

* $V$ – the set of vertices,
* $E$ – the set of edges,
* $l(u, v)$ – the lower flow bound for each edge $(u, v) \in E$,
* $c(u, v)$ – the upper flow capacity for edge $(u, v) \in E$,
* $s \in V$ – the designated source vertex,
* $t \in V$ – the designated sink vertex.

The system includes cycle removal functionality, allowing it to process both directed cyclic graphs (*DCGs*) and directed acyclic graphs (*DAGs*).

Also, it is important to note that each vertex of this graph is either a prime path or artificial vertex created by removing the cycle and each vertex edge models possible transitions between all vertices.

## Requirements
- Operating System: Windows 10 or Windows 11 x86 64-bit (so that libraries' .dll files work)
- C++23 capable compilator: GCC, MSVC, Clang (tested on MinGW-gcc)
- CMake buildsystem: CMake 3.12 or greater (modern CMake)

## Used libraries,dataset and packaging
The project redistributes headers and libraries as part of the project for convenience and ease of distribution on modern x86 Windows. 
It does not require installing required libraries or configuring it.

This limits the ability to run this program on different platforms (Linux or Mac OSes or any different architecture of the CPU).
In the future this limitation can be overcome by doing proper packaging and installing dependencies automatically assuming that the libraries support them.

All libraries that are used in this project are open-source and free to use for non-commercial use.
This project requires [graphviz](https://graphviz.org/) library for rendering graph visualization and saving it to file.

Also, the [Boost Graph Library](https://www.boost.org/doc/libs/latest/libs/graph/doc/html/graph/index.html) is used for converting any graph to planar graph so that it can be drawn in more readable way.

For testing purposes the graph dataset from UJ was used which was described and used in paper describing [efficient prime path generation](https://arxiv.org/pdf/2602.18768)
## Instalation guide:
1. Download (clone) this repository
```bash
git clone https://github.com/FlowGraphSystemTester/FlowGraphSystem
```
2. Enter the cloned folder
```bash
cd FlowGraphSystem
```
3. Run CMake to compile and build the program (libraries should be automatically copied to executables' directory):
```bash
cmake -S . -B build
cmake --build build --config Release 
```

## How to use
This project build two executables:
- Main executable that transform initial graph and calculates minimal flow.
- Test executable that runs the test suite on UJ's graph dataset (a part of) containing over 700 example graphs.
Sections below describe the assumptions about the input data and what is described in the output of terminal (output data).

## How to pass input
The program reads the graph from standard input (stdin) in this format:
```txt
V E
S1 D1
S2 D2
SE DE
```

The graph is represented in the form of the incidence list read from the stdin. 
If you want to read from the file you need to redirect file to stdin (using pipes or bash redirection operators)

The first line contains metadata about the graph where $V$ is number of unique vertices and $E$ is number of edges.
Each next line contains an edge described by the source $S_i$ vertex and the destination (neighbor) $D_i$ vertex where `i` is edge index ($i <= E$).
The number of lines is equal to number of edges $E + 1$ (for metadata). 

Each vertex must be a number (int) from the range of $[0,V-1]$ assuming that $V$ won't overflow. 
It is also important to note that despite this broad range of variables, the first vertex ($S_0$,$D_0$) is reserved for source (special) vertex and the last vertex ($S_{V-1}$, $D_{V-1}$) is reserved for the sink (special) vertex
These specially vertices are not created automatically - it is responsibility of the user to manually add (to the edge list) an edge from source vertex to entry state/function vertex and an edge from exit state/function vertex to sink vertex. 
So the program assumes that the source and sink vertices belong to vertices set. 

Also, the program assumes that the vertex is a valid int value. If you need to pass graph where each vertex has alphanumeric name you
need to map (or find bijective function) each named vertex to unique vertex ID which is a valid int.

The final incidence list is initialized by adding empty `std::vector<int>` to `std::vector<std::vector<int>>` and then adding sequentially the incidence edges.
Therefore, an empty list of neighbors in the incidence list means that this vertex contains no neighboring vertices.

## How to interpret results - Program output
For a given initial input graph the programs prints the graph (incident list representation) in the standard output (stdout, terminal) after each graph transformation and returns minimum flow.
Also, the time of each graph transformation is measured allowing to benchmark the solution and then image saved to the file (png/svg).
