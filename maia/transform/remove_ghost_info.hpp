#pragma once

#include "cpp_cgns/cgns.hpp"
#include "cpp_cgns/sids/creation.hpp"
#include "mpi.h"

namespace cgns {

auto remove_ghost_info(tree& b, factory F, MPI_Comm comm) -> void;

auto remove_ghost_info_from_zone(tree& z, factory F) -> void;

} // cgns
