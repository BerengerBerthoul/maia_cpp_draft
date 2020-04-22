#pragma once


#include <algorithm>
#include "maia/connectivity/connectivity_ref.hpp"

template<class I, class Connectivity_kind>
class heterogenous_connectivity_ref {
  public:
  // traits
    using kind = Connectivity_kind;
    using index_type = std::remove_const_t<I>;
    using elt_t_reference = typename kind::template elt_t_reference<I>;

  // ctors
    heterogenous_connectivity_ref(elt_t_reference elt_t_ref, I* nodes_ptr)
      : elt_t_ref(elt_t_ref)
      , nodes_ptr(nodes_ptr)
    {}

  // reference semantics
    heterogenous_connectivity_ref() = delete;
    heterogenous_connectivity_ref(const heterogenous_connectivity_ref&  other) = default;
    heterogenous_connectivity_ref(      heterogenous_connectivity_ref&& other) = default;

    heterogenous_connectivity_ref& operator=(const heterogenous_connectivity_ref& other) {
      std::copy(other.nodes_ptr, other.nodes_ptr+other.size(), nodes_ptr);
      elt_t_ref = other.elt_t_ref;
      return *this;
    }
    heterogenous_connectivity_ref& operator=(heterogenous_connectivity_ref&& other) {
      // even if the reference is temporary, we only care about the underlying values
      std::copy(other.nodes_ptr, other.nodes_ptr+other.size(), nodes_ptr);
      elt_t_ref = other.elt_t_ref;
      return *this;
    }
    // operator= overloads for const types {
    template<class,class> friend class heterogenous_connectivity_ref;
    template<class I0> auto
    // requires I0 is I or const I
    operator=(const heterogenous_connectivity_ref<I0,kind>& other) -> decltype(auto) {
      std::copy(other.nodes_ptr, other.nodes_ptr+other.size(), nodes_ptr);
      elt_t_ref = other.elt_t_ref;
      return *this;
    }
    template<class I0> auto
    // requires I0 is I or const I
    operator=(heterogenous_connectivity_ref<I0,kind>&& other) -> decltype(auto) {
      // even if the reference is temporary, we only care about the underlying values
      std::copy(other.nodes_ptr, other.nodes_ptr+other.size(), nodes_ptr);
      elt_t_ref = other.elt_t_ref;
      return *this;
    }
    // }

  // heterogenous accessors
    constexpr auto
    elt_t() const -> I {
      return elt_t_ref;
    }
    constexpr auto
    nb_nodes() const -> I {
      return kind::nb_nodes(elt_t());
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
    elt_t_reference elt_t_ref;
    I* nodes_ptr;
};
template<class I0, class I1, class CK> inline auto
operator==(const heterogenous_connectivity_ref<I0,CK>& x, const heterogenous_connectivity_ref<I1,CK>& y) {
  if (x.elt_t() != y.elt_t()) return false;
  else return std::equal( x.begin() , x.end() , y.begin() );
}
template<class I0, class I1, class CK> inline auto
operator!=(const heterogenous_connectivity_ref<I0,CK>& x, const heterogenous_connectivity_ref<I1,CK>& y) {
  return !(x == y);
}

template<class I0, class I1, class CK> inline auto
operator< (const heterogenous_connectivity_ref<I0,CK>& x, const heterogenous_connectivity_ref<I1,CK>& y) {
  return
       x.elt_t()< y.elt_t()
   || (x.elt_t()==y.elt_t() && std::lexicographical_compare(x.begin(),x.end(),y.begin(),y.end()));
}
template<class I, class CK> constexpr auto begin(const heterogenous_connectivity_ref<I,CK>& x) { return x.begin(); }
template<class I, class CK> constexpr auto begin(      heterogenous_connectivity_ref<I,CK>& x) { return x.begin(); }
template<class I, class CK> constexpr auto end  (const heterogenous_connectivity_ref<I,CK>& x) { return x.end(); }
template<class I, class CK> constexpr auto end  (      heterogenous_connectivity_ref<I,CK>& x) { return x.end(); }

template<class I, class CK> inline auto
to_string(const heterogenous_connectivity_ref<I,CK>& x) {
  return std_e::range_to_string(x);
}

