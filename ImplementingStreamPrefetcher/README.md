# Lab-4 ChampSim

## Part II : Data Prefetcher

We will be implementing Stream Prefetcher in this part.

It works as follows:
1. The first miss, say to cache line `X`,
initiates a stream.
2. the second miss to cache line `X+Y` (or `X-Y`).
defines the direction of the stream in this case
3. The third miss, at `X+Z` (or `X-Z`) (where `Z>Y`), confirms the direction. Here `Z<PREFETCH_DISTANCE`.

- If X -> X+Y -> X+Z set stream direction as 1
- If X -> X-Y -> X-Z set stream direction as -1

Now we mark `X` as `start_addr` and `X+stream_dirn*PREFETCH_DISTANCE` as `end_addr`. This forms a monitoring region between either (`start_addr`, `end_addr`) or (`end_addr`, `start_addr`). Whenever we get miss in this monitoring region, we prefetch lines `end_addr+1`, `end_addr+2`, ... `end_addr+PREFETCH_DEGREE` or `end_addr-1`, `end_addr-2`, ... `end_addr-PREFETCH_DEGREE` depending on the stream direction. Also now we move monitoring region to either (`start_addr+PREFETCH_DEGREE`, `end_addr+PREFETCH_DEGREE`) or (`end_addr-PREFETCH_DEGREE`, `start_addr-PREFETCH_DEGREE`) depending on stream direction.

In short, after getting miss in the monitoring region
1. Prefetch lines `end_addr+stream_dirn*1`, `end_addr+stream_dirn*2`, ... `end_addr+stream_dirn*PREFETCH_DEGREE`
2. Move monitoring region as `start_addr += stream_dirn*PREFETCH_DEGREE` and `end_addr += stream_dirn*PREFETCH_DEGREE`

Maintain these monitoring regions in a table. When table gets filled and you need to evict one of the monitoring regions, use LRU policy to do so. Keep size of the table as 64.

Look inside [prefetcher](./ChampSim/prefetcher/) folder in ChampSim

![](.images/stream1.png)
![](.images/stream2.png)

Compare Stream Prefetcher with IP Stride prefetcher which is already implemented in the ChampSim provided. Both the prefetchers should be working at **L2** level.

Compare these prefetcher on following metrics:
- Speedup
- Prefetcher Accuracy
- L2C Load MPKI
- L1D MPKI

Plot bar graphs for each one of the above metrics

### Traces
Use the following [traces](https://drive.google.com/drive/folders/1BE4XkJhgXWZ6JOIByQCMF_4cFSvlb3f1?usp=sharing) for the simulations for above questions

To speedup simulation, run them parallely. You can use `multiprocessing` module of python to do this.

**Simulations should be done with `25M` Warmup and `25M` Simulation instructions**

## Submission
Submit only the following files:
- `FIFO.cc`
- `LFU.cc`
- `BIP.cc`
- `stream_prefetcher.cc`
- `report.pdf`
- `*.cc` (if you have implemented apart from these)

### Important Note
Unlike previous labs, in this part you are free to experiment and come up with improvements. It's completetly fine if you don't get improvements. You should mention what you tried in the report.

## Note

Speedup is the ratio of IPCs. In case of replacement policies divide the IPCs by IPC of LRU policy. In case of prefetcher divide the IPCs by IPC of IP Stride Prefetcher. This quantity tells us how fast have our improvements been compared to baseline. 

Prefetcher accuracy is `USEFUL/ISSUED`
