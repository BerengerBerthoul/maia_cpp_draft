#include "maia/parallel/neighbor_graph.hpp"
#include "cpp_cgns/node_manip.hpp"
#include "cpp_cgns/tree_manip.hpp"
#include "std_e/utils/vector.hpp"
#include "std_e/parallel/mpi.hpp"
#include "std_e/log.hpp"
#include "std_e/utils/to_string.hpp"
#include "tmp_cgns/exchange/spread_then_collect.hpp"


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

// TODO compressed_vector type instead of vector<vector>
auto
compute_zone_registry(const tree& b, MPI_Comm comm) -> zone_registry {
  auto paths = paths_of_all_mentionned_zones(b);
  label_registry zone_reg(paths,comm);

  std::vector<std::string> owned_zone_names;
  std::vector<int> owned_zone_ids;
  std_e::jagged_vector<std::string> neighbor_zone_names;
  std_e::jagged_vector<int> neighbor_zone_ids;
  auto zones = get_children_by_label(b,"Zone_t");
  // TODO factorize with above
  for (const tree& z : zones) {
    owned_zone_names.push_back(z.name);
    int z_id = get_global_id_from_path(zone_reg,"/"+b.name+"/"+z.name);
    owned_zone_ids.push_back(z_id);
    if (has_child_of_name(z,"ZoneGridConnectivity")) {
      const tree& zgc = get_child_by_name(z,"ZoneGridConnectivity");
      auto gcs = get_children_by_label(zgc,"GridConnectivity_t");
      neighbor_zone_names.push_level();
      neighbor_zone_ids.push_level();
      for (const tree& gc : gcs) {
        std::string opp_zone_name = to_string(value(gc));
        neighbor_zone_names.push_back(opp_zone_name);
        int z_id = get_global_id_from_path(zone_reg,"/"+b.name+"/"+opp_zone_name);
        neighbor_zone_ids.push_back(z_id);
      }
    }
  }

  int nb_zones_on_proc = owned_zone_ids.size();
  std::vector<int> proc_of_owned_zones(nb_zones_on_proc,std_e::rank(comm));


  auto proc_of_neighbor_zones = spread_then_collect(
    comm, zone_reg.distribution(), 
    owned_zone_ids, proc_of_owned_zones,
    neighbor_zone_ids.flat_view()
  );

  std_e::jagged_vector<int> neighbor_ranks(std::move(proc_of_neighbor_zones),neighbor_zone_names.indices());

  return {
    owned_zone_names,
    owned_zone_ids,
    neighbor_zone_names,
    neighbor_zone_ids,
    neighbor_ranks
  };
}

auto
zones_neighborhood_graph(const tree& b, MPI_Comm comm) -> void {
  auto zr = compute_zone_registry(b,comm);
}


} // cgns
