#pragma once


#include <algorithm>
#include "maia/connectivity/connectivity_ref.hpp"

template<class I, class Connectivity_kind>
class heterogenous_connectivity_ref {
  public:
  // traits
    using kind = Connectivity_kind;
    using index_type = std::remove_const_t<I>;

  // ctors

    heterogenous_connectivity_ref(I* type_ptr, I* nodes_ptr)
      : type_ptr(type_ptr)
      , nodes_ptr(nodes_ptr)
    {}

  // reference semantics
    heterogenous_connectivity_ref() = delete;
    heterogenous_connectivity_ref(const heterogenous_connectivity_ref&  other) = default;
    heterogenous_connectivity_ref(      heterogenous_connectivity_ref&& other) = default;

    heterogenous_connectivity_ref& operator=(const heterogenous_connectivity_ref& other) {
      std::copy(other.nodes_ptr, other.nodes_ptr+other.size(), nodes_ptr);
      *type_ptr = *other.type_ptr;
      return *this;
    }
    heterogenous_connectivity_ref& operator=(heterogenous_connectivity_ref&& other) {
      std::copy(other.nodes_ptr, other.nodes_ptr+other.size(), nodes_ptr);
      *type_ptr = *other.type_ptr;
      return *this;
    }

  // heterogenous accessors
    constexpr auto
    type() const -> I {
      return *type_ptr;
    }
    constexpr auto
    nb_nodes() const -> I {
      return kind::nb_nodes(type());
    }

  // range interface
    constexpr auto size() const -> int { return nb_nodes(); }

    constexpr auto begin()       ->       I* { return nodes_ptr; }
    constexpr auto begin() const -> const I* { return nodes_ptr; }
    constexpr auto end()         ->       I* { return nodes_ptr+size(); }
    constexpr auto end()   const -> const I* { return nodes_ptr+size(); }

    template<class I0> constexpr auto operator[](I0 i)       ->       I& { return nodes_ptr[i]; }
    template<class I0> constexpr auto operator[](I0 i) const -> const I& { return nodes_ptr[i]; }
  private:
    I* type_ptr;
    I* nodes_ptr;
};
template<class I0, class I1, class CK> inline auto
operator==(const heterogenous_connectivity_ref<I0,CK>& x, const heterogenous_connectivity_ref<I1,CK>& y) {
  if (x.type() != y.type()) return false;
  else return std::equal( x.begin() , x.end() , y.begin() );
}
template<class I0, class I1, class CK> inline auto
operator!=(const heterogenous_connectivity_ref<I0,CK>& x, const heterogenous_connectivity_ref<I1,CK>& y) {
  return !(x == y);
}
