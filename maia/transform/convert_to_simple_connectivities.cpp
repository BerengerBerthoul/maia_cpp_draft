#include "maia/transform/convert_to_simple_connectivities.hpp"


#include "maia/transform/partition_with_boundary_first/boundary_ngons_at_beginning.hpp" // TODO rename file
#include "cpp_cgns/sids/Hierarchical_Structures.hpp"
#include "cpp_cgns/sids/Grid_Coordinates_Elements_and_Flow_Solution.hpp"
#include "std_e/future/span.hpp"
#include "cpp_cgns/sids/creation.hpp"
#include "cpp_cgns/cgnslib.h"


namespace cgns {


auto
sort_nface_into_simple_connectivities(tree& nfaces, const tree& ngons, const factory& F) -> void {
  I4 partition_prism_start = sort_nfaces_by_simple_polyhedron_type(nfaces,ngons);
  mark_simple_polyhedron_groups(nfaces,partition_prism_start,F);
}

auto
convert_to_simple_boundary_connectivities(const tree& ngons, const factory& F) -> void {
}
auto
convert_to_simple_volume_connectivities(const tree& nfaces, const factory& F) -> void {
}

auto
convert_zone_to_simple_connectivities(tree& z, const factory& F) -> void {
  STD_E_ASSERT(z.label=="Zone_t");

  auto elt_pools = get_children_by_label(z,"Elements_t");
  auto ngons = *std::find_if(begin(elt_pools),end(elt_pools),[](const auto& elt_pool){ return ElementType<I4>(elt_pool)==NGON_n; });
  auto nfaces = *std::find_if(begin(elt_pools),end(elt_pools),[](const auto& elt_pool){ return ElementType<I4>(elt_pool)==NFACE_n; });

  sort_nface_into_simple_connectivities(nfaces,ngons,F);
  //convert_to_simple_boundary_connectivities(ngons,F);
  //convert_to_simple_volume_connectivities(nfaces,F);
}


auto
convert_to_simple_connectivities(tree& b, const factory& F) -> void {
  STD_E_ASSERT(b.label=="CGNSBase_t");
  auto zs = get_children_by_label(b,"Zone_t");

  for (auto& z : zs) {
    if (is_unstructured_zone(z)) {
      convert_zone_to_simple_connectivities(z,F);
    }
  }
}



} // cgns
