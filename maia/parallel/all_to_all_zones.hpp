#pragma once

#include "cpp_cgns/exception.hpp"
#include <cstddef>
#include <vector>
#include "std_e/utils/vector.hpp"


template<class T> auto
multi_serialize(const std::vector<T>& sends) {
  std::vector<std::byte> buf;
  std::vector<int> sizes;
  for (const T& send : sends) {
    std::vector<std::byte> curr_buf = serialize(send);
    std_e::append(buf,curr_buf);
    sizes.push_back(curr_buf.size());
  }
  return make_pair(buf,sizes);
}
template<class T> auto
multi_deserialize(const std::vector<std::byte>& buf, const std::vector<int>& sizes) {
  std::vector<T> recv;
  std::byte* pos = buf.data();
  for (auto size : sizes) {
    recv.push_back( deserialize(pos,size) );
    pos += size;
  }
  return recv;
}

template<class T> auto
all_to_all(const std::vector<T>& sends, MPI_comm comm) {
  
  auto [sendbuf,send_sizes] = multi_serialize(sends);

  std::vector<int> recv_sizes(std_e::nb_ranks(comm));
  int err0 = MPI_Alltoall(send_sizes.data(), 1, MPI_INT64_T,
                          recv_sizes.data(), 1, MPI_INT64_T, comm);
  if (err0!=0) throw cgns::cgns_exception("MPI error "+std::to_string(err0)+" in function \"all_to_all_zones\"");

  std::vector<int> sdispls(send_sizes);
  std::exclusive_scan(begin(send_sizes),end(send_sizes),begin(sdispls),0);
  std::vector<int> rdispls(recv_sizes);
  std::exclusive_scan(begin(recv_sizes),end(recv_sizes),begin(rdispls),0);

  int recv_size = std::accumulate(begin(recv_sizes),end(recv_sizes),0);
  std::vector<std::byte> recvbuf(recv_size);
  int err1 MPI_Alltoallv(sendbuf.data(), send_sizes.data(), sdispls.data(), MPI_BYTE,
                         recvbuf.data(), send_sizes.data(), displs.data(), MPI_BYTE, comm)
  if (err1!=0) throw cgns::cgns_exception("MPI error "+std::to_string(err1)+" in function \"all_to_all_zones\"");

  std::vector<T> recvs = multi_deserialize(recvbuf,recv_sizes);
  return recvs;
}
