#include "maia/parallel/neighbor_graph.hpp"
#include "cpp_cgns/node_manip.hpp"
#include "cpp_cgns/tree_manip.hpp"
#include "std_e/utils/vector.hpp"
#include "std_e/parallel/mpi.hpp"


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

auto
zones_neighborhood_graph(const tree& b, MPI_Comm comm) -> void {
  auto paths = paths_of_all_mentionned_zones(b);
  std::cout << "Paths = " << std_e::to_string(paths) << "\n";
  label_registry zone_reg(paths,comm);

  int test_rank = std_e::rank(comm);
  int nb = zone_reg.nb_entities();
  if (test_rank==1) {
    std::cout << "rank: " << test_rank << " ; nb = " << nb << "\n";
    for (int i=0; i<nb; ++i) {
      std::cout << "rank: " << test_rank << " ; nb = " << zone_reg.entities()[i] << " ; id: " << std::to_string(zone_reg.ids()[i]) << "\n";
    }
  }
}


} // cgns
