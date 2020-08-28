#include "std_e/unit_test/doctest.hpp"
#include "std_e/unit_test/mpi/doctest.hpp"

#include "maia/sids_example/distributed_base.hpp"
#include "maia/parallel/neighbor_graph.hpp"

using namespace cgns;

MPI_TEST_CASE("paths_of_all_mentionned_zones",2) {
  cgns_allocator alloc; // allocates and owns memory
  factory F(&alloc);
  auto b = example::create_distributed_base(test_rank,F);

  auto paths = paths_of_all_mentionned_zones(b);

  std::vector<std::string> expected_paths_0 = {
    "/Base/Zone0",
    "/Base/Zone1",
    "/Base/Zone3"
  };
  std::vector<std::string> expected_paths_1 = {
    "/Base/Zone0",
    "/Base/Zone1",
    "/Base/Zone2"
  };

  MPI_CHECK(0, paths == expected_paths_0);
  MPI_CHECK(1, paths == expected_paths_1);
}

MPI_TEST_CASE("zones_neighborhood_graph",2) {
  cgns_allocator alloc; // allocates and owns memory
  factory F(&alloc);
  auto b = example::create_distributed_base(test_rank,F);

  auto zr = zones_registry(b,test_comm);
  
  MPI_CHECK(0, zr.paths() == std::vector<std::string>{"/Base/Zone0","/Base/Zone1",             "/Base/Zone3"} );
  MPI_CHECK(0, zr.procs() == std::vector<    int    >{      0      ,      1      ,                   0      } );
  MPI_CHECK(1, zr.paths() == std::vector<std::string>{"/Base/Zone0","/Base/Zone1","/Base/Zone2"             } );
  MPI_CHECK(1, zr.procs() == std::vector<    int    >{      0      ,      1      ,      1                   } );
}

//MPI_TEST_CASE("zones_neighborhood_graph",2) {
//  cgns_allocator alloc; // allocates and owns memory
//  factory F(&alloc);
//  auto b = example::create_distributed_base(test_rank,F);
//
//  zones_neighborhood_graph(b,test_comm);
//  
//}
//MPI_TEST_CASE("neighbor_zone_graph - non-symmetric",2) {
//  auto b = create_3_connected_zones(test_rank);
//
//}
