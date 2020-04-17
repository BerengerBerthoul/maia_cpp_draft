#include "std_e/unit_test/doctest.hpp"

#include "maia/connectivity/heterogenous_connectivity_range.hpp"
#include "maia/connectivity/test/test_utils.hpp"
#include <algorithm>
#include <iostream>

using namespace maia;

TEST_CASE("heterogenous ngon connectivity") {
  const std::vector<int> polygon_offsets = {0          , 3              , 7       , 10};
  const std::vector<int> polygon_cs      = {100,101,102, 142,143,144,145, 44,45,46};

  SUBCASE("forward iteration") {
    std::vector<int> out_polygon_offsets(polygon_offsets.size());
    std::vector<int> out_polygon_cs     (polygon_cs     .size());
    out_polygon_offsets[0] = 0;

    auto connec_range = make_heterogenous_connectivity_range<heterogenous_polygon_kind>(polygon_offsets,polygon_cs);
    auto out_connec_range = make_heterogenous_connectivity_range<heterogenous_polygon_kind>(out_polygon_offsets,out_polygon_cs);

    std::copy(begin(connec_range),end(connec_range),begin(out_connec_range));
    CHECK( out_polygon_offsets == polygon_offsets );
    CHECK( out_polygon_cs == polygon_cs );
  }

  //SUBCASE("random_iterator") {
  //  using random_it = heterogenous_connectivity_random_access_iterator<const int,heterogenous_polygon_kind>;

  //  auto cs_fwd_range = make_heterogenous_connectivity_range<heterogenous_polygon_kind>(cs);
  //  auto idx_table = index_table(cs_fwd_range);

  //  int first_pos = 0;
  //  auto cs_random_it = random_it(cs.data(),idx_table,first_pos);

  //  auto c_0 = *cs_random_it;
  //  CHECK( c_0.size() == 3 );
  //  CHECK( c_0[0] == 100 );
  //  CHECK( c_0[1] == 101 );
  //  CHECK( c_0[2] == 102 );

  //  cs_random_it += 2;
  //  auto c_2 = *cs_random_it;
  //  CHECK( c_2.size() == 2 );
  //  CHECK( c_2[0] == 44 );
  //  CHECK( c_2[1] == 45 );

  //  cs_random_it += 0;
  //  auto still_c_2 = *cs_random_it;
  //  CHECK( still_c_2.size() == 2 );
  //  CHECK( still_c_2[0] == 44 );
  //  CHECK( still_c_2[1] == 45 );

  //  auto c_1 = *(--cs_random_it);
  //  CHECK( c_1.size() == 4 );
  //  CHECK( c_1[0] == 142 );
  //  CHECK( c_1[1] == 143 );
  //  CHECK( c_1[2] == 144 );
  //  CHECK( c_1[3] == 145 );
  //}

  //SUBCASE("random_access_range") {
  //  auto random_access_range = make_heterogenous_connectivity_random_access_range<heterogenous_polygon_kind>(cs);

  //  REQUIRE( random_access_range.size() == 3 );

  //  auto c_0 = random_access_range[0];
  //  CHECK( c_0.size() == 3 );
  //  CHECK( c_0[0] == 100 );
  //  CHECK( c_0[1] == 101 );
  //  CHECK( c_0[2] == 102 );

  //  auto c_1 = random_access_range[1];
  //  CHECK( c_1.size() == 4 );
  //  CHECK( c_1[0] == 142 );
  //  CHECK( c_1[1] == 143 );
  //  CHECK( c_1[2] == 144 );
  //  CHECK( c_1[3] == 145 );

  //  auto c_2 = random_access_range[2];
  //  CHECK( c_2.size() == 2 );
  //  CHECK( c_2[0] == 44 );
  //  CHECK( c_2[1] == 45 );
  //}
};
