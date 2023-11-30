#include <algorithm>
#include <array>
#include <map>
#include <optional>
#include <cassert>
#include <map>
#include <iostream>
#include <queue>
#include <utility>
#include "cache.h"

#define TABLE_SIZE  64
#define PREFETCH_DEGREE 32
#define PREFETCH_DISTANCE 2048
#define CONTINUE_THRESHOLD 1
#define AGGRESSIVE_THRESHOLD 4
#define KILL_THRESHOLD 8
#define BOSS_THRESHOLD 16
#define WEIGHT_PREAGRR 1
#define WEIGHT_AGGR 2
#define WEIGHT_KILL 4
#define WEIGHT_BOSS 8

namespace
{
    struct STREAM{
        public:
        uint64_t origin=0;
        bool direction=0;
        uint64_t sink=0;
        uint64_t c_start=0;
        uint64_t c_end=0;
        bool is_ready = 0;
        uint64_t num_hits = 0;
        uint64_t last_cycle =0;
    };

        //no problem in probing linearly as table size is small
    std::map<CACHE*, std::vector<struct STREAM*>> rvr;
    std::queue<std::pair<uint64_t, int>> requests;
    bool comparator (STREAM* a, STREAM* b)
    {
        return a->last_cycle < b->last_cycle;
    }
    int cT= CONTINUE_THRESHOLD; 
    int aT= AGGRESSIVE_THRESHOLD;
    int kT =KILL_THRESHOLD;
    int bT = BOSS_THRESHOLD;
    int pD = PREFETCH_DEGREE;
    std::vector<int> freqbinning;
};

void CACHE::prefetcher_initialize() {
        for (int i=0; i<10; i++)
        {::freqbinning.push_back(0);}
        for (int i=0; i<TABLE_SIZE; i++)
        {
            struct STREAM* pipe = new STREAM;
            ::rvr[this].push_back(pipe);
        }
        assert(::rvr[this].size()==TABLE_SIZE);
}

