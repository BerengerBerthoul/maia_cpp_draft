#pragma once


#include "cpp_cgns/cgns.hpp"
#include "mpi.h"
#include "tmp_cgns/cgns/cgns_registery.hpp"
#include "std_e/data_structure/jagged_range.hpp"

#include "std_e/parallel/all_to_all.hpp"
#include "std_e/parallel/dist_graph.hpp"
#include "std_e/algorithm/partition_sort.hpp"
#include "cpp_cgns/tree_manip.hpp"


namespace cgns {


struct neighbor_zones {
  std::vector<std::string> names;
  std::vector<int> ids;
  std::vector<int> procs;
};
inline auto find_id_from_name(const neighbor_zones& nzs, const std::string& z_name) -> int {
  auto it = find_if(begin(nzs.names),end(nzs.names),[&](const auto& n){ return n == z_name; });
  STD_E_ASSERT(it!=end(nzs.names));
  auto idx = it-begin(nzs.names);
  return nzs.ids[idx];
}
inline auto find_name_from_id(const neighbor_zones& nzs, int z_id) -> const std::string& {
  auto it = find_if(begin(nzs.ids),end(nzs.ids),[=](const auto& id){ return id == z_id; });
  STD_E_ASSERT(it!=end(nzs.ids));
  auto idx = it-begin(nzs.ids);
  return nzs.names[idx];
}

struct connectivity_info {
  std::string zone_name;
  std::string zone_donor_name;
  tree* node;
};
auto
find_donor_proc(const connectivity_info& x, const neighbor_zones& nzs) -> int {
  auto it = std::find(begin(nzs.names),end(nzs.names),x.zone_donor_name);
  int idx = it-begin(nzs.names);
  return nzs.procs[idx];
}


auto
paths_of_all_mentionned_zones(const tree& b) -> cgns_paths;

auto
compute_neighbor_zones(const tree& b, MPI_Comm comm) -> neighbor_zones;
auto
create_connectivity_infos(tree& b) -> std::vector<connectivity_info>;

auto
zones_neighborhood_graph(const tree& b, MPI_Comm comm) -> void;


// Note: for now, only PointListDonor can be exchanged (not much else to exchange anyway)
class zone_exchange {
  private:
    neighbor_zones nzs;
    MPI_Comm neighbor_to_donor_comm;
    MPI_Comm donor_to_neighbor_comm;
    std_e::jagged_vector<connectivity_info> cis;
  public:
    zone_exchange(tree& b, MPI_Comm comm)
    {
      nzs = cgns::compute_neighbor_zones(b,comm);

      std::vector<int> ranks = nzs.procs;
      std_e::sort_unique(ranks); // TODO: also remove non-neighbor zones

      neighbor_to_donor_comm = std_e::dist_graph_create_adj(comm,ranks);

      auto cis_ = cgns::create_connectivity_infos(b);
      cis = std_e::sort_into_partitions(std::move(cis_),[&](const auto& ci){ return find_donor_proc(ci,nzs); });

      // neighbor_ranks {
      int nb_neighbor_procs = cis.size();
      std::vector<int> procs(nb_neighbor_procs,std_e::rank(neighbor_to_donor_comm));
      std::vector<int> neighbor_ranks = std_e::neighbor_all_to_all(procs,neighbor_to_donor_comm);

      donor_to_neighbor_comm = std_e::dist_graph_create_adj(comm,neighbor_ranks);
      // neighbor_ranks }
    }

    auto
    point_list_neighbor_to_donor() {
      std::vector<I4> pld_cat;
      std::vector<int> neighbor_data_indices;
      std::vector<int> z_ids;
      int cur = 0;
      for (const auto& ci : cis.flat_view()) {
        tree& gc = *ci.node;
        tree& pld = get_child_by_name(gc,"PointListDonor");
        auto pld_span = view_as_span<I4>(pld.value);
        std_e::append(pld_cat,pld_span);

        neighbor_data_indices.push_back(cur);
        cur += pld_span.size();

        const std::string& z_name = ci.zone_donor_name;
        int z_id = find_id_from_name(nzs,z_name);
        z_ids.push_back(z_id);
      }
      neighbor_data_indices.push_back(cur);

      std_e::jagged_vector<int,3> pl_data_from(std::move(pld_cat),std::move(neighbor_data_indices),cis.indices());
      auto pl_data = neighbor_all_to_all_v(pl_data_from,neighbor_to_donor_comm);

      std_e::jagged_vector<int> yx(z_ids,cis.indices());
      auto target_z_ids = neighbor_all_to_all_v(yx,neighbor_to_donor_comm);
      std_e::jagged_vector<std::string> target_names = std_e::transform(target_z_ids,[this](int z_id){ return find_name_from_id(this->nzs,z_id); });

      return std::make_pair(target_names,pl_data);
    }

    auto
    point_list_donor_to_neighbor(std_e::jagged_vector<int,3> pl_donor_data) -> void {
      auto pl_neighbor_data = neighbor_all_to_all_v(pl_donor_data,donor_to_neighbor_comm);
      auto pl_neighbor_data_cat = flattened_last_level_view(pl_neighbor_data);
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
