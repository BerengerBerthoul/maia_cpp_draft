#include "maia/transform/base_renumbering.hpp"

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

} // cgns
