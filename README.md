# Relevance queries for interval data

<p align="justify">A wide range of applications manage large collections of interval data. For instance, temporal databases manage validity intervals of objects or versions thereof, while in probabilistic databases attribute values of records are associated with confidence or uncertainty intervals. The main search operation on interval data is the retrieval of data intervals that intersect (i.e., overlap with) a query interval (e.g., find record versions which were valid at some point in September 2020). As query results can be overwhelmingly large, we need mechanisms that order them based on how relevant they are to the query interval. We define alternative relevance scores between a data and a query interval that intersect, based on their (relative) overlap. We define relevance queries, which compute only a subset of the most relevant intervals that intersect a query. Then, we propose a framework for evaluating relevance queries that can be applied on popular domain-partitioning interval indices (interval tree, HINT). We present experiments on real datasets that demonstrate the efficiency of our framework over baseline approaches.</p>

## Dependencies
- g++/gcc
- Boost Library 


## Datasets

The directory  ```inputs``` containts the BOOKS and BTC datasets used in the experiments; extract the archives before use.
- AARHUS-BOOKS_2013.txt
- BTC.txt


## Query sets

The directory ```queries``` containts the query files for the datasets in ```inputs``` datasets


## Build
Compile the source code using ```make all``` or ```make <option>``` where `<option>` can be one of the following:
   - theta_i 
   - theta_h 
   - topk_i
   - topk_h 


## Shared parameters among all methods
| Parameter | Description | Comment |
| ------ | ------ | ------ |
| -? or -h | display help message  | |
| -v | activate verbose mode; print the output trace for every query; otherwise only the final report is displayed | |
| -q | set the predicate type: currently only "gOVERLAPS" is supported | |
| -r | set the number of runs per query; omit option for default 1 | in the experiments set to 10 |
| -f | set the relevance definition" "ABSOLUTE" for <i>R<sub>a</sub>(s,q)</i>, "RELATIVE" for <i>R<sub>r</sub>(s,q)</i>, "DRELATIVE"  for <i>R<sub>rd</sub>(s,q)</i> or "QRELATIVE" for <i>R<sub>rq</sub>(s,q)</i>| in the experiments "RELATIVE", "DRELATIVE" and "QRELATIVE" were used |
| -t | set the relevance threshold for <i>θRelQuery</i> | in the experiments set inside {0.1, 0.3, 0.5, 0.7, 0.9}; default 0.5 |
| -k | set the number of requested results for <i>kRelQuery</i> | in the experiments set inside {3, 5, 10, 50, 100}; default 10 |
| -a | set the type of min/max (endpoint) bounds used: "UO" for uninformed, "IO" informed | used always in combination with the '-b' option, for the interval tree only informed bounds can be used |
| -b | set the type of reelvance bounds used: "LOWER" for lower, "UPPER" for upper, "BOTH" for lower and upper |  used always in combination with the '-a' option |


## Workloads
For <i>θRelQuery</i>, all methods return the number of intervals overlapping a query, while for <i>kRelQuery</i>, all methods return the collective relavance for <i>k</i> intervals with the highest relevance scores.


## Indexing and query processing methods

### Interval tree:

#### Source code files
- main_relevance-theta_intervaltree
- main_relevance-topk_intervaltree
- containers/relation.h
- containers/relation.cpp
- indices/intervaltree.h
- indices/intervaltree.cpp

#### Execution
| Extra parameter | Description | Comment |
| ------ | ------ | ------ |
| -s |  set the traversing strategy: "DEPTH-FIRST" for <i>θRelQuery</i> and "DEPTH-FIRST" or "BEST-FIRST" for <i>kRelQuery</i>; by default the depth-first strategy is used | omit option for default |

