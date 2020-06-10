#pragma once


#include "std_e/algorithm/algorithm.hpp"
#include <algorithm>
#include <tuple>
#include <numeric>
#include "cpp_cgns/sids/utils.hpp"
#include "std_e/algorithm/id_permutations.hpp"
#include "std_e/utils/time_logger.hpp"
#include "maia/connectivity_cgns/range.hpp"
#include "cpp_cgns/sids/Grid_Coordinates_Elements_and_Flow_Solution.hpp"
#include "cpp_cgns/sids/creation.hpp"
#include "cpp_cgns/tree_manip.hpp"


namespace cgns {


// DOC the returned boundary/interior partition is *stable*
template<class I> auto
boundary_interior_permutation(const md_array_view<I,2>& parent_elts) -> std::pair<std::vector<I>,I> {
  I nb_connec = parent_elts.extent(0);
  STD_E_ASSERT(parent_elts.extent(1)==2);

  // init
  std::vector<I> permutation(nb_connec);
  std::iota(begin(permutation),end(permutation),0);

  // permute
  auto connectivity_is_on_boundary = [&parent_elts](I i){ return is_boundary(parent_elts,i); };
  auto partition_ptr = std::stable_partition(permutation.begin(),permutation.end(),connectivity_is_on_boundary);
  I partition_index = partition_ptr - permutation.begin();

  return {permutation,partition_index};
}




template<class I> auto
create_partitionned_ngon_connectivities(std_e::span<I> old_connectivities, const std::vector<I>& permutation, I partition_index)
  -> std::pair<std::vector<I>,I> 
{
  std_e::time_logger _("create_partitionned_ngon_connectivities");

  // prepare accessors
  auto old_ngon_accessor = cgns::interleaved_ngon_random_access_range(old_connectivities);

  std::vector<I> new_connectivities(old_ngon_accessor.memory_length());
  auto new_ngon_accessor = cgns::interleaved_ngon_range(new_connectivities);

  // prepare partition segments
  auto permutation_first = permutation.begin();
  auto permutation_partition_point = permutation.begin() + partition_index;
  auto first_part_size = partition_index;
  auto second_part_size = permutation.size() - partition_index;

  // apply permutation copy at beginning
  auto new_ngon_partition_point_it =
  std_e::permute_copy_n(old_ngon_accessor.begin(),new_ngon_accessor.begin(),permutation_first          ,first_part_size);

  // record partition point in ngon
  I ngon_partition_index = new_ngon_partition_point_it.data() - new_ngon_accessor.begin().data();

  // finish permutation copy
  std_e::permute_copy_n(old_ngon_accessor.begin(),new_ngon_partition_point_it,permutation_partition_point,second_part_size);

  return {new_connectivities,ngon_partition_index};
}

template<class I> auto
apply_partition_to_ngons(std_e::span<I> old_ngon_cs, const std::vector<I>& permutation, I partition_index) -> I {
  auto [new_connectivities,ngon_partition_index]  = create_partitionned_ngon_connectivities(old_ngon_cs,permutation,partition_index);
  std::copy(new_connectivities.begin(),new_connectivities.end(),old_ngon_cs.begin());

  return ngon_partition_index;
}




template<class I> auto
apply_partition_to_parent_elts(md_array_view<I,2>& parent_elts, const std::vector<I>& permutation) -> void {
  std_e::permute(column(parent_elts,0).begin(),permutation);
  std_e::permute(column(parent_elts,1).begin(),permutation);
}




auto
mark_as_boundary_partitionned(tree& ngons, I8 partition_index, I8 ngon_partition_index, const factory& F) -> void {
  ElementSizeBoundary<I4>(ngons) = partition_index;

  node_value part_idx_val = create_node_value_1d({ngon_partition_index},F.alloc());
  tree pt_node = F.newUserDefinedData(".#PartitionIndex",part_idx_val);

  emplace_child(ngons,std::move(pt_node));
}


auto
permute_boundary_ngons_at_beginning(tree& ngons, const factory& F) -> std::vector<I4> { // TODO find a way to do it for I4 and I8
  // Precondition: ngons.type = "Elements_t" and elements of type NGON_n
  tree parent_elts_node = get_child_by_name(ngons,"ParentElements");
  auto parent_elts = view_as_md_array<I4,2>(parent_elts_node.value);
  tree ngon_connectivity = get_child_by_name(ngons,"ElementConnectivity");

  // compute permutation
  auto [permutation,partition_index] = boundary_interior_permutation(parent_elts);

  // apply permutation
  auto ngon_partition_index = apply_partition_to_ngons(view_as_span<I4>(ngon_connectivity.value),permutation,partition_index);
  apply_partition_to_parent_elts(parent_elts,permutation);

  mark_as_boundary_partitionned(ngons,partition_index,ngon_partition_index,F);

  return permutation;
}



} // cgns
