Maia
========

`Maia` provides algorithms and manipulations over CGNS meshes.

## Dependencies ##
`Maia` is a C++17 library. It depends on `std_e`, `cpp_cgns`, `range-v3` and `doctest`. The build process requires Git>=2.14 and Cmake>=3.12.

## Build and install ##
1. `git clone https://github.com/BerengerBerthoul/maia`
2. `cd maia`
3. `git submodule update --init` # needed for submodules dependencies `std_e` and `cpp_cgns`
4. Use cmake to configure, build and install. The configuration step will automatically download the two remaining dependencies (`range-v3` and `doctest`). If you are not familiar with cmake usage, an example is given in `scripts/install.sh`. By default, cmake will build the library, its dependencies, and a unit test executable.

