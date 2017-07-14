# OSPRayTinyLoader
A mesh viewer program using OSPRay + TinyObjLoader as backends. 

## Build
Basically you have two ways to build the program

First, you can build it directly from command line with `cmake` or `ccmake`:
```
mkdir build
cd build
cmake .. -Dospray_DIR=<...> -Dembree_DIR=<...>    \
         -DTBB_ROOT=<...> -DISPC_EXECUTABLE=<...> \
         -DOSPRAY_USE_EXTERNAL_EMBREE=ON
make -j8
``` 

Second, you can create a configure file called `<your-machine-hostname>.cmake` and but it inside `/cmake/config-site` folder.
This is an example of the configure file
```
set(embree_DIR ~/software/embree-2.14.0.x86_64.linux/lib/cmake/embree-2.14.0)
set(ospray_DIR ~/software/ospray-qwu-23.05.2017/lib64/cmake/ospray-1.3.0)
set(TBB_ROOT ~/software/tbb2017_20160916oss)
set(ISPC_EXECUTABLE ~/software/ispc-v1.9.1-linux)
mark_as_advanced(embree_DIR ospray_DIR)
set(OSPRAY_USE_EXTERNAL_EMBREE ON)
```

## Screenshots
![SciFi_Fighter_MK6](https://github.com/wilsonCernWq/OSPRayTinyLoader/blob/master/assets/Screenshot%20from%202017-07-14%2012-43-37.png)
![Wooddoll](https://github.com/wilsonCernWq/OSPRayTinyLoader/blob/master/assets/Screenshot%20from%202017-07-14%2012-46-23.png)
![Demod_Head](https://github.com/wilsonCernWq/OSPRayTinyLoader/blob/master/assets/Screenshot%20from%202017-07-14%2012-44-21.png)
