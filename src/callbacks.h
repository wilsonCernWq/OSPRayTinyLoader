/**
 * This file defines all helper functions.
 * Those functions shouldn't depend on any thing in this project
 */
#pragma once
#ifndef _CALLBACKS_H_
#define _CALLBACKS_H_

#include "common.h"
#include "helper.h"
#include "trackball.h"
#include "meshwrapper.h"

// Global Variables
namespace otv 
{
    // window size
    static unsigned int WINX = 0, WINY = 0;
    static ospcommon::vec2i WINSIZE(1024, 1024);

    // texture maps
    static uint32_t*          ofb;
    static cyGLRenderBuffer2D gfb;

    // OSPRay objects
    static OSPModel       world;
    static OSPCamera      camera;
    static OSPRenderer    renderer;
    static OSPFrameBuffer framebuffer;

    // camera objects
    static float camZoom = 1.0f;
    static ospcommon::vec3f camFocus(0, 0, 0);
    static ospcommon::vec3f camPos(0, 0, 10);
    static ospcommon::vec3f camUp(0, 1, 0);
    static ospcommon::vec3f camDir = camFocus - camPos;
    static Trackball camRotate(true);

    // mesh
    static Mesh mesh;
};

namespace otv 
{
    void UpdateCamera(bool cleanbuffer = true);

    void GetMouseButton(GLint button, GLint state, GLint x, GLint y);

    void GetMousePosition(GLint x, GLint y);

    void GetNormalKeys(unsigned char key, GLint x, GLint y);

    void Clean();

    inline void Idle() { glutPostRedisplay(); }
};

#endif//_CALLBACKS_H_
