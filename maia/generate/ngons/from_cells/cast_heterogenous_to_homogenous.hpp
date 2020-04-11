#pragma once


#include "maia/connectivity_cgns/range.hpp"


namespace cgns {


template<ElementType_t ElementType, class I, class Connectivity_kind> auto
cast_as(const interleaved_connectivity_view<I,Connectivity_kind>& interleaved_con) {
  return connectivity_view<const I,connectivity_kind<ElementType>>(interleaved_con.begin());
}


} // cgns
