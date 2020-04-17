#pragma once


#include <type_traits>
#include <algorithm>
#include "std_e/utils/meta.hpp"
#include "std_e/base/not_implemented_exception.hpp"
#include "maia/connectivity/heterogenous_connectivity_ref.hpp"
#include "maia/connectivity/poly_elt_t_kind.hpp"


namespace maia {


template<class I, class Connectivity_kind>
class indexed_poly_connectivity_iterator { // TODO factor with std_e/iterator/index_iterator
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
    using iterator_category = std::forward_iterator_tag; // TODO random

  // ctor
    indexed_poly_connectivity_iterator() = default;

    indexed_poly_connectivity_iterator(I* offsets_ptr, I* cs_ptr)
      : offsets_ptr(offsets_ptr)
      , cs_ptr(cs_ptr)
    {}

  // iterator interface
    constexpr auto
    operator++() -> indexed_poly_connectivity_iterator& {
      ++offsets_ptr;
      return *this;
    }
    constexpr auto
    operator++(int) -> indexed_poly_connectivity_iterator {
      throw std_e::not_implemented_exception("don't use postfix operator++");
    }

    auto operator*() const -> reference { return {poly_elt_t_reference{offsets_ptr},cs_ptr+(*offsets_ptr)}; }
    
    template<class C0, class C1, class CK> friend constexpr auto
    operator==(const indexed_poly_connectivity_iterator<C0,CK>& x, const indexed_poly_connectivity_iterator<C1,CK>& y) -> bool;
  private:
    I* offsets_ptr;
    I* cs_ptr;
};

template<class C0, class C1, class CK> constexpr auto
operator==(const indexed_poly_connectivity_iterator<C0,CK>& x, const indexed_poly_connectivity_iterator<C1,CK>& y) -> bool {
  return x.offsets_ptr==y.offsets_ptr;
}
template<class C0, class C1, class CK> constexpr auto
operator!=(const indexed_poly_connectivity_iterator<C0,CK>& x, const indexed_poly_connectivity_iterator<C1,CK>& y) -> bool {
  return !(x == y);
}

} // maia
template<class I, class CK>
struct std::iterator_traits<maia::indexed_poly_connectivity_iterator<I,CK>> {
  using type = maia::indexed_poly_connectivity_iterator<I,CK>;
  using value_type = typename type::value_type;
  using reference = typename type::reference;
  using difference_type = typename type::difference_type;
  using iterator_category = typename type::iterator_category;
};
namespace maia {


template<class C, class Connectivity_kind>
// requires C is a Contiguous_range
// requires method C::data() returning ptr to first element
// requires I=C::value_type is an integer type
class indexed_poly_connectivity_range {
  public:
  // type traits
    using I = std_e::add_other_type_constness<typename C::value_type,C>; // If the range is const, then make the content const
    using index_type = std::remove_const_t<I>;
    using kind = Connectivity_kind;

    using iterator = indexed_poly_connectivity_iterator<I,kind>;
    using const_iterator = indexed_poly_connectivity_iterator<const I,kind>;
    using reference = heterogenous_connectivity_ref<I,kind>;

  // ctors
    indexed_poly_connectivity_range() = default;

    indexed_poly_connectivity_range(C& offsets, C& cs)
      : offsets_ptr(&offsets)
      , cs_ptr(&cs)
    {}

  // accessors
    auto size() const -> I {
      return offsets_ptr->size();
    }

    auto begin() -> iterator {
      return {
        offsets_ptr->data(),
        cs_ptr->data()
      };
    }
    auto begin() const -> const_iterator {
      return {
        offsets_ptr->data(),
        cs_ptr->data()
      };
    }
    auto end() -> iterator {
      auto offset_size = cs_ptr->size()-1; // last position is the size of connectivities, not the offset
      return {
        offsets_ptr->data()+offsets_ptr->size(),
        cs_ptr->data()+offset_size
      };
    }
    auto end() const -> const_iterator {
      auto offset_size = cs_ptr->size()-1; // last position is the size of connectivities, not the offset
      return {
        offsets_ptr->data()+offsets_ptr->size(),
        cs_ptr->data()+offset_size
      };
    }

    //auto push_back(const reference c) -> void {
    //  // requires C is a Container
    //  auto old_size = memory_length();
    //  cs_ptr->resize( old_size + c.memory_length() );

    //  auto c_position_in_cs = cs_ptr->begin() + old_size;
    //  *c_position_in_cs = c.size();
    //  std::copy(c.begin(),c.end(),c_position_in_cs+1);
    //}

  private:
    C* offsets_ptr;
    C* cs_ptr;
};

template<class CK, class C> constexpr auto
make_indexed_poly_connectivity_range(C& offsets, C& cs) {
  return indexed_poly_connectivity_range<C,CK>(offsets,cs);
}


} // maia
