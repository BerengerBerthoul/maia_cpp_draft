#pragma once

#include "cpp_cgns/cgns.hpp"
#include "cpp_cgns/sids/creation.hpp"

namespace cgns {

auto remove_ghost_info(tree& b, factory F) -> void;

auto remove_ghost_info_from_zone(tree& z, factory F) -> void;

} // cgns
