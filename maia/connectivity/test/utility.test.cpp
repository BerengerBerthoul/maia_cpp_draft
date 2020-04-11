#include "doctest/doctest.h"

#include "maia/connectivity/utility.hpp"
#include "maia/connectivity_cgns/connectivity.hpp"

TEST_CASE("shift_vertices_ids") {
  std::vector<cgns::quad_4<int>> cs = {
    {42,43,44,45},
    { 5, 4, 3, 2},
    { 0, 2, 4, 6}
  };

  offset_vertices_ids(cs,1);

  std::vector<cgns::quad_4<int>> expected_cs = {
    {43,44,45,46},
    { 6, 5, 4, 3},
    { 1, 3, 5, 7}
  };
  
  CHECK( cs == expected_cs );
}
