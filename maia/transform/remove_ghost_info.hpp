#pragma once

#include "cpp_cgns/cgns.hpp"
#include "cpp_cgns/sids/creation.hpp"

namespace cgns {

auto
remove_ghost_info(tree& b, const factory& F) -> void;

} // cgns
