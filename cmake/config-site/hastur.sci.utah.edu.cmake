#
# configuration for hastur
#   written by Qi, WU 28 Feb 2017
#
set(embree_DIR ~/software/embree-2.14.0.x86_64.linux/lib/cmake/embree-2.14.0)
set(ospray_DIR ~/software/ospray-qwu-23.05.2017/lib64/cmake/ospray-1.3.0)
set(TBB_ROOT ~/software/tbb2017_20160916oss)
set(ISPC_EXECUTABLE ~/software/ispc-v1.9.1-linux)
#
# don't want it to appear in the cmake main window
mark_as_advanced(embree_DIR ospray_DIR)