- ##### Examples for <i>θRelQuery</i>
    
    ```sh
    $ ./query_relevance-theta_intervaltree.exec -r 10 -f RELATIVE -t 0.5 inputs/AARHUS-BOOKS_2013.txt queries/AARHUS-BOOKS_2013_qe0.1%_qn10000.txt
    ```
    ```sh
    $ ./query_relevance-theta_intervaltree.exec -r 10 -f RELATIVE -t 0.5 -a IO -b LOWER inputs/AARHUS-BOOKS_2013.txt queries/AARHUS-BOOKS_2013_qe0.1%_qn10000.txt
    ```
    ```sh
    $ ./query_relevance-theta_intervaltree.exec -r 10 -f RELATIVE -t 0.5 -a IO -b UPPER inputs/AARHUS-BOOKS_2013.txt queries/AARHUS-BOOKS_2013_qe0.1%_qn10000.txt
    ```
    ```sh
    $ ./query_relevance-theta_intervaltree.exec -r 10 -f RELATIVE -t 0.5 -a IO -b BOTH inputs/AARHUS-BOOKS_2013.txt queries/AARHUS-BOOKS_2013_qe0.1%_qn10000.txt
    ```

- ##### Examples for <i>kRelQuery</i>  
    ```sh
    $ ./query_relevance-topk_intervaltree.exec -r 10 -f RELATIVE -k 10 inputs/AARHUS-BOOKS_2013.txt queries/AARHUS-BOOKS_2013_qe0.1%_qn10000.txt
    ```
    ```sh
    $ ./query_relevance-topk_intervaltree.exec -r 10 -f RELATIVE -k 10 -a IO -b UPPER inputs/AARHUS-BOOKS_2013.txt queries/AARHUS-BOOKS_2013_qe0.1%_qn10000.txt
    ```
    ```sh
    $ ./query_relevance-topk_intervaltree.exec -r 10 -f RELATIVE -k 10 -s BEST-FIRST -a IO -b UPPER inputs/AARHUS-BOOKS_2013.txt queries/AARHUS-BOOKS_2013_qe0.1%_qn10000.txt
    ```
    ```sh
    $ ./query_relevance-topk_intervaltree.exec -r 10 -f RELATIVE -k 10 -s BEST-FIRST -a IO -b BOTH inputs/AARHUS-BOOKS_2013.txt queries/AARHUS-BOOKS_2013_qe0.1%_qn10000.txt
    ```

### HINT<sup>m</sup>/HINT: 

#### Source code files
- main_hint_m.cpp
- containers/relation.h
- containers/relation.cpp
- indices/hierarchicalindex.h
- indices/hierarchicalindex.cpp
- indices/hint_m.h
- indices/hint_m_subs+sort+cm.cpp

#### Execution
| Extra parameter | Description | Comment |
| ------ | ------ | ------ |
| -m |  set the number of bits; if not set, a value will be automattically determined using the cost model in Christodoulou et al. @ ACM SIGMOD 2022 | in the experiments automatically set |
| -o |  set the HINT optimizations to be used: only "SUBS+SORT+CM" supported |
| -s |  set the traversing strategy: "BOTTOM-UP" for <i>θRelQuery</i> and "BOTTOM-UP" or "BEST-FIRST" for <i>kRelQuery</i>; by default the bottom-up strategy is used |  omit option for default |

- ##### Examples for <i>θRelQuery</i>

    ```sh
    $ ./query_relevance-theta_hint.exec -r 10 -f RELATIVE -t 0.5 inputs/AARHUS-BOOKS_2013.txt queries/AARHUS-BOOKS_2013_qe0.1%_qn10000.txt
    ```
    ```sh
    $ ./query_relevance-theta_hint.exec -r 10 -f RELATIVE -t 0.5 -a UO -b LOWER inputs/AARHUS-BOOKS_2013.txt queries/AARHUS-BOOKS_2013_qe0.1%_qn10000.txt
    ```
    ```sh
    $ ./query_relevance-theta_hint.exec -r 10 -f RELATIVE -t 0.5 -a UO -b UPPER inputs/AARHUS-BOOKS_2013.txt queries/AARHUS-BOOKS_2013_qe0.1%_qn10000.txt
    ```
    ```sh
    $ ./query_relevance-theta_hint.exec -r 10 -f RELATIVE -t 0.5 -a UO -b BOTH inputs/AARHUS-BOOKS_2013.txt queries/AARHUS-BOOKS_2013_qe0.1%_qn10000.txt
    ```
    ```sh
    $ ./query_relevance-theta_hint.exec -r 10 -f RELATIVE -t 0.5 -a IO -b LOWER inputs/AARHUS-BOOKS_2013.txt queries/AARHUS-BOOKS_2013_qe0.1%_qn10000.txt
    ```
    ```sh
    $ ./query_relevance-theta_hint.exec -r 10 -f RELATIVE -t 0.5 -a IO -b UPPER inputs/AARHUS-BOOKS_2013.txt queries/AARHUS-BOOKS_2013_qe0.1%_qn10000.txt
    ```
    ```sh
    $ ./query_relevance-theta_hint.exec -r 10 -f RELATIVE -t 0.5 -a IO -b BOTH inputs/AARHUS-BOOKS_2013.txt queries/AARHUS-BOOKS_2013_qe0.1%_qn10000.txt
    ```

