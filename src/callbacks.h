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
    extern unsigned int WINX, WINY;
    extern ospcommon::vec2i WINSIZE;

    // texture maps
    extern uint32_t*          ofb;
    extern cyGLRenderBuffer2D gfb;

    // OSPRay objects
    extern OSPModel       world;    
    extern OSPRenderer    renderer;
    extern OSPFrameBuffer framebuffer;

    // camera objects
    class Camera : public Trackball {
    public:
	float zoom = 1.0f;
	ospcommon::vec3f focus = ospcommon::vec3f(0, 0, 0);
	ospcommon::vec3f pos = ospcommon::vec3f(0, 0, 10);
	ospcommon::vec3f up = ospcommon::vec3f(0, 1, 0);
	ospcommon::vec3f dir = focus - pos;
	Trackball rotate = Trackball(true);
	OSPCamera ospCamera;
    public:
	void Update(bool cleanbuffer = true);
	void Init() 
	{
	    ospCamera = ospNewCamera("perspective");
	    ospSetf(ospCamera, "aspect", 
		    static_cast<float>(WINSIZE.x) / 
		    static_cast<float>(WINSIZE.y));
	    this->Update(false);
	}
    };
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
