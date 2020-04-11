#pragma once

#include "maia/connectivity/connectivity_range.hpp"

const int my_connectivity_type_id = 42;

struct my_connectivity_kind {
  static constexpr int type = my_connectivity_type_id;
  static constexpr int nb_nodes = 3;
};
