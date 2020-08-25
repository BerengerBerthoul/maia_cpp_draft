#pragma once


#include "cpp_cgns/cgns.hpp"
#include "tmp_cgns/cgns/cgns_registery.hpp"
#include "mpi.h"


namespace cgns {


auto
paths_of_all_mentionned_zones(const tree& b) -> cgns_paths;

auto
zones_neighborhood_graph(const tree& b, MPI_Comm comm) -> void;


} // cgns
