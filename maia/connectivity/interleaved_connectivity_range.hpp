#pragma once


#include <type_traits>
#include <algorithm>
#include "std_e/utils/meta.hpp"
#include "std_e/base/not_implemented_exception.hpp"
#include "maia/connectivity/heterogenous_connectivity_ref.hpp"




template<class I, class Connectivity_kind>
class interleaved_connectivity_iterator {
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
    interleaved_connectivity_iterator() = default;

    interleaved_connectivity_iterator(I* ptr)
      : ptr(ptr)
    {}

  // iterator interface
    auto nb_nodes() const -> I {
      return kind::nb_nodes(elt_t_ref());
    }
    constexpr auto
    operator++() -> interleaved_connectivity_iterator& {
      ptr += memory_length();
      return *this;
    }
    constexpr auto
    operator++(int) -> interleaved_connectivity_iterator {
      throw std_e::not_implemented_exception("don't use postfix operator++");
    }

    auto operator*() const -> reference { return {elt_t_ref(),begin_nodes()}; }
    
    auto data() const -> I* { return ptr; }
  private:
    auto elt_t_ref() const -> I& {
      return *ptr;
    }
    auto begin_nodes() const -> I* {
      return ptr+1;
    }
    auto memory_length() const -> I {
      return 1+nb_nodes();
    }
    I* ptr;
};

template<class C0, class C1, class CK> constexpr auto
operator==(const interleaved_connectivity_iterator<C0,CK>& x, const interleaved_connectivity_iterator<C1,CK>& y) -> bool {
  return x.data() == y.data();
}
template<class C0, class C1, class CK> constexpr auto
operator!=(const interleaved_connectivity_iterator<C0,CK>& x, const interleaved_connectivity_iterator<C1,CK>& y) -> bool {
  return !(x == y);
}


template<class C, class Connectivity_kind>
// requires C is a Contiguous_range
// requires method C::data() returning ptr to first element
// requires I=C::value_type is an integer type
class interleaved_connectivity_range {
  public:
  // type traits
    using I = std_e::add_other_type_constness<typename C::value_type,C>; // If the range is const, then make the content const
    using index_type = std::remove_const_t<I>;
    using kind = Connectivity_kind;

    using iterator = interleaved_connectivity_iterator<I,kind>;
    using const_iterator = interleaved_connectivity_iterator<const I,kind>;

    using value_type = typename iterator::value_type;
    using reference = typename iterator::reference;

  // ctors
    interleaved_connectivity_range() = default;

    interleaved_connectivity_range(C& cs)
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
make_interleaved_connectivity_range(C& c) {
  return interleaved_connectivity_range<std::remove_reference_t<C>,CK>(c);
}


template<class I, class Connectivity_kind>
class interleaved_connectivity_vertex_iterator {
  public:
    using index_type = std::remove_const_t<I>;
    using kind = Connectivity_kind;
    using interleaved_connectivity_iterator_type = interleaved_connectivity_iterator<I,kind>;
    // TODO std::iterator type traits

    interleaved_connectivity_vertex_iterator() = default;
    interleaved_connectivity_vertex_iterator(interleaved_connectivity_iterator_type it)
      : it(it)
      , pos(0)
    {}

    auto operator++() -> interleaved_connectivity_vertex_iterator& {
      ++pos;
      if (pos == it.nb_nodes()) {
        ++it;
        pos=0;
      }
      return *this;
    }

    auto operator*() const -> I& {
      return (*it)[pos];
    }

    friend inline auto
    operator==(const interleaved_connectivity_vertex_iterator& x, const interleaved_connectivity_vertex_iterator& y) -> bool {
      return x.it==y.it && x.pos==y.pos;
    }
    friend inline auto
    operator!=(const interleaved_connectivity_vertex_iterator& x, const interleaved_connectivity_vertex_iterator& y) -> bool {
      return !(x == y);
    }
  private:
    interleaved_connectivity_iterator_type it;
    int pos;
};

template<class C, class Connectivity_kind>
class interleaved_connectivity_vertex_range {
  public:
  // type traits
    using I = std_e::add_other_type_constness<typename C::value_type,C>; // If the range is const, then make the content const
    using index_type = std::remove_const_t<I>;
    using kind = Connectivity_kind;

    using iterator = interleaved_connectivity_vertex_iterator<I,kind>;
    using const_iterator = interleaved_connectivity_vertex_iterator<const I,kind>;

  // ctor
    interleaved_connectivity_vertex_range() = default;

    interleaved_connectivity_vertex_range(C& cs)
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
make_interleaved_connectivity_vertex_range(C& c) {
  return interleaved_connectivity_vertex_range<std::remove_reference_t<C>,CK>(c);
}
