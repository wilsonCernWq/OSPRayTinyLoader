cmake_minimum_required(VERSION 2.8)
project(OSPRayTinyLoader)
#
# include host specification file
cmake_host_system_information(RESULT SYSNAME QUERY HOSTNAME)
message(STATUS "trying to include ${SYSNAME}.cmake configuration file")
include(cmake/config-site/${SYSNAME}.cmake OPTIONAL RESULT_VARIABLE CONFIGFOUND)
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
#
# load required libraries
include(cmake/common.cmake)
#
#
include_directories("src")
#
# install dlls or shader
file(GLOB_RECURSE SHADERS "src/*.glsl")
set(COPYLIST ${COMMON_DLLS} ${SHADERS})
if(EXECUTABLE_OUTPUT_PATH)
  DeployRepo(${COPYLIST} ${EXECUTABLE_OUTPUT_PATH})
else()
  DeployRepo("${COPYLIST}" "${CMAKE_BINARY_DIR}")
endif()
#
# include sources
file(GLOB_RECURSE MAIN "src/main.cpp")
file(GLOB_RECURSE CXX   "src/*.cpp")
file(GLOB_RECURSE COMMON_CXX "src/common/*.cpp")
file(GLOB_RECURSE LOADER_CXX "src/loader/*.cpp")
file(GLOB_RECURSE OPENGL_CXX "src/opengl/*.cpp")
file(GLOB_RECURSE OSPRAY_CXX "src/ospray/*.cpp")
foreach(f ${MAIN} ${COMMON_CXX} ${LOADER_CXX} ${OPENGL_CXX} ${OSPRAY_CXX})
  list(REMOVE_ITEM CXX ${f})
endforeach()

add_library(osp_common SHARED ${COMMON_CXX})
target_link_libraries(osp_common ${COMMON_LIBS} ${OSPRAY_LIBRARIES})

add_library(osp_loader SHARED ${LOADER_CXX})
target_link_libraries(osp_loader osp_common)

add_library(osp_opengl SHARED ${OPENGL_CXX})
target_link_libraries(osp_opengl osp_common)

add_library(osp_ospray SHARED ${OSPRAY_CXX})
target_link_libraries(osp_ospray osp_common)

add_library(osp_glob SHARED ${CXX})
target_link_libraries(osp_glob osp_common osp_loader osp_opengl osp_ospray)

add_executable(loader ${MAIN})
target_link_libraries(loader osp_glob)
