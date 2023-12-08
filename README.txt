## Experiment Setup
This experiment is based on gcc.trace file. 
Total loads are 318197 and total stores are 197486. 

### Cache Size:
The total block number to examine in this experiment is 8192 blocks. 
The range of block size is as follows: 4, 16, 128 bytes 
Those numbers are chosen such that this experiment can examines different set types with wide ranges of variations. 
(They are enough to examine both small cache and large cache sets.)

### Associativity: 
Testing whether various associativity impact the result. Direct, set-associative, and fully associative mappings are examined. 
1. Direct mapping
2. 4-way
3. 8-way
4. 32-way
5. 64-way
6. Fully associative 

### Resulting cache dimensions:
* set number x block number for each set
* 8192 x 1 (direct mapping)
* 2048 x 4 
* 1024 x 8
* 256 x 32
* 128 x 64 
* 1 x 8192 (fully associative)

### 6 different combinations of parameters are also used:
1. write-allocate, write-through, lru
2. write-allocate, write-back, lru
3. no-write-allocate, write-through, lru
4. write-allocate, write-through, fifo
5. write-allocate, write-back, fifo
6. no-write-allocate, write-through, fifo

*Note that no-write-allocate and write-back combination is impossible.*

### Evaluation Criteria
Miss rate is calculated by 1-hit rate, where hit rate = number of hits / total number of cache hits and misses. 
There is a miss penalty of 100 cycles for every 4 byte data and a hit penalty of 1 cycle. 


## Result of 4 bytes block:
Miss penalty = 100 cycles
### 1. write-allocate, write-through, lru
|params\dim|8192*1 | 2048*4 | 1024*8 | 256*32| 128*64| 1*8192|
|----------|------|---------|--------|-------|-------|-------|
|Load hits |311740|313013|313054|313111|313102|313169|
|Load misses|6457| 5184|5143|5086|5095|5028|
|Store hits|169399 | 169929|169998|170006|170003|170025|
|Store misses|28087|27557|27488|27480|27483|27461|
|miss rate|0.067|0.063|0.063|0.063|0.063|0.063|
|total cycles|23753227|23571124|23560014|23553449|23554661|23545672|

### 2. write-allocate, write-back, lru
|params\dim|8192*1 | 2048*4 | 1024*8 | 256*32| 128*64| 1*8192|
|----------|------|---------|--------|-------|-------|-------|
|Load hits |311740|313013|313054|313111|313102|313169|
|Load misses|6457| 5184|5143|5086|5095|5028|
|Store hits|169399 | 169929|169998|170006|170003|170025|
|Store misses|28087|27557|27488|27480|27483|27461|
|miss rate|0.067|0.063|0.063|0.063|0.063|0.063|
|total cycles|6249027|5987724|5977914|5980049|5983461|5978872|

### 3. no-write-allocate, write-through, lru
|params\dim|8192*1    | 2048*4  | 1024*8 | 256*32 | 128*64 | 1*8192|
|----------|----------|---------|--------|--------|--------|-------|
|Load hits   |307354  |308528   |308550  |308587  |308588  |308588|
|Load misses |10843   | 9669    |9647    |9610    |9609    |5028|
|Store hits  |159171  | 159941  |159993  |159997  |37489  |170025|
|Store misses|38315   |37545    |37493   |37489   |27483   |27461|
|miss rate   |0.095   |0.092    |0.091   |0.091   |0.091   |0.063|
|total cycles|21359426|21240852 |21238630|21234893|21234792|5978872|

*Miss rate correctly represents the information about hits and misses, thereby load/store hits/misses are not shown for the following tables for the sake of conciseness.*
### 4. write-allocate, write-through, fifo
|params\dim|8192*1    | 2048*4  | 1024*8 | 256*32 | 128*64 | 1*8192|
|----------|----------|---------|--------|--------|--------|-------|
|miss rate   |0.067   |0.065    |0.064   |0.064   |0.064   |0.064|
|total cycles|23753227|23627482 |23613039|23607080|23606979|23578598|

