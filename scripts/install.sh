module purge
module load cmake/3.12.3
module load gcc/9.2
module load python/3.6.1

export CC=gcc
export CXX=g++

cd ..
cmake -S . -Bbuild/ -DCMAKE_PREFIX_PATH=${PREFIX_PATH} -DCMAKE_INSTALL_PREFIX:PATH=${INSTALL_PATH}

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
