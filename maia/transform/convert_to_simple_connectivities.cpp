#include "maia/transform/convert_to_simple_connectivities.hpp"


#include "maia/transform/partition_with_boundary_first/boundary_ngons_at_beginning.hpp" // TODO rename file
#include "cpp_cgns/sids/Hierarchical_Structures.hpp"
#include "cpp_cgns/sids/Grid_Coordinates_Elements_and_Flow_Solution.hpp"
#include "std_e/future/span.hpp"
#include "cpp_cgns/sids/creation.hpp"
#include "cpp_cgns/cgnslib.h"
#include "maia/generate/ngons/from_cells/cast_heterogenous_to_homogenous.hpp"


namespace cgns {


auto
sort_nface_into_simple_connectivities(tree& nfaces, const tree& ngons, const factory& F) -> void {
  I4 partition_prism_start = sort_nfaces_by_simple_polyhedron_type(nfaces,ngons);
  mark_simple_polyhedron_groups(nfaces,partition_prism_start,F);
}

auto
only_contains_tris_and_quads(std_e::span<const I4> polygon_types) -> bool {
  return 
      polygon_types == std::vector{3} // only tris
   || polygon_types == std::vector{4} // only quads
   || polygon_types == std::vector{3,4}; // only boundary tris
}
auto
convert_to_simple_boundary_connectivities(const tree& ngons, const factory& F) -> std::vector<tree> {
  auto ngon_range = ElementRange<I4>(ngons);
  auto ngon_connectivity = ElementConnectivity<I4>(ngons);
  auto polygon_types = view_as_span<I4>(get_child_by_name(ngons,".#PolygonTypeBoundary").value);
  auto polygon_type_starts = view_as_span<I4>(get_child_by_name(ngons,".#PolygonTypeStartBoundary").value);
 
  STD_E_ASSERT(only_contains_tris_and_quads(polygon_types));
  STD_E_ASSERT(polygon_type_starts.size()==polygon_types.size()+1);

  std::cout << "lala01\n";
  int nb_polygon_types = polygon_types.size();
  std::vector<tree> elt_pools;
  I4 elt_pool_start = ngon_range[0];
  std::cout << "lala02\n";
  for (int i=0; i<nb_polygon_types; ++i) {
  std::cout << "lala03\n";
    I4 polygon_type = polygon_types[i];
    const I4* poly_start = ngon_connectivity.data()+polygon_type_starts[i];
    const I4* poly_finish = ngon_connectivity.data()+polygon_type_starts[i+1];
  std::cout << "polygon_type_starts[i] = " << polygon_type_starts[i] << "\n";
  std::cout << "polygon_type_starts[i+1] = " << polygon_type_starts[i+1] << "\n";
  std::cout << "lala04\n";
    auto homogenous_range = std_e::make_span(poly_start,poly_finish);
  std::cout << "lala05\n";
    auto ngon_accessor = cgns::interleaved_ngon_random_access_range(homogenous_range);
  std::cout << "lala06\n";
    I4 nb_connec = ngon_accessor.size();
    I4 nb_vertices = nb_connec*polygon_type;
    auto homogenous_connectivities = make_cgns_vector<I4>(nb_vertices,F.alloc());
    I4 cgns_type = 0;
    if (polygon_type==3) { // TODO remove the if once make_connectivity_range is not templated anymore
      cgns_type = cgns::TRI_3;
      using Tri_kind = cgns::connectivity_kind<cgns::TRI_3>;
      auto tris = make_connectivity_range<Tri_kind>(homogenous_connectivities);
      std::transform(ngon_accessor.begin(),ngon_accessor.end(),tris.begin(),[](const auto& het_c){ return cgns::cast_as<cgns::TRI_3>(het_c); });
    }
    if (polygon_type==4) { // TODO remove the if once make_connectivity_range is not templated anymore
      cgns_type = cgns::QUAD_4;
      using Quad_kind = cgns::connectivity_kind<cgns::QUAD_4>;
      auto quads = make_connectivity_range<Quad_kind>(homogenous_connectivities);
      std::transform(ngon_accessor.begin(),ngon_accessor.end(),quads.begin(),[](const auto& het_c){ return cgns::cast_as<cgns::QUAD_4>(het_c); });
    }

    elt_pools.push_back(
      F.newElements(
        "Poly_"+std::to_string(polygon_type),
        cgns_type,
        std_e::make_span(homogenous_connectivities),
        elt_pool_start,elt_pool_start+nb_connec-1
      )
    );
    elt_pool_start += nb_connec;
  }
  return elt_pools;
}

//auto
//convert_to_simple_volume_connectivities(const tree& nfaces, const tree& ngons, const factory& F) -> std::vector<tree> {
//}

auto
convert_zone_to_simple_connectivities(tree& z, const factory& F) -> void {
  STD_E_ASSERT(z.label=="Zone_t");

  auto elt_pools = get_children_by_label(z,"Elements_t");
  auto ngons = *std::find_if(begin(elt_pools),end(elt_pools),[](const auto& elt_pool){ return ElementType<I4>(elt_pool)==NGON_n; });
  auto nfaces = *std::find_if(begin(elt_pools),end(elt_pools),[](const auto& elt_pool){ return ElementType<I4>(elt_pool)==NFACE_n; });

  sort_nface_into_simple_connectivities(nfaces,ngons,F);
  emplace_children(z,convert_to_simple_boundary_connectivities(ngons,F));
  //convert_to_simple_volume_connectivities(nfaces,ngons,F);
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
