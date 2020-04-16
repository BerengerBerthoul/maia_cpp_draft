#include "doctest/doctest.h"

#include "maia/connectivity/heterogenous_connectivity_ref.hpp"
#include "maia/connectivity/test/test_utils.hpp"

using namespace std;

using con_ref_type = heterogenous_connectivity_ref<int,my_ngon_connectivity_kind>;
using con_const_ref_type = heterogenous_connectivity_ref<const int,my_ngon_connectivity_kind>;

TEST_CASE("connectivity_ref") {
  int connectivity_type = 3;
  vector<int> vertices = {1,2,3};
  int* v_ptr = vertices.data();
  const int* v_const_ptr = vertices.data();

  con_const_ref_type con_const_ref(connectivity_type,v_const_ptr);
  con_ref_type       con_ref_type (connectivity_type,v_ptr      );

  SUBCASE("basic_tests") {
    CHECK( con_ref_type.type() == 3 );
    CHECK( con_ref_type.size() == 3 );
  }

  SUBCASE("equality") {
    int same_connectivity_type = 3;
    vector<int> same_vertices = {1,2,3};
    con_const_ref_type same(same_connectivity_type,same_vertices.data());
    CHECK( con_ref_type == same );

    int different_connectivity_type = 2;
    con_const_ref_type different_type(different_connectivity_type,same_vertices.data());
    CHECK( con_ref_type != different_type );

    vector<int> different_vertices = {2,3,1};
    con_const_ref_type different(same_connectivity_type,different_vertices.data());
    CHECK( con_ref_type != different );
  }


  SUBCASE("read") {
    CHECK( con_const_ref[0] == 1 );
    CHECK( con_const_ref[1] == 2 );
    CHECK( con_const_ref[2] == 3 );
  }
      
  SUBCASE("write") {
    con_ref_type[0] = 10;
    con_ref_type[1] = 11;
    con_ref_type[2] = 12;

    CHECK( con_const_ref[0] == 10 );
    CHECK( con_const_ref[1] == 11 );
    CHECK( con_const_ref[2] == 12 );
  }

  SUBCASE("begin_and_end") {
    CHECK( con_ref_type.begin() == vertices.data()  );
    CHECK( con_ref_type.end()   == vertices.data()+3);
  }
}