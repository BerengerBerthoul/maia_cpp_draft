#pragma once


#include "cpp_cgns/cgns.hpp"
#include "std_e/future/span.hpp"
#include "cpp_cgns/sids/creation.hpp"
#include "cpp_cgns/tree_manip.hpp"
#include "maia/parallel/neighbor_graph.hpp"
#include "maia/transform/donated_point_lists.hpp"


namespace cgns {

//// OLD
//struct PointList_info {
//    std::string zone_name = "";
//    I4 identifier = 0; // since CGNS is 1-indexed, 0 is an *invalid* value
//    tree* PointList = nullptr;
//    tree* PointListDonor = nullptr;
//};
//
//
//// declarations {
//template<class Fun> auto apply_base_renumbering(tree& b, factory F, Fun zone_renumbering, MPI_Comm) -> void;
//auto register_connectivities_PointList_infos(tree_range& zones) -> std::vector<PointList_info>;
//auto register_PointLists_of_GridConnectivities(tree_range& zones) -> std::vector<PointList_info>;
//auto bind_PointLists_to_their_PointListDonors(std::vector<PointList_info>& pl_infos, tree_range& zones) -> void;
//auto get_PointList_identifier(const node_value& pl) -> I4;
//auto re_number_point_lists_donors(std::vector<PointList_info>& PointList_infos) -> void;
//auto permute_boundary_vertices(node_value& coord, const std::vector<I4>& perm) -> void;
//// declarations }
//
//
//template<class Fun> auto
//apply_base_renumbering(tree& b, factory F, Fun zone_renumbering, MPI_Comm) -> void {
//  STD_E_ASSERT(b.label=="CGNSBase_t");
//  auto zs = get_children_by_label(b,"Zone_t");
//  auto pl_infos = register_connectivities_PointList_infos(zs);
//
//  for (auto& z : zs) {
//    zone_renumbering(z,F);
//  }
//
//  re_number_point_lists_donors(pl_infos);
//}
//// OLD


struct interzone_point_list_info {
  zone_exchange ze;
  point_list_donors_by_zone pld_by_z;
};

// TODO also return the GridLocation
inline auto find_point_list_donors(interzone_point_list_info& pl_infos, const std::string& z_name) -> donated_point_lists {
  // TODO use std::find_if (need iterators in jagged_range)
  donated_point_lists res;
  int nb_pl_donors = pl_infos.pld_by_z.donor_names.size();
  for (int i=0; i<nb_pl_donors; ++i) {
    if (pl_infos.pld_by_z.donor_names[i] == z_name) {
      res.locs.push_back(pl_infos.pld_by_z.locs[i]);
      res.pls.push_back(pl_infos.pld_by_z.plds[i]);
    }
  }
  return res;
}


// declarations {
template<class Fun> auto apply_base_renumbering(tree& b, factory F, Fun zone_renumbering, MPI_Comm comm) -> void; // TODO no default
auto register_connectivities_PointList_infos(tree& base, MPI_Comm comm) -> interzone_point_list_info;
auto re_number_point_lists_donors(interzone_point_list_info& pl_infos) -> void;
// declarations }


template<class Fun> auto
apply_base_renumbering(tree& b, factory F, Fun zone_renumbering, MPI_Comm comm) -> void {
  STD_E_ASSERT(b.label=="CGNSBase_t");
  auto zs = get_children_by_label(b,"Zone_t");
  auto pl_infos = register_connectivities_PointList_infos(b,comm);

  for (tree& z : zs) {
    auto z_plds = find_point_list_donors(pl_infos,z.name);
    zone_renumbering(z,z_plds,F);
  }

  re_number_point_lists_donors(pl_infos);
}


} // cgns
