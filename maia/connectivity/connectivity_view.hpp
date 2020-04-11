#pragma once


#include <algorithm>


template<class I, class Connectivity_kind>
class connectivity_view {
  public:
  // type traits
    using index_type = std::remove_const_t<I>;
    using kind = Connectivity_kind;
    static constexpr int nb_nodes = kind::nb_nodes;
    static constexpr int type = kind::type;

  // ctors
    connectivity_view() = default;

    connectivity_view(I* ptr)
      : ptr(ptr)
    {}

    connectivity_view(const connectivity_view&  other): ptr(other.ptr) {}
    connectivity_view(      connectivity_view&& other): ptr(other.ptr) {}

  // assignement {
    // TODO extract this DOC on what is a view somewhere
    // NOTE 0: 
    //   be it either copy or move,
    //   assigning the view means assigning underlying values
    //   and this must be done by std::copy (not std::move), because
    //   the viewed values are not owned
    //   (hence the view is not allowed to move them, even if it is a rvalue view)
    // NOTE 1:
    //   copy ctor and copy assignement do not have the same semantics
    //     * the ctor creates a view by creating a view location (reference semantics)
    //     * the assignment assigns new values to the viewed values (value semantics)
    //   same for move ctor and move assignement
    connectivity_view& operator=(const connectivity_view& other) {
      std::copy(other.ptr, other.ptr+other.memory_length(), ptr);
    }
    connectivity_view& operator=(connectivity_view&& other) {
      std::copy(other.ptr, other.ptr+other.memory_length(), ptr);
    }

    // operator= overloads for different const types {
    template<class I0> auto
    // requires I0 is I or const I
    operator=(const connectivity_view<I0,kind>& other) -> decltype(auto) {
      std::copy(other.ptr, other.ptr+other.memory_length(), ptr);
    }
    template<class I0> auto
    // requires I0 is I or const I
    operator=(connectivity_view<I0,kind>&& other) -> decltype(auto) {
      std::copy(other.ptr, other.ptr+other.memory_length(), ptr);
    }
    // }
  // }

  // accessors
    static constexpr auto
    size() -> int {
      return nb_nodes;
    }

    constexpr auto begin()       ->       I* { return ptr; }
    constexpr auto begin() const -> const I* { return ptr; }
    constexpr auto end()         ->       I* { return ptr + nb_nodes; }
    constexpr auto end()   const -> const I* { return ptr + nb_nodes; }

    template<class Integer> constexpr auto
    operator[](Integer i) -> I& {
      return ptr[i];
    }
    template<class Integer> constexpr auto
    operator[](Integer i) const -> const I& {
      return ptr[i];
    }
  private:
    I* ptr;
};

template<class I0, class I1, class CK> inline auto
operator==(const connectivity_view<I0,CK>& v0, const connectivity_view<I1,CK>& v1) {
  return std::equal( v0.begin() , v0.end() , v1.begin() );
}
template<class I0, class I1, class CK> inline auto
operator!=(const connectivity_view<I0,CK>& v0, const connectivity_view<I1,CK>& v1) {
  return !(v0 == v1);
}
