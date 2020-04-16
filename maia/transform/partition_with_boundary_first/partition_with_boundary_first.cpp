#include "maia/transform/partition_with_boundary_first/partition_with_boundary_first.hpp"

#include "maia/transform/partition_with_boundary_first/boundary_vertices.hpp"
#include "maia/transform/partition_with_boundary_first/boundary_vertices_at_beginning.hpp"
#include "maia/transform/partition_with_boundary_first/boundary_ngons_at_beginning.hpp"

#include "cpp_cgns/sids/Hierarchical_Structures.hpp"
#include "cpp_cgns/sids/Grid_Coordinates_Elements_and_Flow_Solution.hpp"
#include "cpp_cgns/sids/Multizone_Interface_Connectivity.hpp"


namespace cgns {


struct PointList_info {
    std::string zone_name = "";
    I4 identifier = 0; // since CGNS is 1-indexed, 0 is an *invalid* value
    tree* PointList = nullptr;
    tree* PointListDonor = nullptr;
};


auto
partition_with_boundary_first(tree& b, factory& F) -> void {
  STD_E_ASSERT(b.label=="CGNSBase_t");
  auto zs = get_children_by_label(b,"Zone_t");
  auto pl_infos = register_connectivities_PointList_infos(zs);

  for (auto& z : zs) {
    if (is_unstructured_zone(z)) {
      partition_zone_with_boundary_first(z,F);
    }
  }

  re_number_point_lists_donors(pl_infos);
}


auto
partition_zone_with_boundary_first(tree& z, factory& F) -> void {
  STD_E_ASSERT(z.label=="Zone_t");
  partition_coordinates(z);
  partition_elements(z,F);
}


auto
partition_coordinates(tree& z) -> void {
  STD_E_ASSERT(z.label=="Zone_t");
  if (!is_boundary_partitionned_zone(z)) {
    auto elt_pools = get_children_by_label(z,"Elements_t");
    auto boundary_vertex_ids = get_ordered_boundary_vertex_ids(elt_pools);
    permute_boundary_vertices_at_beginning(z,boundary_vertex_ids);
  }
}


auto
permute_boundary_vertices_at_beginning(tree& z, const std::vector<I4>& boundary_vertex_ids) -> void {
  STD_E_ASSERT(z.label=="Zone_t");
  auto nb_of_vertices = VertexSize_U<I4>(z);
  auto vertex_permutation = vertex_permutation_to_move_boundary_at_beginning(nb_of_vertices, boundary_vertex_ids);

  auto grid_coords = get_child_by_name(z,"GridCoordinates");
  auto elt_pools = get_children_by_label(z,"Elements_t");
  permute_boundary_grid_coords_at_beginning(grid_coords,vertex_permutation);
  update_vertex_ids_in_connectivities(elt_pools,vertex_permutation);

  I4 vertex_partition_point = boundary_vertex_ids.size();
  save_partition_point(z,vertex_partition_point);
}


auto
permute_boundary_grid_coords_at_beginning(tree& grid_coords, const std::vector<I4>& vertex_permutation) -> void {
  STD_E_ASSERT(grid_coords.label=="GridCoordinates_t");
  auto coords = get_children_by_label(grid_coords,"DataArray_t");
  for (tree& coord : coords) {
    permute_boundary_vertices(coord.value,vertex_permutation);
  }
}

auto
permute_boundary_vertices(node_value& coord, const std::vector<I4>& perm) -> void {
  auto coord_span = view_as_span<R8>(coord);
  std_e::permute(coord_span.begin(), perm);
};


auto
update_vertex_ids_in_connectivities(tree_range& elt_pools, const std::vector<I4>& vertex_permutation) -> void {
  /* Precondition */ for([[maybe_unused]] tree& elt_pool : elt_pools) { STD_E_ASSERT(elt_pool.label=="Elements_t"); }
  for(tree& elt_pool : elt_pools) {
    re_number_vertex_ids_in_elements(elt_pool,vertex_permutation);
  };
}


auto
save_partition_point(tree& z, I4 nb_of_boundary_vertices) -> void {
  STD_E_ASSERT(z.label=="Zone_t");
  VertexBoundarySize_U<I4>(z) = nb_of_boundary_vertices;
}


auto
partition_elements(tree& z, factory& F) -> void {
  STD_E_ASSERT(z.label=="Zone_t");
  auto elt_pools = get_children_by_label(z,"Elements_t");
  for (tree& elt_pool : elt_pools) {
    if (!is_boundary_partitionned_element_pool(elt_pool)) {
      if (ElementType<I4>(elt_pool)==NGON_n) {
        auto elements_permutation = permute_boundary_ngons_at_beginning(elt_pool,F);

        re_number_point_lists(z,ElementRange<I4>(elt_pool),elements_permutation);
      }
    }
  }
}


auto
re_number_point_lists(tree& z, const std_e::span<I4,2>& elt_range, const std::vector<I4>& elements_permutation) -> void {
  STD_E_ASSERT(z.label=="Zone_t");
  std_e::interval<I4> elt_interval = {elt_range[0],elt_range[1]+1}; // CGNS ranges are closed, C++ std ranges are semi-open

  for (tree& bc : get_nodes_by_matching(z,"ZoneBC/BC_t")) {
    tree& pl = get_child_by_name(bc,"PointList");
    re_number_point_list(pl.value,elements_permutation,elt_interval);
  }
  for (tree& gc : get_nodes_by_matching(z,"ZoneGridConnectivity/GridConnectivity_t")) {
    tree& pl = get_child_by_name(gc,"PointList");
    re_number_point_list(pl.value,elements_permutation,elt_interval);
  }
}


auto
re_number_point_list(node_value& point_list, const std::vector<I4>& elts_permutation, std_e::interval<I4> elt_range) -> void {
  // Precondition: elts_permutation is an index permutation (i.e. sort(permutation) == integer_range(permutation.size()))
  auto pl = view_as_span<I4>(point_list);
  update_ids_in_range_after_permutation(pl,elts_permutation,elt_range);
}


auto
register_connectivities_PointList_infos(tree_range& zones) -> std::vector<PointList_info> {
  auto pl_infos = register_PointLists_of_GridConnectivities(zones);

  bind_PointLists_to_their_PointListDonors(pl_infos,zones);

  return pl_infos;
}


auto
register_PointLists_of_GridConnectivities(tree_range& zones) -> std::vector<PointList_info> {
  std::vector<PointList_info> pl_infos;
  for (tree& z : zones) {
    for (tree& gc : get_nodes_by_matching(z,"ZoneGridConnectivity/GridConnectivity_t")) {
      tree& pl = get_child_by_name(gc,"PointList");
      auto pl_identifier = get_PointList_identifier(pl.value);
      pl_infos.push_back( PointList_info{z.name,pl_identifier,&pl,nullptr} );
    }
  }
  return pl_infos;
}


auto
bind_PointLists_to_their_PointListDonors(std::vector<PointList_info>& pl_infos, tree_range& zones) -> void {
  for (tree& z : zones) {
    for (tree& gc : get_nodes_by_matching(z,"ZoneGridConnectivity/GridConnectivity_t")) {
      std::string pld_zone_name = ZoneDonorName(gc);
      tree& pl_donor = get_child_by_name(gc,"PointListDonor");
      auto pld_identifier = get_PointList_identifier(pl_donor.value);

      auto associated_pl_info = 
        std::find_if(
          pl_infos.begin(),pl_infos.end(),[=](auto pl_info) {
            return (pl_info.zone_name == pld_zone_name) && (pl_info.identifier == pld_identifier);
          }
        );
      if (associated_pl_info == pl_infos.end()) {
        throw cgns_exception(
            "Incorrect intergrid connectivity: "
            "Unable to find PointList "
            "associated to GridConnectivity \"" + std::string(gc.name) + "\" "
            "of zone \"" + std::string(z.name) + "\""
        );
      } else {
        associated_pl_info->PointListDonor = &pl_donor;
      }
    }
  }
}


auto
get_PointList_identifier(const node_value& pl) -> I4 {
  return view_as_span<I4>(pl)[0];
}

auto
re_number_point_lists_donors(std::vector<PointList_info>& PointList_infos) -> void {
  for (auto& pl_info : PointList_infos) {
    auto pl = view_as_span<I4>(pl_info.PointList->value);
    auto pld = view_as_span<I4>(pl_info.PointListDonor->value);
    std::copy(pl.begin(),pl.end(),pld.begin());
  }
}


} // cgns
