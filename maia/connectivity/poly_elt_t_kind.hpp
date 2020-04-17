#pragma once

#include "maia/connectivity/concepts.hpp"
#include "maia/connectivity/poly_elt_t_reference.hpp"


namespace maia {


struct polygon_kind_base {
  static constexpr int nb_nodes(int n) { return n; }
};
struct interleaved_polygon_kind : public polygon_kind_base {
  template<class I> using elt_t_reference = I&;
};
struct heterogenous_polygon_kind : public polygon_kind_base {
  template<class I> using elt_t_reference = poly_elt_t_reference<I>;
};


struct polyhedron_kind_base {
  static constexpr int nb_nodes(int n) { return n; }
};
struct interleaved_polyhedron_kind : public polyhedron_kind_base {
  template<class I> using elt_t_reference = I&;
};
struct heterogenous_polyhedron_kind : public polyhedron_kind_base {
  template<class I> using elt_t_reference = poly_elt_t_reference<I>;
};


} // maia