### 5. write-allocate, write-back, fifo
|params\dim|8192*1    | 2048*4  | 1024*8 | 256*32 | 128*64 | 1*8192|
|----------|----------|---------|--------|--------|--------|-------|
|miss rate   |0.067   |0.065    |0.064   |0.064   |0.064   |0.091|
|total cycles|6249027|6077482 |6064939|6063380|6064979|6030998|

### 6. no-write-allocate, write-through, fifo
|params\dim|8192*1    | 2048*4  | 1024*8 | 256*32 | 128*64 | 1*8192|
|----------|----------|---------|--------|--------|--------|-------|
|miss rate   |0.095   |0.093    |0.092   |0.092   |0.092   |0.091 |
|total cycles|21359426|21271354 |21265193|21258628|21258628|21229439|

## Result of 16 bytes block:
Miss penalty = 100 cycles * (16/4) = 400 cycles
### 1. write-allocate, write-through, lru
|params\dim|8192*1    | 2048*4  | 1024*8 | 256*32 | 128*64 | 1*8192|
|----------|----------|---------|--------|--------|--------|-------|
|miss rate   |0.023   |0.022    |0.022   |0.022   |0.022   |0.022|
|total cycles|25073476|24824856 |24805608|24797187|24793979|23578598|

### 2. write-allocate, write-back, lru
|params\dim|8192*1    | 2048*4  | 1024*8 | 256*32 | 128*64 | 1*8192|
|----------|----------|---------|--------|--------|--------|-------|
|miss rate   |0.023   |0.022    |0.022   |0.022   |0.022   |0.022|
|total cycles|6638876|6039456 |5970608|5914187|5907379|5891764|

### 3. no-write-allocate, write-through, lru
|params\dim|8192*1    | 2048*4  | 1024*8 | 256*32 | 128*64 | 1*8192|
|----------|----------|---------|--------|--------|--------|-------|
|miss rate   |0.076   |0.074    |0.074   |0.074   |0.074   |0.074|
|total cycles|22872387|22672288 |22668278|22666674|22666674|22666273|

### 4. write-allocate, write-through, fifo
|params\dim|8192*1    | 2048*4  | 1024*8 | 256*32 | 128*64 | 1*8192|
|----------|----------|---------|--------|--------|--------|-------|
|miss rate   |0.023   |0.022    |0.022   |0.022   |0.022   |0.022|
|total cycles|25073476|24884605 |24871773|24857337|24856134|24852926|

### 5. write-allocate, write-back, fifo
|params\dim|8192*1    | 2048*4  | 1024*8 | 256*32 | 128*64 | 1*8192|
|----------|----------|---------|--------|--------|--------|-------|
|miss rate   |0.023   |0.022    |0.022   |0.022   |0.022   |0.022|
|total cycles|6638876|6134805 |6077173|6006737|5998334|5979126|

### 6. no-write-allocate, write-through, fifo
|params\dim|8192*1    | 2048*4  | 1024*8 | 256*32 | 128*64 | 1*8192|
|----------|----------|---------|--------|--------|--------|-------|
|miss rate   |0.076   |0.076    |0.074   |0.074   |0.074   |0.074 |
|total cycles|22872387|22683115 |22677501|22669882|22667476|22666273|


## Result of 128 bytes block:
Miss penalty = 100 cycles * (128/4) = 3200 cycles
### 1. write-allocate, write-through, lru
|params\dim|8192*1    | 2048*4  | 1024*8 | 256*32 | 128*64 | 1*8192|
|----------|----------|---------|--------|--------|--------|-------|
|miss rate   |0.0039   |0.0038    |0.0038   |0.0038   |0.0038   |0.0038|
|total cycles|26679087|26522238 |26525439|26503032|26503032|26503032|

### 2. write-allocate, write-back, lru
|params\dim|8192*1    | 2048*4  | 1024*8 | 256*32 | 128*64 | 1*8192|
|----------|----------|---------|--------|--------|--------|-------|
|miss rate   |0.0039   |0.0038    |0.0038   |0.0038   |0.0038   |0.0038|
|total cycles|7228087|6892038 |6885639|6754432|6754432|6754432|

