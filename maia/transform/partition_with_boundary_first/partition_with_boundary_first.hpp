#pragma once


#include "cpp_cgns/cgns.hpp"
#include "std_e/future/span.hpp"
#include "std_e/interval/interval.hpp"
#include "cpp_cgns/sids/creation.hpp"


namespace cgns {


struct PointList_info;


/**
This function reorders coordinates and connectivities of unstructured zones of CGNS base "b"
such that all boundary coordinates and connectivities appear first in their respective "DataArray_t"

Preconditions:
    - "b" is a CGNS base
    - "Element_t" nodes of "MIXED" type are supposed to contain only interior connectivities
    - 3D elements are supposed to be interior elements
    - 2D elements are supposed to be boundary elements (except NGON_n elements)
    - Each "Element_t" node of "NGON_n" type:
        - either contains a node named "ParentElements", in which case it is possible to check if each ngon is on the boundary or interior
        - or does not contain such a node and its connectivities are supposed to refer to only boundary nodes

Postconditions:
    - Only the *unstructured* zones are partitionned with boundary nodes and boundary connectivities first
        (because it is impossible to partition structured zone coordinates without splitting the zone)
    - "GridCoordinates" are reordered so that boundary nodes come first. The reorder is stable
    - For each zone,
        for each "Element_t" node of "NGON_n" type with a "ParentElements" sub-node,
            the node is reordered so that boundary nodes come first
            its "ElementSizeBoundary" value is set to the number or boundary connectivities
            it has a "UserDefinedData_t" sub-node named ".#PartitionIndex" with an "Ordinal" which value is the number of integers representing the boundary connectivities
                examples:
                    if all connectivities are Quad4, then ".#PartitionIndex".Ordinal = (1+4)*"ElementSizeBoundary"
                    if all connectivities are Tri3, then ".#PartitionIndex".Ordinal = (1+3)*"ElementSizeBoundary"
                    if connectivities are Tri3 and Quad4, its impossible to express only in terms of "ElementSizeBoundary"
                        it is the reason that this ".#PartitionIndex".Ordinal is given in the tree: its impossible to find it easily otherwise
    - "PointList" nodes refering to "NGON_n" connectivities are changed to match the new NGON connectivities indexing

Limitations:
    - All PointList and PointListDonor are considered to be with GridLocation==Face (even if the GridLocation node tells the opposite: it is not checked)
    - All PointLists are supposed to be disjoints between all GridConnectivity_t of a Zone_t
        (reason of this supposition: it enables for faster identification)
    - If a Zone A has a GridConnectivity with its PointListDonor being in Zone B, it is supposed that the inverse GridConnectivity structure is also present in Zone B, with the PointListDonor in Zone A being equal to the PointList in Zone B.
    - The following nodes are deleted because their re-indexing has not been implemented:
        - "Elements_t" nodes of "NFACE" type
        - Sub node "ParentElementsPosition" of the ngon node
     - Even if the tree is already partitionned (VertexSizeBoundary != 0), all partitionning steps are done (again).

Complexity:
    - An effort as been made to isolate all complex loop computations into well-known algorithms (find, stable_partition)
        The C++ standard library implementations of find and stable_partition are O(n).
        Note that it would also make sense to use std::partition instead of std::stable_partition
    - Algorithms are at most linear time and space in the arrays number of elements.
*/
auto
partition_with_boundary_first(tree& b, const factory& F) -> void;


auto
partition_zone_with_boundary_first(tree& zone, const factory& F) -> void;


auto
partition_coordinates(tree& zone) -> void;


auto
permute_boundary_vertices_at_beginning(tree& zone, const std::vector<I4>& boundary_vertex_ids) -> void;


auto
permute_boundary_grid_coords_at_beginning(tree& grid_coords, const std::vector<I4>& vertex_permutation) -> void;


auto
update_vertex_ids_in_connectivities(tree_range& elt_pools, const std::vector<I4>& vertex_permutation) -> void;


auto
save_partition_point(tree& zone, I4 nb_of_boundary_vertices) -> void;


auto
partition_elements(tree& zone, const factory& F) -> void;


auto
re_number_point_lists(tree& zone, const std_e::span<I4,2>& elt_range, const std::vector<I4>& elements_permutation) -> void;


auto
register_connectivities_PointList_infos(tree_range& zones) -> std::vector<PointList_info>;


auto
register_PointLists_of_GridConnectivities(tree_range& zones) -> std::vector<PointList_info>;


auto
bind_PointLists_to_their_PointListDonors(std::vector<PointList_info>& pl_infos, tree_range& zones) -> void;


auto
get_PointList_identifier(const node_value& pl) -> I4;


auto
re_number_point_lists_donors(std::vector<PointList_info>& PointList_infos) -> void;


auto
permute_boundary_vertices(node_value& coord, const std::vector<I4>& perm) -> void;


auto
re_number_point_list(node_value& point_list, const std::vector<I4>& elts_permutation, std_e::interval<I4> elt_range) -> void;


} // cgns
