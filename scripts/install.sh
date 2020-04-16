module purge
module load cmake/3.12.3
module load gcc/9.2
module load python/3.6.1

export CC=gcc
export CXX=g++

cd ..
export PROJECT_ROOT=$(pwd)/../..
#cmake -S . -Bbuild/ \
#  -DCMAKE_INSTALL_PREFIX:PATH=${PREFIX_PATH} \
#  -DCMAKE_BUILD_TYPE=Release \
#  -DCMAKE_CXX_FLAGS="-Wall -std=c++17 -fmax-errors=4" \
#  -DCMAKE_CXX_FLAGS_RELEASE="-O3 -march=native -DNDEBUG -fopenmp -fPIC" && \

echo -e "\033[31m" && date && echo -e "\e[0m" && \
cmake --build build/ -j 24 && \
echo -e "\033[31m" && date && echo -e "\e[0m" && \

## launch unit tests ##
(cd build && \
echo
echo "std_e_unit_tests"
./external/std_e/std_e_unit_tests && \
echo
echo "cpp_cgns_unit_tests"
./external/cpp_cgns/cpp_cgns_unit_tests && \
echo
echo "maia_unit_tests"
./maia_unit_tests)
