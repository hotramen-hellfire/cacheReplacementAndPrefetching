#include <algorithm>
#include <cassert>
#include <map>
#include <vector>

#include "cache.h"

namespace
{
std::map<CACHE*, std::vector<uint64_t>> num_times_used;
}

void CACHE::initialize_replacement() { ::num_times_used[this] = std::vector<uint64_t>(NUM_SET * NUM_WAY, 0); }

uint32_t CACHE::find_victim(uint32_t triggering_cpu, uint64_t instr_id, uint32_t set, const BLOCK* current_set, uint64_t ip, uint64_t full_addr, uint32_t type)
{
  auto begin = std::next(std::begin(::num_times_used[this]), set * NUM_WAY);
  auto end = std::next(begin, NUM_WAY);

  // Find the way whose last use cycle is most distant
  auto victim = std::min_element(begin, end);
  assert(begin <= victim);
  assert(victim < end);
  return static_cast<uint32_t>(std::distance(begin, victim)); // cast protected by prior asserts
}

void CACHE::update_replacement_state(uint32_t triggering_cpu, uint32_t set, uint32_t way, uint64_t full_addr, uint64_t ip, uint64_t victim_addr, uint32_t type,
                                     uint8_t hit)
{
  if (hit)
  {
    ::num_times_used[this][set * NUM_WAY + way]++;
    return; 
  }
  if (!hit)
  {
    ::num_times_used[this][set * NUM_WAY + way]=0;
    return;
  }//for verbosity
}

void CACHE::replacement_final_stats() {}
