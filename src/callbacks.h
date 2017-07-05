/**
 * This file defines all helper functions.
 * Those functions shouldn't depend on any thing in this project
 */
#pragma once
#ifndef _CALLBACKS_H_
#define _CALLBACKS_H_

#include "common.h"
#include "helper.h"
#include "light.h"
#include "camera.h"
#include "meshwrapper.h"
#include "framebuffer.h"

// Global Variables
namespace otv 
{
    // window size
    extern unsigned int WINX, WINY;
    extern ospcommon::vec2i WINSIZE;

    // OSPRay objects
    extern OSPModel       world;    
    extern OSPRenderer    renderer;

    // light object
    extern Light light;

    // framebuffer object
    extern FrameBuffer framebuffer;

    // camera objects
    extern Camera camera;

    // mesh
    extern Mesh mesh;
};

namespace otv 
{
    void KeyboardAction(int key, int x, int y);

    void GetMouseButton(GLint button, GLint state, GLint x, GLint y);
    void GetMousePosition(GLint x, GLint y);

    void GetNormalKeys(unsigned char key, GLint x, GLint y);
    void GetSpecialKeys(int key, GLint x, GLint y);

    void Clean();

    inline void Idle() { glutPostRedisplay(); }
};

#endif//_CALLBACKS_H_
