#pragma once


#include "cpp_cgns/cgns.hpp"
#include "cpp_cgns/sids/creation.hpp"


auto
create_distributed_base(int mpi_rank, cgns::factory F) -> cgns::tree;
