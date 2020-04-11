#pragma once


#include "maia/generate/ngons/from_cells/cast_heterogenous_to_homogenous.hpp"
#include "maia/generate/ngons/from_cells/faces_heterogenous_container.hpp"
#include "std_e/utils/switch.hpp"
#include "maia/generate/ngons/from_cells/append_faces_with_parent_id.hpp"

#include "std_e/utils/time_logger.hpp"
#include "std_e/base/lift.hpp"


namespace cgns {


template<class Connectivity_type, class I> auto
generate_faces_from_connectivity_elts(const Connectivity_type& elts, I first_elt_id)
 -> faces_heterogenous_container<I>;

template<class Connectivity_type, class I> auto
append_faces_from_connectivity_elts(faces_heterogenous_container<I>& faces, const Connectivity_type& elts, I first_elt_id) -> void;

template<class I, class CK> auto
append_generated_faces(faces_heterogenous_container<I>& faces, const connectivity_view<const I,CK>& elt, I elt_id);
template<class I, class CK> auto
append_generated_faces(faces_heterogenous_container<I>& faces, const interleaved_connectivity_view<const I,CK>& het_elt, I elt_id);

template<ElementType_t ElementType, class I, class Het_elt_type> auto
append_generated_faces_of_het_elt(std::integral_constant<ElementType_t,ElementType>, faces_heterogenous_container<I>& faces, const Het_elt_type& het_elt, I first_elt_id) -> void;


// TODO DEL
template<class Connectivity_type, class I> auto
generate_faces_from_connectivity_elts(const Connectivity_type& elts, I first_elt_id)
 -> faces_heterogenous_container<I>
{
  faces_heterogenous_container<I> faces;
  append_faces_from_connectivity_elts(faces,elts,first_elt_id);
  return faces;
}


template<class Connectivity_type, class I> auto
// requires I==Connectivity_type::I
append_faces_from_connectivity_elts(faces_heterogenous_container<I>& faces, const Connectivity_type& elts, I first_elt_id) -> void
{
  std_e::time_logger _("append_faces_from_connectivity_elts");
  for (auto elt : elts) {
    append_generated_faces(faces,elt,first_elt_id);
    ++first_elt_id;
  }
}


template<class I, class CK> auto
append_generated_faces(faces_heterogenous_container<I>& faces, const connectivity_view<const I,CK>& elt, I elt_id) {
  append_faces_with_parent_id(faces,elt,elt_id);
}
    

template<class I, class CK> auto
append_generated_faces(faces_heterogenous_container<I>& faces, const interleaved_connectivity_view<const I,CK>& het_elt, I elt_id) {
  std_e::switch_<all_homogenous_basic_2D_and_3D_elements>((ElementType_t)het_elt.type())
    .apply(
      LIFT(append_generated_faces_of_het_elt),
      faces,het_elt,elt_id
    );
}

template<ElementType_t ElementType, class I, class Het_elt_type> auto
append_generated_faces_of_het_elt(std::integral_constant<ElementType_t,ElementType>, faces_heterogenous_container<I>& faces, const Het_elt_type& het_elt, I elt_id) -> void {
  auto elt = cast_as<ElementType>(het_elt);
  append_faces_with_parent_id(faces,elt,elt_id);
}


} // cgns


