IF (WIN32)
  add_library(imgui
    imgui.cpp
    imgui_demo.cpp
    imgui_draw.cpp
    examples/freeglut_opengl2_example/imgui_impl_glut.cpp
    )
ELSE()
  add_library(imgui SHARED
    imgui.cpp
    imgui_demo.cpp
    imgui_draw.cpp
    examples/freeglut_opengl2_example/imgui_impl_glut.cpp
    )
ENDIF()
TARGET_LINK_LIBRARIES(imgui ${GLUT_LIBRARIES} ${GLEW_LIBRARIES} ${OPENGL_LIBRARIES})
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
