cmake_minimum_required(VERSION 3.13)
project(EMPSIPON C CXX)

# set c/c++ standard and inline option
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(lib_source_files
    src/Auxiliary.cpp
    src/b3D_integrator.cpp
    src/distributions.cpp
    src/domain.cpp
    src/dump.cpp
    src/error.cxx
    src/grid.cpp
    src/hopping.cpp
    src/netmin.cpp
    src/network.cpp
    src/non_bonded_scheme_routines.cpp
    src/rng.cpp)

# uncomment to enable constant slip springs mode
# add_compile_definitions(CONST_SL_SCHEME)
add_compile_definitions(CHECKDISPLACEMENT)

add_library(EmpsiponCore ${lib_source_files})
target_include_directories(EmpsiponCore PUBLIC "src/include")

add_executable(netmin.x src/main.cpp)
target_link_libraries(netmin.x PRIVATE EmpsiponCore)

file(COPY ./configurations
    DESTINATION ${CMAKE_BINARY_DIR})
