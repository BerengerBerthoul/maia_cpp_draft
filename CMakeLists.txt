## Global config ##
cmake_minimum_required(VERSION 3.12)

execute_process(COMMAND git rev-parse --show-superproject-working-tree
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  OUTPUT_VARIABLE git_root_dir_with_space
)
string(STRIP "${git_root_dir_with_space}" git_root_dir)
if("${git_root_dir}" STREQUAL "")
  set(git_root_dir ".")
endif()
include(${git_root_dir}/external/project_utils/scripts/cmake/target_add_dependency.cmake)

set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")

## Project ##
project(
  maia VERSION 0.1.0
  DESCRIPTION	"Mesh algorithms and manipulations"
  LANGUAGES	CXX
)


## Find files ##
set(src_dir ${PROJECT_SOURCE_DIR}/${PROJECT_NAME})
set(include_dir ${PROJECT_SOURCE_DIR})

file(GLOB_RECURSE cpp_and_test_files
  "${src_dir}/*.cpp"
)
set(cpp_files ${cpp_and_test_files})
list(FILTER cpp_files EXCLUDE REGEX ".*\.test\.cpp$")
set(test_files ${cpp_and_test_files})
list(FILTER test_files INCLUDE REGEX ".*\.test\.cpp$")


## Compiler flags ##
# TODO move
add_compile_options(-Wall -fmax-errors=3 -std=c++17 -O3)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -fno-math-errno -march=native -DNDEBUG -fopenmp -fPIC")


## Targets ##
add_library(${PROJECT_NAME} SHARED ${cpp_files})
#target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)
target_include_directories(${PROJECT_NAME} PUBLIC
  $<BUILD_INTERFACE:${include_dir}>
  $<INSTALL_INTERFACE:include/${PROJECT_NAME}>
)

target_add_dependency(${PROJECT_NAME} std_e)
target_add_dependency(${PROJECT_NAME} cpp_cgns)
add_subdirectory(tmp/range-v3) # TODO use target_add_dependency (adapt it)
#add_subdirectory(tmp/cmcstl2) # TODO use target_add_dependency (adapt it)
target_link_libraries(${PROJECT_NAME}
  PUBLIC 
    std_e::std_e
    cpp_cgns::cpp_cgns
    range-v3
    #stl2
)

add_executable(${PROJECT_NAME}_unit_tests ${test_files})
target_add_dependency(${PROJECT_NAME}_unit_tests doctest)
target_link_libraries(${PROJECT_NAME}_unit_tests
  PUBLIC
    ${PROJECT_NAME}
  PRIVATE
    doctest::doctest
)


## Install ##
install(TARGETS ${PROJECT_NAME} EXPORT ${PROJECT_NAME}Targets
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  RUNTIME DESTINATION bin
  INCLUDES DESTINATION include
)
install(EXPORT ${PROJECT_NAME}Targets
  FILE ${PROJECT_NAME}Targets.cmake
  NAMESPACE ${PROJECT_NAME}::
  DESTINATION lib/cmake/${PROJECT_NAME}
)
install(DIRECTORY ${src_dir} DESTINATION include FILES_MATCHING PATTERN "*.hpp")

set(TARGET_NAME ${PROJECT_NAME})
string(REPLACE ";" " " TARGET_DEPENDENCIES_FIND_PACKAGE_STRING "${${PROJECT_NAME}_DEPENDENCIES_FIND_PACKAGE_STRING}")
configure_file(
  ${git_root_dir}/external/project_utils/scripts/cmake/target_config.cmake.in
  ${PROJECT_NAME}Config.cmake
  @ONLY
)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
  DESTINATION lib/cmake/${PROJECT_NAME}
)

add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
