#include "maia/transform/renumber_point_lists.hpp"

#include "std_e/future/contract.hpp"
#include "cpp_cgns/tree_manip.hpp"
#include "cpp_cgns/sids/Building_Block_Structure_Definitions.hpp"
#include <iostream> // TODO


namespace cgns {


auto
renumber_point_list(node_value& point_list, const std_e::offset_permutation<I4>& permutation) -> void {
  // Precondition: permutation is an index permutation (i.e. sort(permutation) == integer_range(permutation.size()))
  auto pl = view_as_span<I4>(point_list);
  std_e::apply(permutation,pl);
}

auto
renumber_point_lists(tree& z, const std_e::offset_permutation<I4>& permutation, const std::string& grid_location) -> void {
  STD_E_ASSERT(z.label=="Zone_t");
  std::vector<std::string> search_gen_paths = {"ZoneBC/BC_t","ZoneGridConnectivity/GridConnectivity_t"};
  for (tree& bc : get_nodes_by_matching(z,search_gen_paths)) {
    tree& pl = get_child_by_name(bc,"PointList");
    if (GridLocation(bc)==grid_location) {
      renumber_point_list(pl.value,permutation);
    }
  }
}


auto
rm_invalid_ids_in_point_list(node_value& point_list) -> void {
  // Precondition: permutation is an index permutation (i.e. sort(permutation) == integer_range(permutation.size()))
  auto pl = view_as_span<I4>(point_list);
  auto new_end = std::remove_if(begin(pl),end(pl),[](I4 i){ return i==-1; });
  I4 new_size = new_end - begin(pl);
  std::cout << "old size = " << point_list.dims[0] << "new size = " << new_size << "\n";
  point_list.dims[0] == new_size;
}

auto
rm_invalid_ids_in_point_lists(tree& z, const std::string& grid_location) -> void {
  STD_E_ASSERT(z.label=="Zone_t");
  std::vector<std::string> search_gen_paths = {"ZoneBC/BC_t","ZoneGridConnectivity/GridConnectivity_t"};
  for (tree& bc : get_nodes_by_matching(z,search_gen_paths)) {
    tree& pl = get_child_by_name(bc,"PointList");
    if (GridLocation(bc)==grid_location) {
      rm_invalid_ids_in_point_list(pl.value);
    }
  }
}


} // cgns
