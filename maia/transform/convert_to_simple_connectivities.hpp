#pragma once

#include "cpp_cgns/cgns.hpp"

namespace cgns {

// Fwd decl
class factory;

auto
convert_to_simple_connectivities(tree& b, const factory& F) -> void;

auto
sort_nface_into_simple_connectivities(tree& b, const factory& F) -> void;
auto
convert_zone_to_simple_connectivities(tree& z, const factory& F) -> void;

} // cgns
