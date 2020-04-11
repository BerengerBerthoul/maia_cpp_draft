#pragma once


#include "cpp_cgns/cgnslib.h"
#include "cpp_cgns/sids/elements_utils.hpp"
#include "cpp_cgns/sids/connectivity_category.hpp"


namespace cgns {


template<int elt_type>
struct connectivity_kind {
  static constexpr int type = elt_type;
  static constexpr int nb_nodes = cgns::number_of_nodes2(elt_type);
};

struct ngon_kind {
  static constexpr int type = NGON_n;
  static constexpr int nb_nodes(int n) { return n; }
};
struct nface_kind {
  static constexpr int type = NFACE_n;
  static constexpr int nb_nodes(int n) { return n; }
};
struct mixed_kind {
  static constexpr int type = MIXED;
  static constexpr int nb_nodes(int n) { return cgns::number_of_nodes2(n); }
};


template<connectivity_category cat> struct connectivity_kind_of__impl;
template<> struct connectivity_kind_of__impl<            ngon > { using type = ngon_kind ; };
template<> struct connectivity_kind_of__impl<interleaved_ngon > { using type = ngon_kind ; };
template<> struct connectivity_kind_of__impl<            nface> { using type = nface_kind; };
template<> struct connectivity_kind_of__impl<interleaved_nface> { using type = nface_kind; };
template<> struct connectivity_kind_of__impl<            mixed> { using type = mixed_kind; };
template<> struct connectivity_kind_of__impl<interleaved_mixed> { using type = mixed_kind; };
template<connectivity_category cat> using connectivity_kind_of = typename connectivity_kind_of__impl<cat>::type;


} // cgns
