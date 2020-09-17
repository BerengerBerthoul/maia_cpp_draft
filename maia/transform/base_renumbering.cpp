#include "maia/transform/base_renumbering.hpp"

//#include "cpp_cgns/sids/Multizone_Interface_Connectivity.hpp"


namespace cgns {


auto
register_connectivities_PointList_infos(tree& base, MPI_Comm comm) -> interzone_point_list_info {
  zone_exchange ze(base,comm);
  auto plds_by_zone = ze.point_list_neighbor_to_donor();
  return {std::move(ze),std::move(plds_by_zone)};
}

auto
re_number_point_lists_donors(interzone_point_list_info& pl_infos) -> void {
  pl_infos.ze.point_list_donor_to_neighbor(pl_infos.pld_by_z.plds);
}
//auto
//register_connectivities_PointList_infos(tree_range& zones) -> std::vector<PointList_info> {
//  auto pl_infos = register_PointLists_of_GridConnectivities(zones);
//
//  bind_PointLists_to_their_PointListDonors(pl_infos,zones);
//
//  return pl_infos;
//}
//
//
//auto
//register_PointLists_of_GridConnectivities(tree_range& zones) -> std::vector<PointList_info> {
//  std::vector<PointList_info> pl_infos;
//  for (tree& z : zones) {
//    for (tree& gc : get_nodes_by_matching(z,"ZoneGridConnectivity/GridConnectivity_t")) {
//      tree& pl = get_child_by_name(gc,"PointList");
//      auto pl_identifier = get_PointList_identifier(pl.value);
//      pl_infos.push_back( PointList_info{z.name,pl_identifier,&pl,nullptr} );
//    }
//  }
//  return pl_infos;
//}
//
//
//auto
//bind_PointLists_to_their_PointListDonors(std::vector<PointList_info>& pl_infos, tree_range& zones) -> void {
//  for (tree& z : zones) {
//    for (tree& gc : get_nodes_by_matching(z,"ZoneGridConnectivity/GridConnectivity_t")) {
//      std::string pld_zone_name = ZoneDonorName(gc);
//      tree& pl_donor = get_child_by_name(gc,"PointListDonor");
//      auto pld_identifier = get_PointList_identifier(pl_donor.value);
//
//      auto associated_pl_info = 
//        std::find_if(
//          pl_infos.begin(),pl_infos.end(),[=](auto pl_info) {
//            return (pl_info.zone_name == pld_zone_name) && (pl_info.identifier == pld_identifier);
//          }
//        );
//      if (associated_pl_info == pl_infos.end()) {
//        throw cgns_exception(
//            "Incorrect intergrid connectivity: "
//            "Unable to find PointList "
//            "associated to GridConnectivity \"" + std::string(gc.name) + "\" "
//            "of zone \"" + std::string(z.name) + "\""
//        );
//      } else {
//        associated_pl_info->PointListDonor = &pl_donor;
//      }
//    }
//  }
//}
//
//
//auto
//get_PointList_identifier(const node_value& pl) -> I4 {
//  return view_as_span<I4>(pl)[0];
//}
//
//auto
//re_number_point_lists_donors(std::vector<PointList_info>& PointList_infos) -> void {
//  for (auto& pl_info : PointList_infos) {
//    auto pl = view_as_span<I4>(pl_info.PointList->value);
//    auto pld = view_as_span<I4>(pl_info.PointListDonor->value);
//    std::copy(pl.begin(),pl.end(),pld.begin());
//  }
//}


} // cgns
