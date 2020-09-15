#include "std_e/unit_test/doctest.hpp"
#include "std_e/unit_test/mpi/doctest.hpp"

#include "maia/sids_example/distributed_base.hpp"
#include "maia/parallel/neighbor_graph.hpp"
#include "std_e/parallel/all_to_all.hpp" // TODO MOVE
#include "std_e/parallel/dist_graph.hpp" // TODO MOVE
#include "std_e/log.hpp" // TODO MOVE
#include "cpp_cgns/tree_manip.hpp" // TODO MOVE
#include "std_e/algorithm/mismatch_points.hpp"

using namespace cgns;
using namespace std;
using namespace std_e;

MPI_TEST_CASE("paths_of_all_mentionned_zones",2) {
  cgns_allocator alloc; // allocates and owns memory
  factory F(&alloc);
  cgns::tree b = example::create_distributed_base(test_rank,F);

  auto paths = paths_of_all_mentionned_zones(b);

  vector<string> expected_paths_0 = {
    "/Base/Zone0",
    "/Base/Zone1",
    "/Base/Zone3"
  };
  vector<string> expected_paths_1 = {
    "/Base/Zone0",
    "/Base/Zone1",
    "/Base/Zone2",
    "/Base/Zone3"
  };

  MPI_CHECK(0, paths == expected_paths_0);
  MPI_CHECK(1, paths == expected_paths_1);
}

MPI_TEST_CASE("neighbor_zones",2) {
  cgns_allocator alloc; // allocates and owns memory
  factory F(&alloc);
  cgns::tree b = example::create_distributed_base(test_rank,F);

  neighbor_zones nzs = cgns::compute_neighbor_zones(b,test_comm);

  MPI_CHECK(0, nzs.names == vector<string>{"Zone0","Zone1","Zone3"} );
  MPI_CHECK(0, nzs.procs == vector< int  >{   0   ,   1   ,   0   } );
  MPI_CHECK(1, nzs.names == vector<string>{"Zone0","Zone1","Zone2","Zone3"} );
  MPI_CHECK(1, nzs.procs == vector< int  >{   0   ,   1   ,   1   ,   0   } );
}
MPI_TEST_CASE("connectivity_infos",2) {
  cgns_allocator alloc; // allocates and owns memory
  factory F(&alloc);
  cgns::tree b = example::create_distributed_base(test_rank,F);

  std::vector<connectivity_info> cis = cgns::create_connectivity_infos(b);

  MPI_CHECK(0, cis.size() == 4 );
  MPI_CHECK(0, cis[0].zone_name == "Zone0" ); MPI_CHECK(0, cis[0].zone_donor_name == "Zone0" );
  MPI_CHECK(0, cis[1].zone_name == "Zone0" ); MPI_CHECK(0, cis[1].zone_donor_name == "Zone1" );
  MPI_CHECK(0, cis[2].zone_name == "Zone3" ); MPI_CHECK(0, cis[2].zone_donor_name == "Zone1" );
  MPI_CHECK(0, cis[3].zone_name == "Zone3" ); MPI_CHECK(0, cis[3].zone_donor_name == "Zone1" );
  MPI_CHECK(1, cis.size() == 3 );
  MPI_CHECK(1, cis[0].zone_name == "Zone1" ); MPI_CHECK(1, cis[0].zone_donor_name == "Zone1" );
  MPI_CHECK(1, cis[1].zone_name == "Zone1" ); MPI_CHECK(1, cis[1].zone_donor_name == "Zone0" );
  MPI_CHECK(1, cis[2].zone_name == "Zone2" ); MPI_CHECK(1, cis[2].zone_donor_name == "Zone3" );
}


#include "std_e/utils/functional.hpp"

class zone_exchange {
  private:
    neighbor_zones nzs;
    std::vector<int> ranks;
    MPI_Comm dist_comm;
    MPI_Comm owner_to_neighbor_comm;
    jagged_vector<connectivity_info> cis;
    std::vector<int> neighbor_ranks;
  public:
    zone_exchange(tree& b, MPI_Comm comm)
    {
      nzs = cgns::compute_neighbor_zones(b,comm);

      ranks = nzs.procs;
      std_e::sort_unique(ranks);

      dist_comm = std_e::dist_graph_create(comm,ranks);

      auto cis_ = cgns::create_connectivity_infos(b);
      cis = sort_into_partitions(std::move(cis_),[&](const auto& ci){ return find_donor_proc(ci,nzs); });

      // neighbor_ranks {
      int nb_neighbor_procs = cis.size();
      std::vector<int> procs(nb_neighbor_procs,std_e::rank(dist_comm));
      neighbor_ranks = neighbor_all_to_all(procs,dist_comm);

      owner_to_neighbor_comm = std_e::dist_graph_create(comm,neighbor_ranks);
      // neighbor_ranks }
    }

