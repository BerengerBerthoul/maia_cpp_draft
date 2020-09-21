#pragma once


#include "cpp_cgns/cgns.hpp"
#include "mpi.h"
#include "tmp_cgns/cgns/cgns_registery.hpp"
#include "std_e/data_structure/jagged_range.hpp"

#include "std_e/parallel/all_to_all.hpp"
#include "std_e/parallel/dist_graph.hpp"
#include "std_e/algorithm/partition_sort.hpp"
#include "cpp_cgns/tree_manip.hpp"
#include "cpp_cgns/sids/Building_Block_Structure_Definitions.hpp"


namespace cgns {


struct zone_infos {
  std::vector<std::string> names;
  std::vector<int> ids;
  std::vector<int> procs;
};
inline auto find_id_from_name(const zone_infos& zis, const std::string& z_name) -> int {
  auto it = find_if(begin(zis.names),end(zis.names),[&](const auto& n){ return n == z_name; });
  STD_E_ASSERT(it!=end(zis.names));
  auto idx = it-begin(zis.names);
  return zis.ids[idx];
}
inline auto find_name_from_id(const zone_infos& zis, int z_id) -> const std::string& {
  auto it = find_if(begin(zis.ids),end(zis.ids),[=](const auto& id){ return id == z_id; });
  STD_E_ASSERT(it!=end(zis.ids));
  auto idx = it-begin(zis.ids);
  return zis.names[idx];
}

struct connectivity_info {
  std::string zone_name;
  std::string zone_donor_name;
  tree* node;
};
inline auto
find_donor_proc(const connectivity_info& x, const zone_infos& zis) -> int {
  auto it = std::find(begin(zis.names),end(zis.names),x.zone_donor_name);
  int idx = it-begin(zis.names);
  return zis.procs[idx];
}


auto
paths_of_all_mentionned_zones(const tree& b) -> cgns_paths;

auto
compute_zone_infos(const tree& b, MPI_Comm comm) -> zone_infos;
auto
create_connectivity_infos(tree& b) -> std::vector<connectivity_info>;

auto
donor_zones_ranks(const zone_infos& zis, const std::vector<connectivity_info>& cis) -> std::vector<int>;

// TODO replace by std_e::multi_range
struct point_list_donors_by_zone {
  std::vector<std::string> donor_names;
  std::vector<cgns::GridLocation_t> locs;
  std_e::jagged_vector<I4,2> plds;
};
// Note: for now, only PointListDonor can be exchanged (not much else to exchange anyway)
class zone_exchange {
  private:
    zone_infos zis;
    MPI_Comm neighbor_to_donor_comm;
    MPI_Comm donor_to_neighbor_comm;
    std_e::jagged_vector<connectivity_info> cis;
    std_e::knot_vector<int> proc_indices_in_donor;
  public:
    zone_exchange() = default;
    zone_exchange(tree& b, MPI_Comm comm)
    {
      zis = cgns::compute_zone_infos(b,comm);
      auto cis_ = cgns::create_connectivity_infos(b);

      std::vector<int> donor_ranks = donor_zones_ranks(zis,cis_);

      neighbor_to_donor_comm = std_e::dist_graph_create_adj(comm,donor_ranks);

      cis = std_e::sort_into_partitions(std::move(cis_),[&](const auto& ci){ return find_donor_proc(ci,zis); });

      proc_indices_in_donor = neighbor_all_to_all(cis.indices(),neighbor_to_donor_comm);

      // receiver_ranks {
      int nb_donor_procs = donor_ranks.size();
      std::vector<int> procs(nb_donor_procs,std_e::rank(neighbor_to_donor_comm));
      std::vector<int> receiver_ranks = std_e::neighbor_all_to_all(procs,neighbor_to_donor_comm);

      donor_to_neighbor_comm = std_e::dist_graph_create_adj(comm,receiver_ranks);
      // receiver_ranks }
    }

    auto
    point_list_neighbor_to_donor() -> point_list_donors_by_zone {
      std::vector<I4> pld_cat;
      std::vector<int> neighbor_data_indices;
      std::vector<int> z_ids;
      std::vector<cgns::GridLocation_t> locs_neigh;
      int cur = 0;
      for (const auto& ci : cis.flat_view()) {
        tree& gc = *ci.node;
        tree& pld = get_child_by_name(gc,"PointListDonor");
        auto pld_span = view_as_span<I4>(pld.value);
        std_e::append(pld_cat,pld_span);

        neighbor_data_indices.push_back(cur);
        cur += pld_span.size();

        const std::string& z_name = ci.zone_donor_name;
        int z_id = find_id_from_name(zis,z_name);
        z_ids.push_back(z_id);

        auto loc_str = to_string(get_child_by_name(gc,"GridLocation").value);
        auto loc = std_e::to_enum<cgns::GridLocation_t>(loc_str);
        locs_neigh.push_back(loc);
      }
      neighbor_data_indices.push_back(cur);

      std_e::jagged_vector<I4,3> pl_data_from(std::move(pld_cat),std::move(neighbor_data_indices),cis.indices());
      auto pl_data_3 = neighbor_all_to_all_v(pl_data_from,neighbor_to_donor_comm);
      auto pl_data = std_e::flatten_last_level(std::move(pl_data_3));

      auto [target_z_ids,_0] = neighbor_all_to_all_v_from_indices(z_ids,cis.indices(),neighbor_to_donor_comm);
      auto target_names = std_e::transform(target_z_ids,[this](int z_id){ return find_name_from_id(this->zis,z_id); });

      auto [target_locs,_1] = neighbor_all_to_all_v_from_indices(locs_neigh,cis.indices(),neighbor_to_donor_comm);

      return {std::move(target_names) , std::move(target_locs) , std::move(pl_data)};
    }

    auto
    point_list_donor_to_neighbor(const std_e::jagged_vector<I4,2>& pl_donor_data) -> void {
      auto pl_donor_data_by_proc = std_e::view_with_new_level(pl_donor_data,proc_indices_in_donor);
      auto pl_neighbor_data = neighbor_all_to_all_v(pl_donor_data_by_proc,donor_to_neighbor_comm);
      auto pl_neighbor_data_cat = std_e::flattened_last_level_view(pl_neighbor_data);
      auto nb_joins = pl_neighbor_data_cat.size();
      STD_E_ASSERT(nb_joins==(int)cis.flat_view().size());
      for (int i=0; i<nb_joins; ++i) {
        auto& ci = cis.flat_view()[i];
        tree& gc = *ci.node;
        tree& pld = get_child_by_name(gc,"PointListDonor");
        auto pld_span = view_as_span<I4>(pld.value);
        auto new_pld_span = pl_neighbor_data_cat[i];
        STD_E_ASSERT(new_pld_span.size()==pld_span.size());
        std::copy(begin(new_pld_span),end(new_pld_span),begin(pld_span));
      }
    }
};


} // cgns
