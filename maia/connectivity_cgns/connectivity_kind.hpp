#pragma once


#include "cpp_cgns/cgnslib.h"
#include "cpp_cgns/sids/elements_utils.hpp"
#include "cpp_cgns/sids/connectivity_category.hpp"
#include "maia/connectivity/concepts.hpp"
#include "maia/connectivity_cgns/poly_elt_t_reference.hpp"


namespace cgns {


template<int elt_type>
struct connectivity_kind {
  static constexpr int elt_t = elt_type;
  static constexpr int nb_nodes = cgns::number_of_nodes2(elt_type);
};

struct ngon_kind_base {
  static constexpr int elt_t = NGON_n;
  static constexpr int nb_nodes(int n) { return n; }
};
struct interleaved_ngon_kind : public ngon_kind_base {
  template<class I> using elt_t_reference = I&;
};
struct heterogenous_ngon_kind : public ngon_kind_base {
  template<class I> using elt_t_reference = poly_elt_t_reference<I>;
};

struct nface_kind_base {
  static constexpr int elt_t = NFACE_n;
  static constexpr int nb_nodes(int n) { return n; }
};
struct interleaved_nface_kind : public nface_kind_base {
  template<class I> using elt_t_reference = I&;
};
struct heterogenous_nface_kind : public nface_kind_base {
  template<class I> using elt_t_reference = poly_elt_t_reference<I>;
};

struct mixed_kind {
  static constexpr int type = MIXED;
  static constexpr int nb_nodes(int n) { return cgns::number_of_nodes2(n); }
  template<class I> using elt_t_reference = I&;
};

template<connectivity_category cat> struct connectivity_kind_of__impl;
template<> struct connectivity_kind_of__impl<            ngon > { using type =  heterogenous_ngon_kind; };
template<> struct connectivity_kind_of__impl<interleaved_ngon > { using type =   interleaved_ngon_kind; };
template<> struct connectivity_kind_of__impl<            nface> { using type = heterogenous_nface_kind; };
template<> struct connectivity_kind_of__impl<interleaved_nface> { using type =  interleaved_nface_kind; };
template<> struct connectivity_kind_of__impl<            mixed> { using type =              mixed_kind; };
template<> struct connectivity_kind_of__impl<interleaved_mixed> { using type =              mixed_kind; };
template<connectivity_category cat> using connectivity_kind_of = typename connectivity_kind_of__impl<cat>::type;


} // cgns
