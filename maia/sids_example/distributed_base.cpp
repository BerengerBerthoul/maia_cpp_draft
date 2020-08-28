#include "maia/sids_example/distributed_base.hpp"

using namespace cgns;

namespace example {

auto
create_distributed_base(int mpi_rank, factory F) -> tree {
  tree b = F.newCGNSBase("Base",3,3);

/* Case (note that GridConnectivities are *not* symmetric:
  proc 0
  /Base/Zone0
  /Base/Zone0/ZGC/j0 --> Zone0
  /Base/Zone0/ZGC/j1 --> Zone1
  /Base/Zone3
  /Base/Zone3/ZGC/j2 --> Zone1
  /Base/Zone3/ZGC/j3 --> Zone1
 
  proc 1
  /Base/Zone1
  /Base/Zone1/ZGC/j4 --> Zone1
  /Base/Zone1/ZGC/j5 --> Zone0
  /Base/Zone2
  /Base/Zone2/ZGC/j6 --> Zone1
*/

  if (mpi_rank == 0) {
  // Zone0
    tree z0 = F.newUnstructuredZone("Zone0");

    tree gc00 = F.newGridConnectivity("Join0","Zone0","FaceCenter","Abutting1to1");
    tree gc01 = F.newGridConnectivity("Join1","Zone1","FaceCenter","Abutting1to1");

    tree zone_gc0 = F.newZoneGridConnectivity();
    emplace_child(zone_gc0,std::move(gc00));
    emplace_child(zone_gc0,std::move(gc01));
    emplace_child(z0,std::move(zone_gc0));

    emplace_child(b,std::move(z0));
  // Zone3
    tree z3 = F.newUnstructuredZone("Zone3");

    tree gc31a = F.newGridConnectivity("Join2","Zone1","FaceCenter","Abutting1to1");
    tree gc31b = F.newGridConnectivity("Join3","Zone1","FaceCenter","Abutting1to1");

    tree zone_gc3 = F.newZoneGridConnectivity();
    emplace_child(zone_gc3,std::move(gc31a));
    emplace_child(zone_gc3,std::move(gc31b));
    emplace_child(z3,std::move(zone_gc3));

    emplace_child(b,std::move(z3));
  } else { STD_E_ASSERT(mpi_rank == 1); 
  // Zone1
    tree z1 = F.newUnstructuredZone("Zone1");

    tree gc11 = F.newGridConnectivity("Join4","Zone1","FaceCenter","Abutting1to1");
    tree gc10 = F.newGridConnectivity("Join5","Zone0","FaceCenter","Abutting1to1");

    tree zone_gc1 = F.newZoneGridConnectivity();
    emplace_child(zone_gc1,std::move(gc11));
    emplace_child(zone_gc1,std::move(gc10));
    emplace_child(z1,std::move(zone_gc1));


  // Zone2
    tree z2 = F.newUnstructuredZone("Zone2");

    tree gc21 = F.newGridConnectivity("Join6","Zone1","FaceCenter","Abutting1to1");

    tree zone_gc2 = F.newZoneGridConnectivity();
    emplace_child(zone_gc2,std::move(gc21));
    emplace_child(z1,std::move(zone_gc2));

    emplace_child(b,std::move(z2));
    emplace_child(b,std::move(z1));
  }
  return b;
}

} // example
