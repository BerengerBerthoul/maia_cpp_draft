#pragma once


#include <algorithm>


template<class I, class Connectivity_kind>
class interleaved_connectivity_view {
  public:
  // type traits
    using index_type = std::remove_const_t<I>;
    using kind = Connectivity_kind;

    friend class interleaved_connectivity_view<const I,kind>;
    friend class interleaved_connectivity_view<std::remove_const_t<I>,kind>;

  // ctors
    interleaved_connectivity_view() = default;

    interleaved_connectivity_view(I* ptr)
      : ptr(ptr)
    {}

    interleaved_connectivity_view(const interleaved_connectivity_view&  other): ptr{other.ptr} {}
    interleaved_connectivity_view(      interleaved_connectivity_view&& other): ptr{other.ptr} {}

  // assignement {
    interleaved_connectivity_view& operator=(const interleaved_connectivity_view& other) {
      std::copy(other.ptr, other.ptr+other.memory_length(), ptr);
      return *this;
    }
    interleaved_connectivity_view& operator=(interleaved_connectivity_view&& other) {
      std::copy(other.ptr, other.ptr+other.memory_length(), ptr);
      return *this;
    }

    // operator= overloads for different const types {
    template<class I0> auto 
    // requires I0 is I or const I
    operator=(const interleaved_connectivity_view& other) -> decltype(auto) {
      std::copy(other.ptr, other.ptr+other.memory_length(), ptr);
      return *this;
    }
    template<class I0> auto
    // requires I0 is I or const I
    operator=(interleaved_connectivity_view&& other) -> decltype(auto) {
      std::copy(other.ptr, other.ptr+other.memory_length(), ptr);
      return *this;
    }
    // }
  // assignement } 

  // accessors
    auto type() const -> I {
      return *ptr;
    }
    auto size() const -> I {
      I sz = kind::nb_nodes(type());
      return sz;
    }
    auto memory_length() const -> I {
      return 1+size();
    }

    auto begin()       ->       I* { return ptr+1; }
    auto begin() const -> const I* { return ptr+1; }
    auto end()         ->       I* { return ptr + memory_length(); }
    auto end()   const -> const I* { return ptr + memory_length(); }

    template<class Integer> constexpr auto
    operator[](Integer i) -> I& {
      return ptr[1+i];
    }
    template<class Integer> constexpr auto
    operator[](Integer i) const -> const I& {
      return ptr[1+i];
    }
  private:
    I* ptr;
};
