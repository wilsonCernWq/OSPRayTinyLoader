#pragma once
#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "common/helper.h"
#include "common/trackball.h"

namespace otv {

  class Camera : public Trackball {
  private:
    float zoom = 1.0f;
    vec3f focus = vec3f(0.0f);
    vec3f pos   = vec3f(0.0f, 0.0f, 10.0f);
    vec3f up    = vec3f(0.0f, 1.0f, 0.0f);
    vec3f dir = focus - pos;
    OSPCamera ospCam = nullptr;
  public:
    Camera() = default;
    ~Camera() { Clean(); }
    void SetZoomIn() { this->zoom *= 0.9f; }
    void SetZoomOut() { this->zoom /= 0.9f; }
    void SetZoom(float zoom) { this->zoom = zoom; }
    void SetFocus(vec3f focus) { this->focus = focus; }   
    void Update();
    void Init(vec2i size) 
    {
      ospCam = ospNewCamera("perspective");
      ospSetf(ospCam, "aspect", 
	      static_cast<float>(size.x) / 
	      static_cast<float>(size.y));
      Update();
    }
    void Clean() {
      if (ospCam != nullptr) {
	ospRelease(this->ospCam);	 
	ospCam = nullptr;
      }
    }
    OSPCamera& GetOSPCamera() { return ospCam; }
  };

};

#endif//_CAMERA_H_
