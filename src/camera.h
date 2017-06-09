#pragma once
#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "common.h"
#include "helper.h"
#include "trackball.h"
#include "framebuffer.h"

namespace otv {

    class Camera : public Trackball {
    private:
	float zoom = 1.0f;
	ospcommon::vec3f focus = ospcommon::vec3f(0, 0, 0);
	ospcommon::vec3f pos = ospcommon::vec3f(0, 0, 10);
	ospcommon::vec3f up = ospcommon::vec3f(0, 1, 0);
	ospcommon::vec3f dir = focus - pos;
	OSPCamera ospCam;
    public:
        Camera() = default;
        ~Camera() { Clean(); }
	void SetZoomIn() { this->zoom *= 0.9f; }
	void SetZoomOut() { this->zoom /= 0.9f; }
	void SetZoom(float zoom) { this->zoom = zoom; }
	void SetFocus(ospcommon::vec3f focus) { this->focus = focus; }   
        void Update(FrameBuffer* framebuffer = nullptr);
	void Init(ospcommon::vec2i size) 
	{
	    ospCam = ospNewCamera("perspective");
	    ospSetf(ospCam, "aspect", 
		    static_cast<float>(size.x) / 
		    static_cast<float>(size.y));
	    this->Update(nullptr);
	}
        void Clean() { ospRelease(this->ospCam); }
        OSPCamera& GetOSPCamera() { return ospCam; }
    };

};

#endif//_CAMERA_H_
