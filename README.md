Maia
========

`Maia` provides algorithms and manipulations over CGNS meshes.

## Dependencies ##
`Maia` is a C++17 library. The build process requires GCC>=8, Git>=2.14 and Cmake>=3.12. It depends on libraries `project_utils`, `std_e`, `cpp_cgns`, `range-v3` and `doctest` (see section `Build and install`).

## Build and install ##
1. `git clone https://github.com/BerengerBerthoul/maia`
2. `cd maia`
3. `git submodule update --init` (needed for submodules dependencies `project_utils`, `std_e` and `cpp_cgns`)
4. Use `cmake` to configure, build and install. The configuration step will automatically download the two remaining dependencies (`range-v3` and `doctest`). If you are not familiar with `cmake` usage, an example is given in `scripts/install.sh`. By default, `cmake` will build the library, its dependencies, and a unit test executable.

## Development ##
The `Maia` repository is compatible with the development process described in `external/project_utils/doc/main.md`. It uses git submodules to ease the joint development with other repositories compatible with this organization. TL;DR: configure the git repository with `cd external/project_utils/scripts && configure_top_level_repo.sh`.

## License ##
`Maia` is available under the MPL-2.0 license (https://mozilla.org/MPL/2.0/).
