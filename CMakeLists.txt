## Global config ##
cmake_minimum_required(VERSION 3.12)

if(DEFINED ENV{PROJECT_ROOT})
  set(PROJECT_ROOT $ENV{PROJECT_ROOT})
else()
  set(PROJECT_ROOT ${CMAKE_SOURCE_DIR})
endif()
include(${PROJECT_ROOT}/external/project_utils/scripts/cmake/target_add_dependency.cmake)

set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")

## Project ##
project(
  maia VERSION 0.1.0
  DESCRIPTION "Mesh algorithms and manipulations"
  LANGUAGES C CXX
)


## Dependencies ##
include(FetchContent)
### doctest ###
FetchContent_Declare(
  doctest
  GIT_REPOSITORY https://github.com/onqtam/doctest.git
  GIT_TAG        2.3.7
)
FetchContent_GetProperties(doctest)
if(NOT doctest_POPULATED)
  FetchContent_Populate(doctest)
  add_subdirectory(${doctest_SOURCE_DIR} ${doctest_BINARY_DIR})
endif()
### range-v3 ###
FetchContent_Declare(
  range-v3
  GIT_REPOSITORY https://github.com/ericniebler/range-v3
  GIT_TAG        0.10.0
)
FetchContent_GetProperties(range-v3)
if(NOT range-v3_POPULATED)
  FetchContent_Populate(range-v3)
  add_subdirectory(${range-v3_SOURCE_DIR} ${range-v3_BINARY_DIR})
endif()


## Find files ##
set(src_dir ${PROJECT_SOURCE_DIR}/${PROJECT_NAME})
set(include_dir ${PROJECT_SOURCE_DIR})

file(GLOB_RECURSE cpp_and_test_files
  CONFIGURE_DEPENDS
  "${src_dir}/*.cpp"
)
file(GLOB_RECURSE python_files
  CONFIGURE_DEPENDS
  "${src_dir}/*.py"
)
set(cpp_files ${cpp_and_test_files})
list(FILTER cpp_files EXCLUDE REGEX ".*\.test\.cpp$")
list(FILTER cpp_files EXCLUDE REGEX ".*\.cpython\.cpp$")
set(test_files ${cpp_and_test_files})
list(FILTER test_files INCLUDE REGEX ".*\.test\.cpp$")
set(cpython_files ${cpp_and_test_files})
list(FILTER cpython_files INCLUDE REGEX ".*\.cpython\.cpp$")


## Targets ##
add_library(${PROJECT_NAME} SHARED ${cpp_files})
target_include_directories(${PROJECT_NAME} PUBLIC
  $<BUILD_INTERFACE:${include_dir}>
  $<INSTALL_INTERFACE:include/${PROJECT_NAME}>
)

target_add_dependency(${PROJECT_NAME} std_e)
target_add_dependency(${PROJECT_NAME} cpp_cgns)
target_add_thirdparty_dependency(${PROJECT_NAME} MPI REQUIRED COMPONENTS CXX)
target_link_libraries(${PROJECT_NAME}
  PUBLIC 
    std_e::std_e
    cpp_cgns::cpp_cgns
    range-v3::range-v3
    MPI::MPI_CXX
)


## Install ##
target_install(${PROJECT_NAME})

## CPython interface
set(cpython_project_name maia_cpython)
add_library(${cpython_project_name} SHARED ${cpython_files})
target_link_libraries(${cpython_project_name}
  PUBLIC 
    maia::maia
)
set_target_properties(
  ${cpython_project_name}
  PROPERTIES
    PREFIX ""
    OUTPUT_NAME "${cpython_project_name}"
    LINKER_LANGUAGE C
)
install(TARGETS ${cpython_project_name} EXPORT ${cpython_project_name}Targets
  LIBRARY DESTINATION py
)
install(FILES ${python_files}
  DESTINATION py
)

## Unit tests ##
option(MAIA_ENABLE_TEST "Enable testing for ${PROJECT_NAME}" ON)
if(MAIA_ENABLE_TEST)
  enable_testing()
  include(CTest)
  add_executable(${PROJECT_NAME}_unit_tests
    ${test_files}
    ${PROJECT_ROOT}/external/project_utils/src/unit_test_main.cpp
  )
  target_link_libraries(${PROJECT_NAME}_unit_tests
    PUBLIC
      ${PROJECT_NAME}
    PRIVATE
      doctest::doctest
  )
  add_test(NAME ${PROJECT_NAME}_unit_tests COMMAND ${PROJECT_NAME}_unit_tests)
endif()

option(MAIA_ENABLE_COVERAGE "Enable coverage for ${PROJECT_NAME}" OFF)
if(MAIA_ENABLE_COVERAGE)
  if(NOT MAIA_ENABLE_TEST)
    message(FATAL_ERROR "You need to enable testing to enable coverage: MAIA_ENABLE_TEST is OFF")
  endif()
  # TODO -> Make a correct exclude directory
  # NOTE : not working with gcc-9.2
  include(CodeCoverage)
  APPEND_COVERAGE_COMPILER_FLAGS()

  set(COVERAGE_EXCLUDES "/opt/tools/gcc-8.3/include/c++/8.3.0/ext"
                        "/opt/tools/gcc-8.3/include/c++/8.3.0/*"
                        "/opt/tools/gcc-8.3/include/c++/8.3.0/bits/*")
  SETUP_TARGET_FOR_COVERAGE_LCOV(
    NAME ${PROJECT_NAME}_coverage
    DEPENDENCIES ${PROJECT_NAME}_unit_tests
    EXECUTABLE ctest -j ${n_cores} # Executable in PROJECT_BINARY_DIR
  )
endif()
# --------------------------------------------------------------------------------

# ----------------------------------------------------------------------
option(MAIA_BUILD_DOCUMENTATION "Build ${PROJECT_NAME} documentation" OFF)
if(MAIA_BUILD_DOCUMENTATION)
  # Download bootstrap style
  FetchContent_Declare(
    sphinx_bootstrap_theme
    GIT_REPOSITORY https://github.com/ryan-roemer/sphinx-bootstrap-theme.git
    GIT_TAG        v0.8.0
  )
  FetchContent_GetProperties(sphinx_bootstrap_theme)
  if(NOT sphinx_bootstrap_theme_POPULATED)
    FetchContent_Populate(sphinx_bootstrap_theme)
  endif()

  include(UseBreathe)
  add_breathe_doc(TARGET_NAME ${PROJECT_NAME}_doc
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/doc
    DOXY_FILE  ${CMAKE_CURRENT_SOURCE_DIR}/doc/Doxyfile.in
    CONF_FILE  ${CMAKE_CURRENT_SOURCE_DIR}/doc/conf.py.in
    BUILD_DIR  ${CMAKE_CURRENT_BINARY_DIR}/_build
    CACHE_DIR  ${CMAKE_CURRENT_BINARY_DIR}/_doctrees
    HTML_DIR   ${CMAKE_CURRENT_BINARY_DIR}/html
    COMMENT "HTML documentation"
  )

  install(DIRECTORY   ${CMAKE_CURRENT_BINARY_DIR}/html
          DESTINATION ${CMAKE_INSTALL_PREFIX}/share/doc)
endif()
# ----------------------------------------------------------------------

# ----------------------------------------------------------------------
include(FeatureSummary)
FEATURE_SUMMARY(WHAT ALL)
# ----------------------------------------------------------------------
