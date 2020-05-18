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
  LANGUAGES CXX
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
set(cpp_files ${cpp_and_test_files})
list(FILTER cpp_files EXCLUDE REGEX ".*\.test\.cpp$")
set(test_files ${cpp_and_test_files})
list(FILTER test_files INCLUDE REGEX ".*\.test\.cpp$")


## Targets ##
add_library(${PROJECT_NAME} SHARED ${cpp_files})
target_include_directories(${PROJECT_NAME} PUBLIC
  $<BUILD_INTERFACE:${include_dir}>
  $<INSTALL_INTERFACE:include/${PROJECT_NAME}>
)

target_add_dependency(${PROJECT_NAME} std_e)
target_add_dependency(${PROJECT_NAME} cpp_cgns)
target_link_libraries(${PROJECT_NAME}
  PUBLIC 
    std_e::std_e
    cpp_cgns::cpp_cgns
    range-v3::range-v3
)

add_executable(${PROJECT_NAME}_unit_tests ${test_files})
target_link_libraries(${PROJECT_NAME}_unit_tests
  PUBLIC
    ${PROJECT_NAME}
  PRIVATE
    doctest::doctest
)


## Install ##
target_install(${PROJECT_NAME})
