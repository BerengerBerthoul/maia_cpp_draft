#include "std_e/unit_test/doctest.hpp"

#include "maia/connectivity_cgns/poly_elt_t_reference.hpp"
#include "maia/connectivity_cgns/connectivity_kind.hpp"
#include "maia/connectivity/heterogenous_connectivity_ref.hpp"



TEST_CASE("example use of poly_elt_t_reference and heterogenous_connectivity_ref: heterogenous connectivity range copy") {
  using CC_ref = heterogenous_connectivity_ref<const int,cgns::heterogenous_ngon_kind>;
  using C_ref = heterogenous_connectivity_ref<int,cgns::heterogenous_ngon_kind>;

// input
  // creation
  const std::vector<int> c_ngons_offsets = { 0            , 4       , 7 };
  const std::vector<int> c_ngons_cs      = { 110,101,99,98, 5,25,50 };

  // references to the connecvitivities
  cgns::poly_elt_t_reference<const int> c_elt_t_ref_0(&c_ngons_offsets[0]);
  cgns::poly_elt_t_reference<const int> c_elt_t_ref_1(&c_ngons_offsets[1]);

  CC_ref c_connec_ref_0 = {c_elt_t_ref_0,&c_ngons_cs[c_ngons_offsets[0]]};
  CC_ref c_connec_ref_1 = {c_elt_t_ref_1,&c_ngons_cs[c_ngons_offsets[1]]};

// output
  // initialize with size
  std::vector<int> ngons_offsets(c_ngons_offsets.size());
  std::vector<int> ngons_cs(c_ngons_cs.size());
  // the first offset is always 0
  ngons_offsets[0] = 0;

  // first connectivity
  /// ref to output first connectivity
  cgns::poly_elt_t_reference<int> elt_t_ref_0(&ngons_offsets[0]);
  C_ref connec_ref_0 = {elt_t_ref_0,&ngons_cs[ngons_offsets[0]]};
  /// assign first input to first output
  connec_ref_0 = c_connec_ref_0;
  /// check that the copy is correct
  CHECK( ngons_offsets[0] == 0 );
  CHECK( ngons_offsets[1] == 4 );
  CHECK( ngons_cs[0] == 110 );
  CHECK( ngons_cs[1] == 101 );
  CHECK( ngons_cs[2] ==  99 );
  CHECK( ngons_cs[3] ==  98 );

  // second connectivity
  /// ref to output second connectivity
  cgns::poly_elt_t_reference<int> elt_t_ref_1(&ngons_offsets[1]);
  C_ref connec_ref_1 = {elt_t_ref_1,&ngons_cs[ngons_offsets[1]]};
  /// assign first second to second output
  connec_ref_1 = c_connec_ref_1;
  /// check that the copy is correct
  CHECK( ngons_offsets[0] == 0 );
  CHECK( ngons_offsets[1] == 4 );
  CHECK( ngons_offsets[2] == 7 );
  CHECK( ngons_cs[4] ==  5 );
  CHECK( ngons_cs[5] == 25 );
  CHECK( ngons_cs[6] == 50 );

  // check that everything is correct
  CHECK( ngons_offsets == c_ngons_offsets );
  CHECK( ngons_cs == c_ngons_cs );
}