### 3. no-write-allocate, write-through, lru
|params\dim|8192*1    | 2048*4  | 1024*8 | 256*32 | 128*64 | 1*8192|
|----------|----------|---------|--------|--------|--------|-------|
|miss rate   |0.0411   |0.041    |0.041   |0.041   |0.041   |0.041|
|total cycles|24848115|24790497 |24790497|24787296|24787296|24787296|

### 4. write-allocate, write-through, fifo
|params\dim|8192*1    | 2048*4  | 1024*8 | 256*32 | 128*64 | 1*8192|
|----------|----------|---------|--------|--------|--------|-------|
|miss rate   |0.0039   |0.0038    |0.0038   |0.0038   |0.0038   |0.0038|
|total cycles|26679087|26554248 |26541444|26503032|26503032|26503032|

### 5. write-allocate, write-back, fifo
|params\dim|8192*1    | 2048*4  | 1024*8 | 256*32 | 128*64 | 1*8192|
|----------|----------|---------|--------|--------|--------|-------|
|miss rate   |0.0039   |0.0038    |0.0038   |0.0038   |0.0038   |0.0038|
|total cycles|7228087|6936848 |6911244|6754432|6754432|6754432|

### 6. no-write-allocate, write-through, fifo
|params\dim|8192*1    | 2048*4  | 1024*8 | 256*32 | 128*64 | 1*8192|
|----------|----------|---------|--------|--------|--------|-------|
|miss rate   |0.0411   |0.041    |0.041   |0.041   |0.041   |0.041|
|total cycles|24848115|24793698 |24790497|24787296|24787296|24787296|



## Evaluation
In terms of miss rate, the lowest miss rate is achieved from the block size of 128 bytes. Parameters of 'write-allocate, write-through, lru', 'write-allocate, write-back, lru', 'write-allocate, write-through, fifo', and 'write-allocate, write-back, fifo' achieve the minimum value of 0.0038. Except the direct mapping, both set-associative and fully associative mappings achieve about the same miss rates. 

Among those which achieved the lowest miss rate, 'write-allocate, write-back, lru' and 'write-allocate, write-back, fifo' parameters have relatively lower numbers of cycles: ~7,228,087 cycles. Therefore, by looking at the miss rate and the number of cycle, one can conclude that set-associative and fully associative mappings with the block size of 128 bytes can achieve the best performance. 

The previous results shows a trend that the number of block size may be proportional to the decrease in miss rates. Therefore, one can infer that increasing the block size may achieve the best performance. However, this experiment also accounts for the miss penalty, which is proportional to the block sizes. 

In order to balance and to fully account for those factors, this experiment computes 'Average Memory Access Time' (AMAT):
`AMAT = Hit time + (Miss rate * Miss penalty)`. 
Here, hit time means the time ime to deliver a line in the cache to the processor. This experiment assumes the hit time to be 1 cycle. 

AMAT is calculated as follows. Note that the previous result shows that dimensions of set do impact little on the result, therefore 2048*4 is used to represent each parameter combinations:

|byte\params|1    | 2  | 3 | 4 | 5 | 6|
|----------|--------|---------|--------|--------|--------|-------|
|4   |7.3491 |7.3491|10.1556 | 7.4573  |7.4573   |10.2683|
|16 |26.3962|26.3962 |37.6225|26.8291|26.8291|38.0732|
|128 |204.1698|204.1698 |293.9800|207.6324|207.6324|297.5853|

The result shows that 'write-allocate write-through lru' and 'write-allocate write-back lru' achieve relatively a better results compare to the other parameter combinations. This also shows that smaller block size results in faster AMAT. Note that the first combination results in much larger cycles than the latter one: 23571124 vs. 5987724 cycles. Therefore, this experiment concludes that, putting a heavy influence on the results of the AMAT, the following command on the `gcc.trace` file results in the most efficient cache mapping performance: `./csim 2048 4 4 write-allocate write-through lru`. Note that the dimension of cache can vary, except direct mapping. 

Limitation of this experiment is that the result is heavily based on AMAT. One can redo the experiment using other metrics, including execution time, Memory stall cycles, and CPU time. 
