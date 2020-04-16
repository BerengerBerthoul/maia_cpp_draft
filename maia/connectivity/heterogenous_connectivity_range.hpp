#pragma once


#include <type_traits>
#include <algorithm>
#include "std_e/utils/meta.hpp"
#include "std_e/base/not_implemented_exception.hpp"
#include "maia/connectivity/heterogenous_connectivity_ref.hpp"




template<class I, class Connectivity_kind>
class heterogenous_connectivity_iterator { // TODO factor with std_e/iterator/index_iterator
  public:
  // type traits
    using index_type = std::remove_const_t<I>;
    using kind = Connectivity_kind;

    using connec_view_type = heterogenous_connectivity_ref<I,kind>; // TODO heterogenous_connectivity_view
    using connec_ref_type = heterogenous_connectivity_ref<I,kind>;
       
    /// std::iterator type traits
    using value_type = connec_view_type;
    using reference = connec_ref_type;
    using difference_type = I;
    using iterator_category = std::forward_iterator_tag;

  // ctor
    heterogenous_connectivity_iterator() = default;

    heterogenous_connectivity_iterator(I* offset_ptr, I* con_ptr)
      : offset_ptr(offset_ptr)
      , con_ptr(con_ptr)
    {}

  // iterator interface
    constexpr auto
    operator++() -> heterogenous_connectivity_iterator& {
      ++offset_ptr;
      return *this;
    }
    constexpr auto
    operator++(int) -> heterogenous_connectivity_iterator {
      throw std_e::not_implemented_exception("don't use postfix operator++");
    }

    auto operator*() const -> reference { return {offset_ptr,con_ptr+offset_ptr}; }
    
  private:
    I* offset_ptr;
    I* con_ptr;
};

template<class C0, class C1, class CK> constexpr auto
operator==(const heterogenous_connectivity_iterator<C0,CK>& x, const heterogenous_connectivity_iterator<C1,CK>& y) -> bool {
  return x.data() == y.data();
}
template<class C0, class C1, class CK> constexpr auto
operator!=(const heterogenous_connectivity_iterator<C0,CK>& x, const heterogenous_connectivity_iterator<C1,CK>& y) -> bool {
  return !(x == y);
}


template<class C, class Connectivity_kind>
// requires C is a Contiguous_range
// requires method C::data() returning ptr to first element
// requires I=C::value_type is an integer type
class heterogenous_connectivity_range {
  public:
  // type traits
    using I = std_e::add_other_type_constness<typename C::value_type,C>; // If the range is const, then make the content const
    using index_type = std::remove_const_t<I>;
    using kind = Connectivity_kind;

    using iterator = heterogenous_connectivity_iterator<I,kind>;
    using const_iterator = heterogenous_connectivity_iterator<const I,kind>;
    using reference = heterogenous_connectivity_ref<I,kind>;

  // ctors
    heterogenous_connectivity_range() = default;

    heterogenous_connectivity_range(C& cs)
      : cs_ptr(&cs)
    {}

  // accessors
    auto memory_length() const -> I {
      return cs_ptr->size();
    }

    auto begin()       ->       iterator { return {data()}; }
    auto begin() const -> const_iterator { return {data()}; }
    auto end()         ->       iterator { return {data()+memory_length()}; }
    auto end()   const -> const_iterator { return {data()+memory_length()}; }

    auto push_back(const reference c) -> void {
      // requires C is a Container
      auto old_size = memory_length();
      cs_ptr->resize( old_size + c.memory_length() );

      auto c_position_in_cs = cs_ptr->begin() + old_size;
      *c_position_in_cs = c.size();
      std::copy(c.begin(),c.end(),c_position_in_cs+1);
    }

    auto data() -> I* {
      return cs_ptr->data();
    }
    auto data() const -> const I* {
      return cs_ptr->data();
    }
  private:
    C* cs_ptr;
};

template<class CK, class C> constexpr auto
make_heterogenous_connectivity_range(C&& c) {
  return heterogenous_connectivity_range<std::remove_reference_t<C>,CK>(c);
}


template<class I, class Connectivity_kind>
class heterogenous_connectivity_vertex_iterator {
  public:
    using index_type = std::remove_const_t<I>;
    using kind = Connectivity_kind;
    using heterogenous_connectivity_iterator_type = heterogenous_connectivity_iterator<I,kind>;
    // TODO std::iterator type traits

    heterogenous_connectivity_vertex_iterator() = default;
    heterogenous_connectivity_vertex_iterator(heterogenous_connectivity_iterator_type it)
      : it(it)
      , pos(0)
    {}

    auto operator++() -> heterogenous_connectivity_vertex_iterator& {
      ++pos;
      if (pos == it.size()) {
        ++it;
        pos=0;
      }
      return *this;
    }

    auto operator*() const -> I& {
      return (*it)[pos];
    }

    friend inline auto
    operator==(const heterogenous_connectivity_vertex_iterator& x, const heterogenous_connectivity_vertex_iterator& y) -> bool {
      return x.it==y.it && x.pos==y.pos;
    }
    friend inline auto
    operator!=(const heterogenous_connectivity_vertex_iterator& x, const heterogenous_connectivity_vertex_iterator& y) -> bool {
      return !(x == y);
    }
  private:
    heterogenous_connectivity_iterator_type it;
    int pos;
};

template<class C, class Connectivity_kind>
class heterogenous_connectivity_vertex_range {
  public:
  // type traits
    using I = std_e::add_other_type_constness<typename C::value_type,C>; // If the range is const, then make the content const
    using index_type = std::remove_const_t<I>;
    using kind = Connectivity_kind;

    using iterator = heterogenous_connectivity_vertex_iterator<I,kind>;
    using const_iterator = heterogenous_connectivity_vertex_iterator<const I,kind>;

  // ctor
    heterogenous_connectivity_vertex_range() = default;

    heterogenous_connectivity_vertex_range(C& cs)
      : cs_ptr(&cs)
    {}

  // accessors
    auto memory_length() const -> I {
      return cs_ptr->size();
    }
    auto begin()       ->       iterator { return {data()}; }
    auto begin() const -> const_iterator { return {data()}; }
    auto end()         ->       iterator { return {data()+memory_length()}; }
    auto end()   const -> const_iterator { return {data()+memory_length()}; }

    auto data()       ->       I* { return cs_ptr->data(); }
    auto data() const -> const I* { return cs_ptr->data(); }
  private:
    C* cs_ptr;
};

template<class CK, class C> constexpr auto
make_heterogenous_connectivity_vertex_range(C&& c) {
  return heterogenous_connectivity_vertex_range<std::remove_reference_t<C>,CK>(c);
}
