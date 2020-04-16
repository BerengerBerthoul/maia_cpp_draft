#include "std_e/unit_test/doctest.hpp"

#include "maia/connectivity_cgns/poly_elt_t_reference.hpp"
#include "maia/connectivity_cgns/connectivity_kind.hpp"
#include "maia/connectivity/heterogenous_connectivity_ref.hpp"



TEST_CASE("poly_elt_t_reference") {
  using const_connec_ref_type = heterogenous_connectivity_ref<const int,cgns::heterogenous_ngon_kind>;
  using       connec_ref_type = heterogenous_connectivity_ref<      int,cgns::heterogenous_ngon_kind>;

  const std::vector<int> c_ngons_offsets = { 0            , 4       , 7 };
  const std::vector<int> c_ngons_cs      = { 110,101,99,98, 5,25,50 };

  // swapped connectivities
        std::vector<int>   ngons_offsets = { 0      , 3             , 7 };
        std::vector<int>   ngons_cs      = { 5,25,50, 110,101,99,98 };

  cgns::poly_elt_t_reference<const int> c_elt_t_ref_0(&c_ngons_offsets[0]);
  cgns::poly_elt_t_reference<const int> c_elt_t_ref_1(&c_ngons_offsets[1]);
  cgns::poly_elt_t_reference<      int>   elt_t_ref_0(&  ngons_offsets[0]);
  cgns::poly_elt_t_reference<      int>   elt_t_ref_1(&  ngons_offsets[1]);

  const_connec_ref_type c_connec_ref_0 = {c_elt_t_ref_0,&c_ngons_cs[c_ngons_offsets[0]]};
  const_connec_ref_type c_connec_ref_1 = {c_elt_t_ref_1,&c_ngons_cs[c_ngons_offsets[1]]};
        connec_ref_type   connec_ref_0 = {  elt_t_ref_0,&  ngons_cs[  ngons_offsets[0]]};
        connec_ref_type   connec_ref_1 = {  elt_t_ref_1,&  ngons_cs[  ngons_offsets[1]]};

  connec_ref_0 = c_connec_ref_0;
  connec_ref_1 = c_connec_ref_1;

  CHECK( ngons_offsets == c_ngons_offsets );
  CHECK( ngons_cs == c_ngons_cs );
}