- ##### Examples for <i>kRelQuery</i>  
    ```sh
    $ ./query_relevance-theta_hint.exec -r 10 -f RELATIVE -k 10 inputs/AARHUS-BOOKS_2013.txt queries/AARHUS-BOOKS_2013_qe0.1%_qn10000.txt
    ```
    ```sh
    $ ./query_relevance-theta_hint.exec -r 10 -f RELATIVE -k 10 -a UO -b UPPER inputs/AARHUS-BOOKS_2013.txt queries/AARHUS-BOOKS_2013_qe0.1%_qn10000.txt
    ```
    ```sh
    $ ./query_relevance-theta_hint.exec -r 10 -f RELATIVE -k 10 -a UO -b BOTH inputs/AARHUS-BOOKS_2013.txt queries/AARHUS-BOOKS_2013_qe0.1%_qn10000.txt
    ```    
    ```sh
    $ ./query_relevance-theta_hint.exec -r 10 -f RELATIVE -k 10 -a IO -b UPPER inputs/AARHUS-BOOKS_2013.txt queries/AARHUS-BOOKS_2013_qe0.1%_qn10000.txt
    ```
    ```sh
    $ ./query_relevance-theta_hint.exec -r 10 -f RELATIVE -k 10 -a IO -b BOTH inputs/AARHUS-BOOKS_2013.txt queries/AARHUS-BOOKS_2013_qe0.1%_qn10000.txt
    ```        
    ```sh
    $ ./query_relevance-theta_hint.exec -r 10 -f RELATIVE -k 10 -s BEST-FIRST -a UO -b UPPER inputs/AARHUS-BOOKS_2013.txt queries/AARHUS-BOOKS_2013_qe0.1%_qn10000.txt
    ```
    ```sh
    $ ./query_relevance-theta_hint.exec -r 10 -f RELATIVE -k 10 -s BEST-FIRST -a UO -b BOTH inputs/AARHUS-BOOKS_2013.txt queries/AARHUS-BOOKS_2013_qe0.1%_qn10000.txt
    ```
    ```sh
    $ ./query_relevance-theta_hint.exec -r 10 -f RELATIVE -k 10 -s BEST-FIRST -a IO -b UPPER inputs/AARHUS-BOOKS_2013.txt queries/AARHUS-BOOKS_2013_qe0.1%_qn10000.txt
    ```
    ```sh
    $ ./query_relevance-theta_hint.exec -r 10 -f RELATIVE -k 10 -s BEST-FIRST -a IO -b BOTH inputs/AARHUS-BOOKS_2013.txt queries/AARHUS-BOOKS_2013_qe0.1%_qn10000.txt
    ```


### Paper experiments

To reproduce experiments from the paper, execute the bash scripts found in the ```scripts``` directory. Report files are written in the ```outputs``` directory.

#### Vary query extent for <i>θRelQuery</i>

- ##### Example for BOOKS
    ```sh
    $ bash run_all_theta.sh BOOKS RELATIVE 10
    ```

#### Vary query extent for <i>kRelQuery</i>

- ##### Exampel for BOOKS
    ```sh
    $ bash run_all_topk.sh BOOKS RELATIVE 10
    ```
# code
