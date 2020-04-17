#pragma once

#include "maia/connectivity_cgns/connectivity_kind.hpp"

#include "maia/connectivity/connectivity_range.hpp"
#include "maia/connectivity/interleaved_connectivity_range.hpp"
#include "maia/connectivity/interleaved_connectivity_random_access_range.hpp"
#include "std_e/base/not_implemented_exception.hpp"
#include "cpp_cgns/sids/connectivity_category.hpp"


namespace cgns {


// interleaved (fwd and random access) {
template<class C> inline auto
interleaved_ngon_range(C& c) {
  return interleaved_connectivity_range<C,maia::interleaved_polygon_kind>(c);
}
template<class C> inline auto
interleaved_ngon_random_access_range(C& c) {
  return interleaved_connectivity_random_access_range<C,maia::interleaved_polygon_kind>(c);
}

template<class C> inline auto
interleaved_nface_range(C& c) {
  return interleaved_connectivity_range<C,maia::interleaved_polyhedron_kind>(c);
}
template<class C> inline auto
interleaved_nface_random_access_range(C& c) {
  return interleaved_connectivity_random_access_range<C,maia::interleaved_polyhedron_kind>(c);
}

template<class C> inline auto
interleaved_mixed_range(C& c) {
  return interleaved_connectivity_range<C,mixed_kind>(c);
}
template<class C> inline auto
interleaved_mixed_random_access_range(C& c) {
  return interleaved_connectivity_random_access_range<C,mixed_kind>(c);
}
// interleaved }


// universal fwd {
template<ElementType_t elt_type, class C> inline auto
fwd_connectivity_range(C& c) {
       if constexpr (elt_type== NGON_n) { return  interleaved_ngon_range(c); }
  else if constexpr (elt_type==NFACE_n) { return interleaved_nface_range(c); }
  else if constexpr (elt_type== MIXED ) { return interleaved_mixed_range(c); }
  else { return connectivity_range<C,connectivity_kind<elt_type>>(c); }
}
// universal fwd }



//template<class I, connectivity_category cat> inline auto
//connectivity_random_access_range(tree& elt_pool) {
//  auto elt_type = ElementType(elt_pool);
//  auto c = ElementConnectivity2<I>(elt_pool);
//  if constexpr (cat==connectivity_category::homogenous) {
//    return homogenous_connectivity_range2<cat>(elt_type,c);
//  } else if constexpr (is_interleaved(cat)) {
//    return interleaved_connectivity_random_access_range<cat>(c);
//  } else {
//    throw not_implemented_exception("heterogenous_connectivity_range not implemented yet");
//    //auto elt_start_offset = ElementStartOffset<I>(elt_pool);
//    //return heterogenous_connectivity_range<cat>(c,elt_start_offset);
//  }
//}
//template<class I, connectivity_category cat> inline auto
//connectivity_fwd_range(tree& elt_pool) {
//  if constexpr (is_interleaved(cat)) {
//    return interleaved_connectivity_range<cat>(c);
//  } else {
//    return connectivity_random_access_range<I,cat>(elt_pool); // random-access property is innate.
//  }
//}

template<class I, connectivity_category cat> inline auto
connectivity_vertex_range(tree& elt_pool) {
  auto c = ElementConnectivity<I>(elt_pool);
  if constexpr (is_interleaved(cat)) {
    using connec_kind = connectivity_kind_of<cat>;
    return make_interleaved_connectivity_vertex_range<connec_kind>(c);
  } else {
    return c; // a non-interleaved connectivity range is composed of vertices only
  }
}


} // cgns
