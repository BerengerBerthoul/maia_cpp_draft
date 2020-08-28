#include "maia/parallel/neighbor_graph.hpp"
#include "cpp_cgns/node_manip.hpp"
#include "cpp_cgns/tree_manip.hpp"
#include "std_e/utils/vector.hpp"
#include "std_e/parallel/mpi.hpp"
#include "std_e/log.hpp"
#include "std_e/utils/to_string.hpp"
#include "tmp_cgns/exchange/part_to_block.hpp"
#include "tmp_cgns/exchange/block_to_part.hpp"


namespace cgns {


auto
paths_of_all_mentionned_zones(const tree& b) -> cgns_paths {
  STD_E_ASSERT(b.label=="CGNSBase_t");
  auto zones = get_children_by_label(b,"Zone_t");

  cgns_paths z_paths;
  for (const tree& z : zones) {
    z_paths.push_back("/"+b.name+"/"+z.name);
    if (has_child_of_name(z,"ZoneGridConnectivity")) {
      const tree& zgc = get_child_by_name(z,"ZoneGridConnectivity");
      auto gcs = get_children_by_label(zgc,"GridConnectivity_t");
      for (const tree& gc : gcs) {
        std::string opp_zone_name = to_string(value(gc));
        z_paths.push_back("/"+b.name+"/"+opp_zone_name);
      }
    }
  }

  std_e::sort_unique(z_paths);
  return z_paths;
}

//share_then_collect(
//  MPI_Comm comm, zone_reg.distribution(),
//)

auto
zones_registry(const tree& b, MPI_Comm comm) -> label_proc_registry {
  auto paths = paths_of_all_mentionned_zones(b);
  label_registry zone_reg(paths,comm);

  // share-then-collect pattern
  std::vector<int> owned_zone_ids;
  auto zones = get_children_by_label(b,"Zone_t");
  for (const tree& z : zones) {
    int z_id = get_global_id_from_path(zone_reg,"/"+b.name+"/"+z.name);
    owned_zone_ids.push_back(z_id);
  }
  int nb_zones_on_proc = owned_zone_ids.size();

  part_to_unique_block_protocol ptb_protocol(comm,zone_reg.distribution(),std::move(owned_zone_ids));

  std::vector<int> proc_of_owned_zones(nb_zones_on_proc,std_e::rank(comm));
  auto proc_of_zones_dist = exchange(ptb_protocol,proc_of_owned_zones);

  const auto& mentionned_zone_ids = zone_reg.ids();
  block_to_part_protocol btp_protocol(comm,zone_reg.distribution(),mentionned_zone_ids);

  auto proc_of_all_zones = exchange(btp_protocol,proc_of_zones_dist);

  //auto proc_of_mentionned_zones = share_then_collect(
  //  comm, zone_reg.distribution(), 
  //  std::move(owned_zone_ids), std::move(mentionned_zone_ids),
  //  proc_of_owned_zones
  //);

  return {zone_reg,proc_of_all_zones};
}

auto
zones_neighborhood_graph(const tree& b, MPI_Comm comm) -> void {
}


} // cgns