uint32_t CACHE::prefetcher_cache_operate(uint64_t addr, uint64_t ip, uint8_t cache_hit, bool useful_prefetch, uint8_t type, uint32_t metadata_in)
{
  int num_rvr = ::rvr[this].size();
    if (cache_hit) return metadata_in;
    for (int i=0; i<num_rvr; i++)
    {
        if (::rvr[this][i]->is_ready)
        {
            assert(::rvr[this][i]->c_start<::rvr[this][i]->c_end);
            if (addr>::rvr[this][i]->c_start && addr<::rvr[this][i]->c_end)
            {
                int num_prefetches = pD;
                ::rvr[this][i]->num_hits++;
                int freq_hits = ::rvr[this][i]->num_hits;
                ::rvr[this][i]->last_cycle=current_cycle;
                if (freq_hits<::cT) continue;
                if (freq_hits<::aT) num_prefetches = WEIGHT_PREAGRR*pD;
                if (freq_hits<::kT && freq_hits>::aT) num_prefetches = WEIGHT_AGGR*pD;
                if (freq_hits>::kT && freq_hits<::bT) num_prefetches = WEIGHT_KILL*pD;
                if (freq_hits>::bT) num_prefetches =  WEIGHT_BOSS*pD;
                if (::rvr[this][i]->direction)
                {
                        //we'll always prefetch to the L2 level
                        // prefetch lines sink+1, sink+2. . . sink+prefetch_degree
                        // for (int j=1; j<=num_prefetches; j++)
                        // {
                        //     this->prefetch_line((::rvr[this][i]->sink)+j, (this->get_mshr_occupancy_ratio() < 0.90), 0);
                        // }
                        ::requests.push({::rvr[this][i]->sink >> LOG2_BLOCK_SIZE , num_prefetches});
                        //now update the window
                        ::rvr[this][i]->origin += num_prefetches*BLOCK_SIZE;
                        ::rvr[this][i]->sink += num_prefetches*BLOCK_SIZE;
                        ::rvr[this][i]->c_start += num_prefetches*BLOCK_SIZE;
                        ::rvr[this][i]->c_end += num_prefetches*BLOCK_SIZE;
                        return metadata_in;
                }
                else 
                {
                        // for (int j=1; j<=num_prefetches; j++)
                        // {
                        //     this->prefetch_line((::rvr[this][i]->sink)-j, (this->get_mshr_occupancy_ratio() < 0.90), 0);
                        // }
                        ::requests.push({::rvr[this][i]->sink >> LOG2_BLOCK_SIZE, -1*num_prefetches});
                        //now update the window
                        ::rvr[this][i]->origin -= num_prefetches*BLOCK_SIZE;
                        ::rvr[this][i]->sink -= num_prefetches*BLOCK_SIZE;
                        ::rvr[this][i]->c_start -= num_prefetches*BLOCK_SIZE;
                        ::rvr[this][i]->c_end -= num_prefetches*BLOCK_SIZE;
                        return metadata_in;
                }
            }
            continue;
            return metadata_in;
        }
        else
        {
            if (::rvr[this][i]->origin==0)
            {
                ::rvr[this][i]->origin=addr;
                ::rvr[this][i]->last_cycle=current_cycle;
                return metadata_in;
            }
            if (::rvr[this][i]->sink==0)
            {
                if (addr > ::rvr[this][i]->origin)
                {
                    uint64_t diff = addr - ::rvr[this][i]->origin;
                    if (diff>PREFETCH_DISTANCE) continue;
                    ::rvr[this][i]->direction=true;
                    ::rvr[this][i]->sink=::rvr[this][i]->origin+PREFETCH_DISTANCE;
                }
                else
                {
                    uint64_t diff = ::rvr[this][i]->origin - addr;
                    if (diff>PREFETCH_DISTANCE) continue;
                    ::rvr[this][i]->direction=false;
                    ::rvr[this][i]->sink=::rvr[this][i]->origin-PREFETCH_DISTANCE;
                }//direction mapping works fine
                
                ::rvr[this][i]->last_cycle=current_cycle;
                return metadata_in;
            }
            if (::rvr[this][i]->sink!=0 && ::rvr[this][i]->origin!=0)
            {
                if (::rvr[this][i]->direction)
                {
                        if ((addr>=::rvr[this][i]->sink) && (addr-::rvr[this][i]->origin<=PREFETCH_DISTANCE))
                        {
                            assert(::rvr[this][i]->sink>::rvr[this][i]->origin);
                            ::rvr[this][i]->c_start = ::rvr[this][i]->origin;
                            ::rvr[this][i]->c_end = ::rvr[this][i]->sink;
                            assert(::rvr[this][i]->c_end>::rvr[this][i]->c_start);
                            ::rvr[this][i]->is_ready=1;
                            ::rvr[this][i]->num_hits+=1;
                            ::rvr[this][i]->last_cycle=current_cycle;
                            return metadata_in;
                        }
                        continue;
                }
                else
                {
                        if ((addr<=::rvr[this][i]->sink) && (::rvr[this][i]->origin-addr<=PREFETCH_DISTANCE))
                        {
                            assert(::rvr[this][i]->sink<::rvr[this][i]->origin);
                            ::rvr[this][i]->c_end = ::rvr[this][i]->origin;
                            ::rvr[this][i]->c_start = ::rvr[this][i]->sink;
                            assert(::rvr[this][i]->c_end>::rvr[this][i]->c_start);
                            ::rvr[this][i]->is_ready=1;
                            ::rvr[this][i]->num_hits+=1;
                            ::rvr[this][i]->last_cycle=current_cycle;
                            return metadata_in;
                        }
                        continue;
                }
                return metadata_in;
            }
        }
    }
    auto victim = std::min_element(::rvr[this].begin(), rvr[this].end(), comparator);
    uint32_t index =std::distance(rvr[this].begin(), victim);
    assert(index<TABLE_SIZE);
    assert(rvr[this][0]->last_cycle>=rvr[this][index]->last_cycle);
        // int freq = rvr[this][index]->num_hits;
        rvr[this][index]->origin=addr;
        rvr[this][index]->direction=0;
        rvr[this][index]->sink=0;
        rvr[this][index]->c_start=0;
        rvr[this][index]->c_end=0;
        rvr[this][index]->is_ready = 0;
        rvr[this][index]->num_hits = 0;
        rvr[this][index]->last_cycle =current_cycle;
  return metadata_in;
}

void CACHE::prefetcher_cycle_operate()
{ 
     if (::requests.empty()) return;
     if (::requests.front().second>0)
     {
        while(::requests.front().second>0)
        {
            this->prefetch_line(::requests.front().first << LOG2_BLOCK_SIZE, (this->get_mshr_occupancy_ratio() < 0.90), 0);
            ::requests.front().first++;
            ::requests.front().second--;
        }
        ::requests.pop();
        return;
     }
    if (::requests.front().second<0)
     {
        while (::requests.front().second<0)
        {
            this->prefetch_line(::requests.front().first << LOG2_BLOCK_SIZE, (this->get_mshr_occupancy_ratio() < 0.90), 0);
            ::requests.front().first--;
            ::requests.front().second++;
        }
        ::requests.pop();
        return;
     }
}

uint32_t CACHE::prefetcher_cache_fill(uint64_t addr, uint32_t set, uint32_t way, uint8_t prefetch, uint64_t evicted_addr, uint32_t metadata_in)
{
  return metadata_in;
}

void CACHE::prefetcher_final_stats() {

    std::cout<<"########### freqbinning stats\n"<<std::endl;
    for (int i=0; i<6; i++) std::cout<<::freqbinning[i]<<" ";
    std::cout<<"########### freqbinning end\n"<<std::endl;
    return;
}

