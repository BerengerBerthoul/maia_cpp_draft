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
name_of_zones(const tree& b) -> std::vector<std::string> {
  STD_E_ASSERT(b.label=="CGNSBase_t");
  auto zones = get_children_by_label(b,"Zone_t");

  std::vector<std::string> z_names;
  for (const tree& z : zones) {
    z_names.push_back(z.name);
  }

  return z_names;
}

auto
name_of_all_mentionned_zones(const tree& b) -> std::vector<std::string> {
  STD_E_ASSERT(b.label=="CGNSBase_t");
  auto zones = get_children_by_label(b,"Zone_t");

  std::vector<std::string> z_names;
  for (const tree& z : zones) {
    z_names.push_back(z.name);
    if (has_child_of_name(z,"ZoneGridConnectivity")) {
      const tree& zgc = get_child_by_name(z,"ZoneGridConnectivity");
      auto gcs = get_children_by_label(zgc,"GridConnectivity_t");
      for (const tree& gc : gcs) {
        std::string opp_zone_name = to_string(value(gc));
        z_names.push_back(opp_zone_name);
      }
    }
  }

  std_e::sort_unique(z_names);
  return z_names;
}

auto
create_connectivity_infos(tree& b) -> std::vector<connectivity_info> {
  STD_E_ASSERT(b.label=="CGNSBase_t");
  auto zones = get_children_by_label(b,"Zone_t");

  std::vector<connectivity_info> cis;
  for (tree& z : zones) {
    if (has_child_of_name(z,"ZoneGridConnectivity")) {
      tree& zgc = get_child_by_name(z,"ZoneGridConnectivity");
      auto gcs = get_children_by_label(zgc,"GridConnectivity_t");
      for (tree& gc : gcs) {
        std::string opp_zone_name = to_string(value(gc));
        cis.push_back({z.name,opp_zone_name,&gc});
      }
    }
  }

  return cis;
}

auto
paths_of_all_mentionned_zones(const tree& b) -> cgns_paths {
  cgns_paths z_paths;
  for (const auto& z_name : name_of_all_mentionned_zones(b)) {
    z_paths.push_back("/"+b.name+"/"+z_name);
  }
  return z_paths;
}

auto
compute_zone_procs(const tree& b, MPI_Comm comm) -> zone_procs {
  auto paths = paths_of_all_mentionned_zones(b);
  label_registry zone_reg(paths,comm);

  auto owned_zone_names = name_of_zones(b);
  int nb_owned_zones = owned_zone_names.size();
  std::vector<int> owned_zone_ids(nb_owned_zones);
  for (int i=0; i<nb_owned_zones; ++i) {
    owned_zone_ids[i] = get_global_id_from_path(zone_reg,"/"+b.name+"/"+owned_zone_names[i]);
  }

  auto zone_names = name_of_all_mentionned_zones(b);
  int nb_zones = zone_names.size();
  std::vector<int> zone_ids(nb_zones);
  for (int i=0; i<nb_zones; ++i) {
    zone_ids[i] = get_global_id_from_path(zone_reg,"/"+b.name+"/"+zone_names[i]);
  }

  std::vector<int> proc_of_owned_zones(nb_zones,std_e::rank(comm));

  auto proc_of_zones = spread_then_collect(
    comm, zone_reg.distribution(), 
    owned_zone_ids, proc_of_owned_zones,
    zone_ids
  );

  return {std::move(zone_names),std::move(zone_ids),std::move(proc_of_zones)};
}

auto
zones_neighborhood_graph(const tree& b, MPI_Comm comm) -> void {
  auto zr = compute_zone_procs(b,comm);
}


} // cgns
