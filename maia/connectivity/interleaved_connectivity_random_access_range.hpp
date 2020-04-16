#pragma once


#include "maia/connectivity/interleaved_connectivity_range.hpp"


template<class C, class CK, class I = typename C::value_type> auto
index_table(interleaved_connectivity_range<C,CK> cs_fwd_accessor) -> std::vector<I> {
  std::vector<I> idx_table;
  I idx = 0;
  for (const auto& c : cs_fwd_accessor) {
    idx_table.push_back(idx);
    idx += 1+c.nb_nodes();
  }
  return idx_table;
}


template<class I, class Connectivity_kind>
class interleaved_connectivity_random_access_iterator {
  public:
    using index_type = std::remove_const_t<I>;
    using kind = Connectivity_kind;

    using this_type = interleaved_connectivity_random_access_iterator<I,kind>;
    using fwd_it_type = interleaved_connectivity_iterator<I,kind>;

    using I_nc = std::remove_const_t<I>;

    interleaved_connectivity_random_access_iterator() = default;
    interleaved_connectivity_random_access_iterator(I* first, const std::vector<I_nc>& idx_table, I pos)
      : first(first)
      , pos(pos)
      , idx_table(idx_table)
    {}

    auto size() const -> I {
      return fwd_it->size();
    }

    template<class Integer>
    auto operator+=(Integer i) -> this_type& {
      pos += i;
      return *this;
    }
    template<class Integer>
    auto operator-=(Integer i) -> this_type& {
      return *this += (-i);
    }
    auto operator++() -> this_type& { return (*this) += 1; }
    auto operator--() -> this_type& { return (*this) -= 1; }

    template<class Integer> friend auto
    operator+(const this_type& it, Integer i) -> this_type {
      this_type it0(it);
      it0 += i;
      return it0;
    }
    template<class Integer> friend auto
    operator-(const this_type& it, Integer i) -> this_type {
      this_type it0(it);
      it0 -= i;
      return it0;
    }

    auto operator*() const {
      return *fwd_it();
    }

    friend inline auto
    operator==(const this_type& it0, const this_type& it1) -> bool {
      return it0.data()==it1.data();
    }
    friend inline auto
    operator!=(const this_type& it0, const this_type& it1) -> bool {
      return !(it0 == it1);
    }
    auto data() const -> I* {
      return first+idx_table[pos];
    }
  private:
  // member functions
    auto fwd_it() -> fwd_it_type {
      return {data()};
    }
    auto fwd_it() const -> const fwd_it_type {
      return {data()};
    }
  // data member
    I* first;
    I_nc pos;
    const std::vector<I_nc>& idx_table;
};


// interleaved_connectivity_random_access_range allows to random access connectivities
// by constructing an index table.
// WARNING: due to the heterogeneous structure of the connectivity collection being accessed,
// random access cannot and does not allow to replace a connectivity by another:
// only individual vertices can be mutated.
template<class C, class Connectivity_kind>
// requires C is a Contiguous_range
// requires method C::data() returning ptr to first element
// requires I=C::value_type is an integer type
class interleaved_connectivity_random_access_range {
  public:
    using I = std_e::add_other_type_constness<typename C::value_type,C>; // If the range is const, then make the content const
    using I_nc = std::remove_const_t<I>;

    using index_type = std::remove_const_t<I>;
    using kind = Connectivity_kind;

    using fwd_accessor_type = interleaved_connectivity_range<C,kind>;
    using value_type = typename fwd_accessor_type::value_type;
    using reference = typename fwd_accessor_type::reference;

    // TODO std::iterator traits
    using iterator = interleaved_connectivity_random_access_iterator<I,kind>;
    using const_iterator = interleaved_connectivity_random_access_iterator<const I,kind>;

    interleaved_connectivity_random_access_range() = default;
    interleaved_connectivity_random_access_range(C& cs)
      : fwd_accessor(cs)
      , idx_table(index_table(fwd_accessor))
    {}

    auto memory_length() const -> I {
      return fwd_accessor.memory_length();
    }
    auto size() const -> std::ptrdiff_t {
      return idx_table.size();
    }

    auto begin()       ->       iterator { return {data()                  ,idx_table        ,0}; }
    auto begin() const -> const_iterator { return {data()                  ,idx_table.begin(),0}; }
    auto end()         ->       iterator { return {data() + memory_length(),idx_table        ,0}; }
    auto end()   const -> const_iterator { return {data() + memory_length(),idx_table.end()  ,0}; }

    template<class Integer>
    auto operator[](Integer i) -> reference {
      auto pos = data() + idx_table[i];
      auto& con_type_ref = *pos;
      auto con_start = pos+1;
      return {con_type_ref,con_start};
    }
    template<class Integer>
    auto operator[](Integer i) const -> const reference {
      auto pos = data() + idx_table[i];
      auto& con_type_ref = *pos;
      auto con_start = pos+1;
      return {con_type_ref,con_start};
    }

    auto push_back(const reference c) -> void {
      fwd_accessor.push_back(c);
      idx_table.push_back(idx_table.back() + c.memory_length);
    }

    auto data()       ->       I* { return fwd_accessor.data(); }
    auto data() const -> const I* { return fwd_accessor.data(); }
  private:
    fwd_accessor_type fwd_accessor;
    std::vector<I_nc> idx_table;
};

template<class CK, class C> constexpr auto
make_interleaved_connectivity_random_access_range(C&& c) {
  return interleaved_connectivity_random_access_range<std::remove_reference_t<C>,CK>(c);
}
