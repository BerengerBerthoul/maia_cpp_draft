#pragma once


#include "cpp_cgns/cgns.hpp"
#include "std_e/future/span.hpp"
#include "cpp_cgns/sids/creation.hpp"
#include "cpp_cgns/tree_manip.hpp"


namespace cgns {


struct PointList_info {
    std::string zone_name = "";
    I4 identifier = 0; // since CGNS is 1-indexed, 0 is an *invalid* value
    tree* PointList = nullptr;
    tree* PointListDonor = nullptr;
};


// declarations {
template<class Fun> auto apply_base_renumbering(tree& b, factory F, Fun zone_renumbering) -> void;
auto register_connectivities_PointList_infos(tree_range& zones) -> std::vector<PointList_info>;
auto register_PointLists_of_GridConnectivities(tree_range& zones) -> std::vector<PointList_info>;
auto bind_PointLists_to_their_PointListDonors(std::vector<PointList_info>& pl_infos, tree_range& zones) -> void;
auto get_PointList_identifier(const node_value& pl) -> I4;
auto re_number_point_lists_donors(std::vector<PointList_info>& PointList_infos) -> void;
auto permute_boundary_vertices(node_value& coord, const std::vector<I4>& perm) -> void;
// declarations }


template<class Fun> auto
apply_base_renumbering(tree& b, factory F, Fun zone_renumbering) -> void {
  STD_E_ASSERT(b.label=="CGNSBase_t");
  auto zs = get_children_by_label(b,"Zone_t");
  auto pl_infos = register_connectivities_PointList_infos(zs);

  for (auto& z : zs) {
    zone_renumbering(z,F);
  }

  re_number_point_lists_donors(pl_infos);
}


} // cgns
