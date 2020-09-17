#include "maia/transform/remove_ghost_info.hpp"

#include "maia/transform/base_renumbering.hpp"
#include "cpp_cgns/sids/utils.hpp"
#include "cpp_cgns/sids/elements_utils.hpp"
#include "cpp_cgns/sids/Grid_Coordinates_Elements_and_Flow_Solution.hpp"
#include "cpp_cgns/sids/Building_Block_Structure_Definitions.hpp"
#include "std_e/algorithm/iota.hpp"
#include "maia/transform/renumber_point_lists.hpp"

namespace cgns {

auto
remove_ghost_info(tree& b, factory F, MPI_Comm comm) -> void {
  STD_E_ASSERT(b.label=="CGNSBase_t");
  //apply_base_renumbering(b,F,remove_ghost_info_from_zone,comm);
}


template<class I> auto
nb_elements(const tree& elt_pool) -> I {
  auto elt_range = ElementRange<I4>(elt_pool);
  return elt_range[1]-elt_range[0]+1; // CGNS ranges are closed
}
template<class I> auto
nb_ghosts(const tree& elt_pool) -> I {
  if (has_child_of_name(elt_pool,"Rind")) {
    auto elt_rind = Rind<I4>(elt_pool);
    STD_E_ASSERT(elt_rind[0]=0);
    return elt_rind[1];
  }
}
template<class I> auto
nb_owned_elements(const tree& elt_pool) -> I {
  return nb_elements<I>(elt_pool) - nb_ghosts<I>(elt_pool);
}

auto
remove_ghost_info_from_zone(tree& z, factory F) -> void {
//  tree_range elt_pools = get_children_by_label(z,"Elements_t");
//  std::sort(begin(elt_pools),end(elt_pools),compare_by_range);
//  STD_E_ASSERT(cgns::elts_ranges_are_contiguous(elt_pools));
//
//  // 0. compute permutation
//  I4 elt_first_id = ElementRange<I4>(elt_pools[0])[0];
//  std::vector<I4> permutation;
//  I4 current = 0;
//  for (const tree& elt_pool: elt_pools) {
//    I4 nb_owned_elts = nb_owned_elements<I4>(elt_pool);
//
//    std_e::iota_n(std::back_inserter(permutation), nb_owned_elts, current);
//    current += nb_owned_elts;
//    std::fill_n(std::back_inserter(permutation), nb_ghost_elts, -1-elt_first_id); // UGLY: -offset (here: elt_first_id) because just after, offset_permutation does +offset
//  }
//  std_e::offset_permutation perm(elt_first_id,permutation);
//
//  // 1. renum pl
//  renumber_point_lists(z,perm,"FaceCenter");
//  renumber_point_lists(z,perm,"CellCenter");
//
//  // 2. rm ghost cells
//  I4 elt_range_start = elt_first_id;
//  for (const tree& elt_pool: elt_pools) {
//    auto elt_range = ElementRange<I4>(elt_pool);
//    elt_range[1] = current_end;
//
//    I4 elt_type = ElementType<I4>(elt_pool);
//    elt_connec = get_child_by_name(z,"ElementConnectivity");
//    elt_connec.dims[0] -= nb_ghost_elts*number_of_nodes(elt_type);
//
//    F.rm_child_by_name(elt_pool,"Rind");
//  }
//
//  // 3. rm invalid pl (-1)
//  rm_invalid_ids_in_point_lists(z,"FaceCenter");
//  rm_invalid_ids_in_point_lists(z,"CellCenter");
//
//  // 4. gather all nodes used by elements
//  std::vector<I4> nodes;
//  for (const tree& elt_pool: elt_pools) {
//    auto cs = ElementConnectivity<I4>(elt_pool);
//    std::copy(begin(cs),end(cs),std::back_inserter(nodes));
//  }
//  std_e::sort_unique(nodes);
//  
//  // 5. delete unused nodes
//  
//  // 6. renum pl
}

} // cgns
