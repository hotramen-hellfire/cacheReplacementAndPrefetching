#include <algorithm>
#include <cassert>
#include <ctime>
#include <map>
#include <vector>
#define EPSILON 0.75
#include "cache.h"

namespace
{
std::map<CACHE*, std::vector<uint64_t>> num_times_used;
std::map<CACHE*, std::vector<bool>> color;
bool makeDraw()
{
  int p = 100 * EPSILON;
  int num = rand() % 100 + 1;
  if (num <= p) {
    return 1;
  } else
    return 0;
}
} // namespace

void CACHE::initialize_replacement()
{
  ::color[this] = std::vector<bool>(NUM_SET * NUM_WAY, 0);
  srand(time(0));
}

uint32_t CACHE::find_victim(uint32_t triggering_cpu, uint64_t instr_id, uint32_t set, const BLOCK* current_set, uint64_t ip, uint64_t full_addr, uint32_t type)
{
  if (EPSILON) {
    for (uint32_t i = set * NUM_WAY; i < (set + 1) * NUM_WAY; ++i) {
      if (::color[this][i] == 0) {
        ::color[this][i] = ::makeDraw();
        return i - set * NUM_WAY;
      }
    }
    for (uint32_t i = set * NUM_WAY; i < (set + 1) * NUM_WAY; ++i) {
      ::color[this][i] = 0;
    }
    ::color[this][set * NUM_WAY] = ::makeDraw();
    return 0;
  } else {
    return rand()%NUM_WAY;
  }
}

void CACHE::update_replacement_state(uint32_t triggering_cpu, uint32_t set, uint32_t way, uint64_t full_addr, uint64_t ip, uint64_t victim_addr, uint32_t type,
                                     uint8_t hit)
{
  if (!hit) {
    // do nothing sit back and chill
    return;
  }
  if (hit) {
    ::color[this][set * NUM_WAY + way] = 1;
    return;
  }
}

void CACHE::replacement_final_stats() {}
