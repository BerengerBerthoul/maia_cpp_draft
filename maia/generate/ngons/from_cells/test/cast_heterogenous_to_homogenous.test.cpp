#include "doctest/doctest.h"

#include "maia/generate/ngons/from_cells/cast_heterogenous_to_homogenous.hpp"

TEST_CASE("cast_heterogenous_to_homogenous") {
  const int quad_4_type_id = cgns::QUAD_4;
  std::vector<int> quad_data = {quad_4_type_id, 3,9,12,1};

  interleaved_connectivity_view<int,cgns::mixed_kind> quad_viewed_as_mixed(quad_data.data());

  auto quad = cgns::cast_as<cgns::QUAD_4>(quad_viewed_as_mixed);

  static_assert(quad.type==quad_4_type_id);
  static_assert(quad.size()==4);

  CHECK( quad[0] ==  3 );
  CHECK( quad[1] ==  9 );
  CHECK( quad[2] == 12 );
  CHECK( quad[3] ==  1 );
}
