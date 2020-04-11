#include "doctest/doctest.h"

#include "maia/generate/structured_grid_utils.hpp"

TEST_CASE("structured_block_cell_dims") {
  using MI = std_e::multi_index<int,3>;
  MI vertex_dims = {4,3,2};

  MI cell_dims = structured_block_cell_dims(vertex_dims);
  CHECK( cell_dims == MI{3,2,1} );
}

TEST_CASE("structured_block_face_dims_in_direction") {
  using MI = std_e::multi_index<int,3>;
  SUBCASE("one_quad") {
    MI vertex_dims = {2,2,2};
    auto nb_faces_i = structured_block_face_dims_in_direction(vertex_dims,0);
    auto nb_faces_j = structured_block_face_dims_in_direction(vertex_dims,1);
    auto nb_faces_k = structured_block_face_dims_in_direction(vertex_dims,2);

    CHECK( nb_faces_i == MI{2,1,1} );
    CHECK( nb_faces_j == MI{1,2,1} );
    CHECK( nb_faces_k == MI{1,1,2} );
  }
  SUBCASE("six_quads") {
    MI vertex_dims = {4,3,2};
    auto nb_faces_i = structured_block_face_dims_in_direction(vertex_dims,0);
    auto nb_faces_j = structured_block_face_dims_in_direction(vertex_dims,1);
    auto nb_faces_k = structured_block_face_dims_in_direction(vertex_dims,2);

    CHECK( nb_faces_i == MI{4,2,1} );
    CHECK( nb_faces_j == MI{3,3,1} );
    CHECK( nb_faces_k == MI{3,2,2} );
  }
}