    auto
    zones() const -> const auto& {
      return nzs;
    }

    auto
    point_list_neighbor_to_owner() {
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

      jagged_vector<int,3> pl_data_from(std::move(pld_cat),std::move(neighbor_data_indices),cis.indices());
      auto pl_data = neighbor_all_to_all_v(pl_data_from,dist_comm);

      jagged_vector<int> yx(z_ids,cis.indices());
      auto target_z_ids = neighbor_all_to_all_v(yx,dist_comm);
      jagged_vector<std::string> target_names = std_e::transform(target_z_ids,[this](int z_id){ return find_name_from_id(this->nzs,z_id); });

      return std::make_tuple(neighbor_ranks,target_names,pl_data);
    }

    //owner_to_neighbor()

};
MPI_TEST_CASE("point_list_neighbor_to_owner",2) {
  cgns_allocator alloc; // allocates and owns memory
  factory F(&alloc);
  auto b = example::create_distributed_base(test_rank,F);

  zone_exchange ze(b,test_comm);
  auto [neighbor_ranks,target_zone_names,pl_owner_data] = ze.point_list_neighbor_to_owner();

  MPI_CHECK( 0 , neighbor_ranks[0] == 0 );
  MPI_CHECK( 0 , target_zone_names[0] == vector<string>{"Zone0"} );
  MPI_CHECK( 0 , pl_owner_data[0] == jagged_vector<int>{{1,2,3}} );

  MPI_CHECK( 0 , neighbor_ranks[1] == 1 );
  MPI_CHECK( 0 , target_zone_names[1] == vector<string>{"Zone0","Zone3"} );
  MPI_CHECK( 0 , pl_owner_data[1] == jagged_vector<int>{{111,112},{136,137}} );


  MPI_CHECK( 1 , neighbor_ranks[0] == 1 );
  MPI_CHECK( 1 , target_zone_names[0] == vector<string>{"Zone1"} );
  MPI_CHECK( 1 , pl_owner_data[0] == jagged_vector<int>{{101,102,103,104}} );

  MPI_CHECK( 1 , neighbor_ranks[1] == 0 );
  MPI_CHECK( 1 , target_zone_names[1] == vector<string>{"Zone1","Zone1","Zone1"} );
  MPI_CHECK( 1 , pl_owner_data[1] == jagged_vector<int>{{11,12,13,14},{15},{16,17}} );
}
//MPI_TEST_CASE("point_list_owner_to_neighbor",2) {
//  cgns_allocator alloc; // allocates and owns memory
//  factory F(&alloc);
//  auto b = example::create_distributed_base(test_rank,F);
//
//  zone_exchange ze(b,test_comm);
//
//  jagged_vector<int> neighbor_ranks;
//  jagged_vector<std::string> target_zone_names;
//  jagged_vector<int,3> pl_owner_data;
//  if (test_rank==0) {
//
//  } else if (
//
//  auto [neighbor_ranks,target_zone_names,pl_owner_data] = ze.point_list_neighbor_to_owner();
//
//  MPI_CHECK( 0 , neighbor_ranks[0] == 0 );
//  MPI_CHECK( 0 , target_zone_names[0] == vector<string>{"Zone0"} );
//  MPI_CHECK( 0 , pl_owner_data[0] == jagged_vector<int>{{1,2,3}} );
//
//  MPI_CHECK( 0 , neighbor_ranks[1] == 1 );
//  MPI_CHECK( 0 , target_zone_names[1] == vector<string>{"Zone0","Zone3"} );
//  MPI_CHECK( 0 , pl_owner_data[1] == jagged_vector<int>{{111,112},{136,137}} );
//
//
//  MPI_CHECK( 1 , neighbor_ranks[0] == 1 );
//  MPI_CHECK( 1 , target_zone_names[0] == vector<string>{"Zone1"} );
//  MPI_CHECK( 1 , pl_owner_data[0] == jagged_vector<int>{{101,102,103,104}} );
//
//  MPI_CHECK( 1 , neighbor_ranks[1] == 0 );
//  MPI_CHECK( 1 , target_zone_names[1] == vector<string>{"Zone1","Zone1","Zone1"} );
//  MPI_CHECK( 1 , pl_owner_data[1] == jagged_vector<int>{{11,12,13,14},{15},{16,17}} );
//}
