# HL-index
---
This work is a journal extension of our work accepted at ICDE 2026 with the title "HL-index: Fast Reachability Query in Hypergraphs".
We extend the previous work to adapt to the dynamic setting by investigating and implementing the incidence-level hypergraph insertion for the following paper
* Peiting Xie, Xiangjun Zai, Yanping Wu, Chen Chen, Xiaoyang Wang, Wenjie Zhang, Lu Qin, "Fast Reachability Query in Hypergraphs ", 2026.

### Dataset
---
All the datasets in this paper can be downloaded in [__cornell__](https://www.cs.cornell.edu/~arb/data/), [__konect__](http://konect.cc/networks/), and [__SNAP__](https://snap.stanford.edu/data/).
We provide one sample test data folder in /data.

### Compile
---
  
```
g++ -O3 graph.cpp HI.cpp main.cpp -o hl -std=c++17
```

### Preliminary
---
* Each line in the input hypergraph is a list of vertex ID, for example:
```
1 2 3 4
1 4 5
1 5 6
```
* The test code is in main.cpp
* The hypergraph reading procedures are implemented in graph.cpp

### Run/Test
To execute our Algorithm with hypergraph input file called __DatasetName.txt__, such DatasetName.txt should under src/data

run the code 
```
./run.sh
```

The result of the query time will be reported under 
```
__src/scale/DatasetName/querytime/100
```

100 refers to loading 100% of the dataset

The result of the update performance will be reported under 
```
__src/scale/DatasetName/insert.log
```


