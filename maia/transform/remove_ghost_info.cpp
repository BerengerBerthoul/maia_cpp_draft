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
  apply_base_renumbering(b,F,remove_ghost_info_from_zone,comm);
}


template<class I> auto
nb_elements(const tree& elt_pool) -> I {
  auto elt_range = ElementRange<I4>(elt_pool);
  return elt_range[1]-elt_range[0]+1; // CGNS ranges are closed
}
template<class I> auto
nb_ghost_elements(const tree& elt_pool) -> I {
  if (has_child_of_name(elt_pool,"Rind")) {
    auto elt_rind = Rind<I4>(elt_pool);
    STD_E_ASSERT(elt_rind[0]=0);
    return elt_rind[1];
  } else {
    return 0;
  }
}
template<class I> auto
nb_owned_elements(const tree& elt_pool) -> I {
  return nb_elements<I>(elt_pool) - nb_ghost_elements<I>(elt_pool);
}

auto
remove_ghost_info_from_zone(tree& z, donated_point_lists& plds, factory F) -> void {
  tree_range elt_pools = get_children_by_label(z,"Elements_t");
  std::sort(begin(elt_pools),end(elt_pools),compare_by_range);
  STD_E_ASSERT(cgns::elts_ranges_are_contiguous(elt_pools));
  int nb_elt_pools = elt_pools.size();

  // 0. compute permutation
  I4 elt_first_id = ElementRange<I4>(elt_pools[0])[0];
  std::vector<I4> permutation;
  std_e::knot_vector<I4> knots = {0};
  for (const tree& elt_pool: elt_pools) {
    I4 nb_owned_elts = nb_owned_elements<I4>(elt_pool);
    I4 nb_ghost_elts = nb_ghost_elements<I4>(elt_pool);

    std_e::iota_n(std::back_inserter(permutation), nb_owned_elts, knots.back());
    knots.push_back_length(nb_owned_elts);
    std::fill_n(std::back_inserter(permutation), nb_ghost_elts, -1-elt_first_id); // UGLY: -offset (here: elt_first_id) because just after, offset_permutation does +offset
  }
  std_e::offset_permutation perm(elt_first_id,permutation);

  // 1. renum pl
  renumber_point_lists(z,perm,"FaceCenter");
  renumber_point_lists(z,perm,"CellCenter");
  renumber_point_lists_donated(plds,perm,"FaceCenter");
  renumber_point_lists_donated(plds,perm,"CellCenter");

  // 2. rm ghost cells
  CellSize_U<I4>(z) = knots.length();
  for (int i=0; i<nb_elt_pools; ++i) {
    tree& elt_pool = elt_pools[i];
    auto elt_range = ElementRange<I4>(elt_pool);
    elt_range[0] = knots[i];
    elt_range[1] = knots[i+1]-1; // -1 because CGNS has closed intervals

    I4 elt_type = ElementType<I4>(elt_pool);
    tree& elt_connec = get_child_by_name(z,"ElementConnectivity");
    elt_connec.value.dims[0] = knots.length(i)*number_of_nodes(elt_type); // TODO alloc/copy/deallocate

    F.rm_child_by_name(elt_pool,"Rind");
  }


  // 4. gather all nodes used by elements
  std::vector<I4> nodes;
  for (const tree& elt_pool: elt_pools) {
    auto cs = ElementConnectivity<I4>(elt_pool);
    std::copy(begin(cs),end(cs),std::back_inserter(nodes));
  }
  std_e::sort_unique(nodes);

  tree& coords = get_child_by_name(z,"GridCoordinates");
  auto rind = view_as_span<I4>(get_child_by_name(coords,"Rind").value);
  auto first_ghost_node_id = VertexSize_U<I4>(z)+1 - rind[1];
  auto first_ghost_pos = std::lower_bound(begin(nodes),end(nodes),first_ghost_node_id);
  nodes.erase(first_ghost_pos,end(nodes));

  int nb_nodes = nodes.size();
  std_e::offset_permutation node_perm(1,std::move(nodes)); // CGNS nodes indexed at 1
  
  // 5. delete unused nodes
  VertexSize_U<I4>(z) = nb_nodes;

  F.rm_child_by_name(coords,"Rind");

  for (tree& coord : get_children_by_label(coords,"DataArray_t")) {
    auto old_coord_val = view_as_span<I4>(coord.value);
    auto new_coord_val = make_cgns_vector<I4>(nb_nodes,F.alloc());
    for (int i=0; i<nb_nodes; ++i) {
      new_coord_val[i] = old_coord_val[node_perm[i]];
    }
    F.deallocate_node_value(coord.value);
    coord.value = view_as_node_value(new_coord_val);
  }
  
// TODO move after pld have come back to their neighbor zone
  // 3. rm invalid pl (-1)
  //rm_invalid_ids_in_point_lists(z,"FaceCenter");
  //rm_invalid_ids_in_point_lists(z,"CellCenter");
  //rm_point_lists(z,"Vertex");
  // TODO check no invalid in PointListDonor (since they are donors, they cannot be ghost)
}

} // cgns
