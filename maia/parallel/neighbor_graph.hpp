#pragma once


#include "cpp_cgns/cgns.hpp"
#include "mpi.h"
#include "tmp_cgns/cgns/cgns_registery.hpp"
#include "std_e/data_structure/jagged_range.hpp"
//#include "std_e/data_structure/multi_vector.hpp"


namespace cgns {


struct zone_procs {
  std::vector<std::string> names;
  std::vector<int> ids;
  std::vector<int> procs;
};

struct connectivity_info {
  std::string zone_name;
  std::string zone_donor_name;
  tree* node;
};


auto
paths_of_all_mentionned_zones(const tree& b) -> cgns_paths;

auto
compute_zone_procs(const tree& b, MPI_Comm comm) -> zone_procs;
auto
create_connectivity_infos(tree& b) -> std::vector<connectivity_info>;

auto
zones_neighborhood_graph(const tree& b, MPI_Comm comm) -> void;


} // cgns
