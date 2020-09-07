#include "std_e/unit_test/doctest.hpp"
#include "std_e/unit_test/mpi/doctest.hpp"

#include "maia/sids_example/distributed_base.hpp"
#include "maia/parallel/neighbor_graph.hpp"
#include "std_e/parallel/all_to_all.hpp" // TODO MOVE
#include "std_e/log.hpp" // TODO MOVE

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

MPI_TEST_CASE("zone_registry",2) {
  cgns_allocator alloc; // allocates and owns memory
  factory F(&alloc);
  cgns::tree b = example::create_distributed_base(test_rank,F);

  zone_registry zr = cgns::compute_zone_registry(b,test_comm);
  
  MPI_CHECK(0, zr.names == vector<string>{"Zone0","Zone3"} );
  MPI_CHECK(0, zr.neighbor_names == std_e::jagged_vector<string>{{"Zone0","Zone1"},{"Zone1","Zone1"}} );
  MPI_CHECK(0, zr.neighbor_ranks == std_e::jagged_vector< int  >{{   0   ,   1   },{   1   ,   1   }} );
  MPI_CHECK(1, zr.names == vector<string>{"Zone1","Zone2"} );
  MPI_CHECK(1, zr.neighbor_names == std_e::jagged_vector<string>{{"Zone1","Zone0"},{"Zone3"}} );
  MPI_CHECK(1, zr.neighbor_ranks == std_e::jagged_vector< int  >{{   1   ,   0   },{   0   }} );
}




class zone_exchange {
  public:
    zones_exchange(const tree& b, MPI_Comm comm)
    {
      auto zr = compute_zone_registry(b,test_comm);

      auto ranks = std_e::sort_unique(zr.neighbor_ranks.flat_view());
      MPI_Comm dist_comm = std_e::dist_graph_create(test_comm,ranks);
    }

    auto
    names() const -> const vector<string>& {
      return zr.names;
    }
    auto
    neighbor_names() const -> const jagged_vector<string>& {
      return zr.neighbor_names;
    }

    template<class T, int N>
    neighbor_to_owner(const jagged_vector<T,N>& neighbors_data) -> jagged_vector<T> {
      static_assert(N>=2);
      STD_E_ASSERT(neighbors_data.indices()[0]==ze.neighbor_names().indices()[0]); // neighbors data structured with same neighborhood
      std::vector<data_t> data_received = std_e::neighbor_all_to_all_v(data_to_send,comm_graph);
      if constexpr (N==3) { // TODO generalize
    }

    //owner_to_neighbor()

};
MPI_TEST_CASE("zones_neighborhood_graph",2) {
  cgns_allocator alloc; // allocates and owns memory
  factory F(&alloc);
  auto b = example::create_distributed_base(test_rank,F);

  zone_exchange ze(b,test_comm);
  MPI_CHECK( 0 , ze.names() == vector<string>{"Zone0","Zone3"} );
  MPI_CHECK( 0 , ze.neighbor_names() == jagged_vector<string>{{"Zone0","Zone1"},{"Zone1","Zone1"}} );
  MPI_CHECK( 1 , ze.names() == vector<string>{"Zone1","Zone2"} );
  MPI_CHECK( 1 , ze.neighbor_names() == jagged_vector<string>{{"Zone1","Zone0"},{"Zone3"}} );

  jagged_vector<int,3> neighbors_data;
  //                           Owners         Zone0          |   Zone3
  //                         Neighbors  Zone0      Zone1     |Zone1  Zone1
  if (test_rank==0) neighbors_data = {{{1,2,3},{11,12,13,14}},{{15},{16,17}}};

  //                           Owners         Zone1                |   Zone2
  //                         Neighbors  Zone1      Zone0           |   Zone3
  if (test_rank==1) neighbors_data = {{{111,112},{101,102,103,104}},{{136,137}}};

  jagged_vector<int,3> owner_data = ze.neighbor_to_owner(neighbors_data);

  MPI_CHECK( 0 , owner_data == jagged_vector<int,3>{{{1,2,3},{101,102,103,104}},{{136,137}}} );
  MPI_CHECK( 0 , owner_data == jagged_vector<int,3>{{{11,12,13,14},{15},{16,17},{111,112}},{}} );
}
MPI_TEST_CASE("neighbor_zone_graph - non-symmetric",2) {
  auto b = create_3_connected_zones(test_rank);
}
