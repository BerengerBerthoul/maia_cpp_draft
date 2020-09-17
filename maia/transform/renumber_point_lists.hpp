#pragma once


#include "cpp_cgns/cgns.hpp"
#include "std_e/algorithm/id_permutations.hpp"
#include "maia/transform/donated_point_lists.hpp"


namespace cgns {


auto renumber_point_lists(tree& z, const std_e::offset_permutation<I4>& permutation, const std::string& grid_location) -> void;
auto renumber_point_lists_donated(donated_point_lists& plds, const std_e::offset_permutation<I4>& permutation, const std::string& grid_location) -> void;


} // cgns
