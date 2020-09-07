#pragma once


#include "cpp_cgns/cgns.hpp"
#include "mpi.h"
#include "tmp_cgns/cgns/cgns_registery.hpp"
#include "std_e/data_structure/jagged_range.hpp"


namespace cgns {

//struct zone_parallel_info {};
struct zone_procs { // TODO use std_e::table
  //std::vector<std::string> names;
  std::vector<int> ids;
  std::vector<int> procs;
  //std::vector<tree*> nodes;
};
struct connectivities_with_zone_ids {
  //std::vector<std::string> zone_names;
  std::vector<int> zone_ids;
  //std::vector<std::string> zone_donor_names;
  std::vector<int> zone_donor_ids;
  std::vector<tree*> nodes;
};

struct zone_registry {
  std::vector<std::string> names;
  std::vector<int> ids;
  std_e::jagged_vector<std::string> neighbor_names;
  std_e::jagged_vector<int> neighbor_ids;
  std_e::jagged_vector<int> neighbor_ranks;
};


auto
paths_of_all_mentionned_zones(const tree& b) -> cgns_paths;

auto
compute_zone_registry(const tree& b, MPI_Comm comm) -> zone_registry;

auto
zones_neighborhood_graph(const tree& b, MPI_Comm comm) -> void;


} // cgns
