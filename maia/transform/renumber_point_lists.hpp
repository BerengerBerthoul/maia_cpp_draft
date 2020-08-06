#pragma once


#include "cpp_cgns/cgns.hpp"
#include "std_e/algorithm/id_permutations.hpp"


namespace cgns {


auto renumber_point_lists(tree& z, const std_e::offset_permutation<I4>& permutation, const std::string& grid_location) -> void;


} // cgns
