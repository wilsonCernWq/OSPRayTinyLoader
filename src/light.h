#pragma once
#ifndef _LIGHT_H_
#define _LIGHT_H_

#include "common.h"
#include "helper.h"
#include "trackball.h"

namespace otv {

  class Light : public Trackball {
  private:
    float Iamb = 0.5f;
    float Idir = 2.00f;
    // lights
    OSPLight ospAmb = nullptr;
    OSPLight ospDir = nullptr;
    OSPLight ospSun = nullptr;
    float zoom = 1.0f;
    vec3f focus = vec3f(0, 0, 0);
    vec3f pos   = vec3f(0, 0, 2);
    vec3f up    = vec3f(0, 1, 0);
    vec3f dir = focus - pos;
    // ospray data
    OSPData lightsdata;
  public:
    OSPData& GetOSPLights() { return lightsdata; }
    void Clean() {}
    void Update();
    void Init(OSPRenderer renderer) {
      if (ospAmb != nullptr) { ospRelease(ospAmb); }
      ospAmb = ospNewLight(renderer, "AmbientLight");
      if (ospDir != nullptr) { ospRelease(ospDir); }
      ospDir = ospNewLight(renderer, "DirectionalLight");
      Update();
      // setup light data
      std::vector<OSPLight> lightslist = {ospAmb, ospDir};
      lightsdata = ospNewData(lightslist.size(), OSP_OBJECT, lightslist.data());
      ospCommit(lightsdata);
    }
  };
};

#endif//_LIGHT_H_
