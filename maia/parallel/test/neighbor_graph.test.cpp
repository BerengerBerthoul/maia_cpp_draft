#include "std_e/unit_test/doctest.hpp"
#include "std_e/unit_test/mpi/doctest.hpp"

#include "maia/sids_example/distributed_base.hpp"
#include "maia/parallel/neighbor_graph.hpp"

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


MPI_TEST_CASE("point_list_neighbor_to_donor",2) {
  cgns_allocator alloc; // allocates and owns memory
  factory F(&alloc);
  auto b = example::create_distributed_base(test_rank,F);

  zone_exchange ze(b,test_comm);
  auto [target_zone_names,grid_locs,pl_donor_data] = ze.point_list_neighbor_to_donor();

  MPI_CHECK( 0 , target_zone_names == vector<string>{"Zone0","Zone0","Zone3"} );
  MPI_CHECK( 0 , grid_locs == vector<GridLocation_t>{FaceCenter,Vertex,CellCenter} );
  MPI_CHECK( 0 , pl_donor_data == jagged_vector<int,2>{{1,2,3},{111,112},{136,137}} );

  MPI_CHECK( 1 , target_zone_names == vector<string>{"Zone1","Zone1","Zone1","Zone1"} );
  MPI_CHECK( 1 , grid_locs == vector<GridLocation_t>{FaceCenter,Vertex,Vertex,CellCenter} );
  MPI_CHECK( 1 , pl_donor_data == jagged_vector<int,2>{{11,12,13,14},{15},{16,17},{101,102,103,104}} );
}

MPI_TEST_CASE("point_list_donor_to_neighbor",2) {
  cgns_allocator alloc; // allocates and owns memory
  factory F(&alloc);
  auto b = example::create_distributed_base(test_rank,F);

  zone_exchange ze(b,test_comm);

  jagged_vector<int,2> pl_donor_data;
  if (test_rank==0) {
    //                   "Zone0"  ,    "Zone0"  ,   "Zone3"
    pl_donor_data = {{100,200,300},{11100,11200},{13600,13700}};
  } 
  if (test_rank==1) {
    //                        "Zone1"     ,"Zone1",  "Zone1" ,          "Zone1"
    pl_donor_data = {{1100,1200,1300,1400},{1500},{1600,1700},{10100,10200,10300,10400}};
  }

  ze.point_list_donor_to_neighbor(pl_donor_data);

  if (test_rank==0) {
    auto pl_join0 = view_as_span<I4>(get_node_by_matching(b,"Zone0/ZoneGridConnectivity/Join0/PointListDonor").value);
    auto pl_join1 = view_as_span<I4>(get_node_by_matching(b,"Zone0/ZoneGridConnectivity/Join1/PointListDonor").value);
    auto pl_join2 = view_as_span<I4>(get_node_by_matching(b,"Zone3/ZoneGridConnectivity/Join2/PointListDonor").value);
    auto pl_join3 = view_as_span<I4>(get_node_by_matching(b,"Zone3/ZoneGridConnectivity/Join3/PointListDonor").value);
    MPI_CHECK( 0 , pl_join0 == vector{100,200,300} );
    MPI_CHECK( 0 , pl_join1 == vector{1100,1200,1300,1400} );
    MPI_CHECK( 0 , pl_join2 == vector{1500} );
    MPI_CHECK( 0 , pl_join3 == vector{1600,1700} );
  }
  if (test_rank==1) {
    auto pl_join4 = view_as_span<I4>(get_node_by_matching(b,"Zone1/ZoneGridConnectivity/Join4/PointListDonor").value);
    auto pl_join5 = view_as_span<I4>(get_node_by_matching(b,"Zone1/ZoneGridConnectivity/Join5/PointListDonor").value);
    auto pl_join6 = view_as_span<I4>(get_node_by_matching(b,"Zone2/ZoneGridConnectivity/Join6/PointListDonor").value);
    MPI_CHECK( 1 , pl_join4 == vector{10100,10200,10300,10400} );
    MPI_CHECK( 1 , pl_join5 == vector{11100,11200} );
    MPI_CHECK( 1 , pl_join6 == vector{13600,13700} );
  }
}
