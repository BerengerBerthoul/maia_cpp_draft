#pragma once


#include "maia/connectivity_cgns/range.hpp"


namespace cgns {


template<ElementType_t ElementType, class I, class Connectivity_kind> auto
cast_as(const heterogenous_connectivity_ref<I,Connectivity_kind>& het_con) {
  return connectivity_ref<const I,connectivity_kind<ElementType>>(het_con.begin());
}


} // cgns
