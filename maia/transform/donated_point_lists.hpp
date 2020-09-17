#pragma once

#include <vector>
#include "std_e/future/span.hpp"
#include "cpp_cgns/sids/Building_Block_Structure_Definitions.hpp"

namespace cgns {

// PointList values coming from a neighbor zone (that this neighbor zone stores in a GridConnectivity/PointListDonor)
// TODO multi_range
struct donated_point_lists {
  std::vector<cgns::GridLocation_t> locs; 
  std::vector<std_e::span<I4>> pls;
};

} // cgns
