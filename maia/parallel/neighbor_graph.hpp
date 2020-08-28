#pragma once


#include "cpp_cgns/cgns.hpp"
#include "mpi.h"
#include "maia/parallel/label_proc_registry.hpp"


namespace cgns {


auto
paths_of_all_mentionned_zones(const tree& b) -> cgns_paths;

auto
zones_registry(const tree& b, MPI_Comm comm) -> label_proc_registry;

auto
zones_neighborhood_graph(const tree& b, MPI_Comm comm) -> void;


} // cgns
