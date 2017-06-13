/**
 * This file defines all helper functions.
 * Those functions shouldn't depend on any thing in this project
 */
#pragma once
#ifndef _CALLBACKS_H_
#define _CALLBACKS_H_

#include "common.h"
#include "helper.h"
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

    // light
    class Light : public Trackball {
    private:
	float Iamb = 0.9f;
	float Idir = 2.00f;
	// ambient light
	OSPLight ospAmb;
	// directional light
	OSPLight ospDir;
	float zoom = 1.0f;
	ospcommon::vec3f focus = ospcommon::vec3f(0, 0, 0);
	ospcommon::vec3f pos = ospcommon::vec3f(0, 0, 2);
	ospcommon::vec3f up = ospcommon::vec3f(0, 1, 0);
	ospcommon::vec3f dir = focus - pos;
	// light list
	std::vector<OSPLight> lightslist;
	OSPData lightsdata;
    public:
	void Update();
	void Init(OSPRenderer& renderer) {
	    ospAmb = ospNewLight(renderer, "AmbientLight");
	    ospDir = ospNewLight(renderer, "DirectionalLight");
	    Update();
	    // setup light data
	    lightslist.push_back(ospAmb);
	    lightslist.push_back(ospDir);
	    lightsdata = ospNewData(lightslist.size(), OSP_OBJECT, 
				    lightslist.data());
	    ospCommit(lightsdata);
	    ospSetData(renderer, "lights", lightsdata);
	}
	OSPData& GetLightsData() { return lightsdata; } 

    };
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
