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
    vec3f pos   = vec3f(0.0f, 1.0f, 1.0f);
    vec3f up    = vec3f(0.0f, 1.0f, 0.0f);
    vec3f dir = focus - pos;
    OSPCamera ospCam = nullptr;
  public:
    Camera() = default;
    ~Camera() { Clean(); }
    
    OSPCamera& GetOSPCamera() { return ospCam; }
    vec3f GetFocus() { return focus; }
    vec3f GetPos() { return pos; }
    vec3f GetDir() { return dir; }
    vec3f GetUp() { return up; }
    float GetZoom() { return zoom; }
    
    void SetZoomIn() { this->zoom *= 0.9f; }
    void SetZoomOut() { this->zoom /= 0.9f; }
    void SetZoom(float zoom) { this->zoom = zoom; }
    void SetFocus(vec3f focus) { this->focus = focus; }
    
    void Update();
    void Init(vec2i size);
    void Clean();
  };

};

#endif//_CAMERA_H_
