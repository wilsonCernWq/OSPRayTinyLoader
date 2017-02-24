cmake_minimum_required(VERSION 3.0)
project(OSPRayTinyLoader)

# include host specification file
cmake_host_system_information(RESULT SYSNAME QUERY HOSTNAME)
message(STATUS "trying to include ${SYSNAME}.cmake configuration file")
include(config-site/${SYSNAME}.cmake OPTIONAL RESULT_VARIABLE CONFIGFOUND)
if (NOT CONFIGFOUND)
  message(STATUS "failed to load the configuration file")
  message(STATUS "helper message:
    To build the program sucessfully, you might need to provide following paths to your cmake
    *) path to your osprayConfig.cmake included in the OSPRay releases to CMake as '-Dospray_DIR=<..>'
    *) path to the embreeConfig.cmake from the Embree releases similarly as '-Dembree_DIR=<..>'
    *) path to the root directory of your standalone TBB library as '-DTBB_ROOT=<..>'
    *) path to the root directory of your standalone ISPC executable as '-DISPC_EXECUTABLE=<..>'
  ")
endif()

# load required libraries
find_package(ospray REQUIRED)

# include sources
include_directories(${OSPRAY_INCLUDE_DIRS})
add_executable(osp_tiny_loader src/ospray_volume_example.cpp)
target_link_libraries(osp_tiny_loader ${OSPRAY_LIBRARIES})
