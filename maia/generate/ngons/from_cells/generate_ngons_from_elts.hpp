#pragma once


#include "maia/generate/ngons/from_cells/unique_faces.hpp"
#include "maia/generate/ngons/from_cells/create_ngon.hpp"



namespace cgns {

auto generate_ngons_from_elts(tree& b, factory& F) -> void;
auto generate_ngons_from_elts(const tree_range& elt_pools, factory& F) -> tree;

auto
append_faces(ElementType_t ElementType, faces_heterogenous_container<std::int32_t>& all_faces, const std_e::span<const std::int32_t>& connectivities, std::int32_t first_elt_id) -> void;

template<ElementType_t ElementType> auto
append_faces_for_elt_type(std::integral_constant<ElementType_t,ElementType>, faces_heterogenous_container<std::int32_t>& all_faces, const std_e::span<const std::int32_t>& connectivities_, std::int32_t first_elt_id) -> void;


inline auto
append_faces(ElementType_t ElementType, faces_heterogenous_container<std::int32_t>& all_faces, const std_e::span<const std::int32_t>& connectivities, std::int32_t first_elt_id) -> void { // TODO std::int32_t -> template I
  std_e::switch_<all_basic_2D_and_3D_elements>(ElementType)
    .apply(LIFT(append_faces_for_elt_type), all_faces,connectivities,first_elt_id );
};


template<ElementType_t ElementType> auto
append_faces_for_elt_type(std::integral_constant<ElementType_t,ElementType>, faces_heterogenous_container<std::int32_t>& all_faces, const std_e::span<const std::int32_t>& connectivities_, std::int32_t first_elt_id) -> void { // TODO std::int32_t -> template I
  auto connectivities = fwd_connectivity_range<ElementType>(connectivities_);
  append_faces_from_connectivity_elts(all_faces,connectivities,first_elt_id);
}


} // cgns
