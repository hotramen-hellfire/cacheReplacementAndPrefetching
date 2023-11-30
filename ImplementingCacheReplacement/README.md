# Lab-4 ChampSim

## Part I : Replacement Policies
In this part we will be implementing replacement policies and comparing them

- **LRU (Least Recently Used)** : Already implemented in the provided ChampSim
- **FIFO (First In First Out)** :  In this replacement policy, when the cache becomes full we evict the oldest cache block in the set
- **LFU (Least Frequently Used)** : We evict the cache line that is least frequently used, where frequency stands for the number of times the cache line is accessed(this could mean both read/write)
- **BIP (Binary Insertion Policy)** : Cache lines are inserted in _MRU_ position with some probability $\epsilon$ and in _LRU_ position with probability $1-\epsilon$. Cache lines in _LRU_ position are promoted to _MRU_ position only if they are accessed while being in _LRU_ position. Any line in _LRU_ position is evicted to make space for incoming line. For more details go through [this](https://www.cs.cmu.edu/afs/cs/academic/class/15740-f18/www/papers/isca07-qureshi-dip.pdf). Run it with $\epsilon = 0, \frac{1}{4}, \frac{1}{2}, \frac{3}{4}, 1$.

All the policies are to be used for L2C cache

Look inside [replacement](./ChampSim/replacement/) folder in ChampSim

### Comparison of Replacement Policies
We will be comparing `Speedup`, `L2C Miss Rate` of these replacement policies for `L2C` cache. 

Plot the `Speedup` and `L2C Miss Rate` values for different policies and traces. Use a multi-bar chart. Write your reasoning for the variations in these values in the report.
