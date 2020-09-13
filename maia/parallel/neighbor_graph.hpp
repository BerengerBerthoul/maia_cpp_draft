#pragma once


#include "cpp_cgns/cgns.hpp"
#include "mpi.h"
#include "tmp_cgns/cgns/cgns_registery.hpp"
#include "std_e/data_structure/jagged_range.hpp"
//#include "std_e/data_structure/multi_vector.hpp"


namespace cgns {


struct neighbor_zones {
  std::vector<std::string> names;
  std::vector<int> ids;
  std::vector<int> procs;
};
inline auto find_id_from_name(const neighbor_zones& nzs, const std::string& z_name) -> int {
  auto it = find_if(begin(nzs.names),end(nzs.names),[&](const auto& n){ return n == z_name; });
  STD_E_ASSERT(it!=end(nzs.names));
  auto idx = it-begin(nzs.names);
  return nzs.ids[idx];
}
inline auto find_name_from_id(const neighbor_zones& nzs, int z_id) -> const std::string& {
  auto it = find_if(begin(nzs.ids),end(nzs.ids),[=](const auto& id){ return id == z_id; });
  STD_E_ASSERT(it!=end(nzs.ids));
  auto idx = it-begin(nzs.ids);
  return nzs.names[idx];
}

struct connectivity_info {
  std::string zone_name;
  std::string zone_donor_name;
  tree* node;
};


auto
paths_of_all_mentionned_zones(const tree& b) -> cgns_paths;

auto
compute_neighbor_zones(const tree& b, MPI_Comm comm) -> neighbor_zones;
auto
create_connectivity_infos(tree& b) -> std::vector<connectivity_info>;

auto
zones_neighborhood_graph(const tree& b, MPI_Comm comm) -> void;


} // cgns
