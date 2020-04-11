#pragma once


#include <array>
#include "std_e/utils/type_traits.hpp"


template<class I, class Connectivity_kind> 
class connectivity {
  public:
  // type traits
    using index_type = std::remove_const_t<I>;
    using kind = Connectivity_kind;
    static constexpr int nb_nodes = kind::nb_nodes;
    static constexpr int type = kind::type;

  // ctor
    constexpr
    connectivity() = default;

    template<
      class... Is,
      std::enable_if_t<std_e::are_integrals<Is...>,int> =0
    > constexpr
    connectivity(Is... is)
      : impl{is...} {}

    static constexpr auto
    size() -> int {
      return nb_nodes;
    }

  // accessors
    constexpr auto begin()       ->       I* { return impl.begin(); }
    constexpr auto begin() const -> const I* { return impl.begin(); }
    constexpr auto end()         ->       I* { return impl.end(); }
    constexpr auto end()   const -> const I* { return impl.end(); }

    template<class Integer> constexpr auto
    operator[](Integer i) -> I& {
      return impl[i];
    }
    template<class Integer> constexpr auto
    operator[](Integer i) const -> const I& {
      return impl[i];
    }

    friend auto
    operator==(const connectivity& c0, const connectivity& c1) {
      return c0.impl == c1.impl;
    }
    friend auto
    operator!=(const connectivity& c0, const connectivity& c1) {
      return !(c0==c1);
    }
    friend auto
    operator< (const connectivity& c0, const connectivity& c1) {
      return c0.impl < c1.impl;
    }
  private:
    std::array<I,nb_nodes> impl;
};

template<class I, class CK> constexpr auto begin(      connectivity<I,CK>& c) ->       I* { return c.begin(); }
template<class I, class CK> constexpr auto begin(const connectivity<I,CK>& c) -> const I* { return c.begin(); }
template<class I, class CK> constexpr auto end  (      connectivity<I,CK>& c) ->       I* { return c.end();   }
template<class I, class CK> constexpr auto end  (const connectivity<I,CK>& c) -> const I* { return c.end();   }
