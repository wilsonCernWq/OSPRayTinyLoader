#pragma once
#ifndef _LIGHT_H_
#define _LIGHT_H_

#include "common.h"
#include "helper.h"
#include "trackball.h"

namespace otv {
  // light
  class Light : public Trackball {
  private:
    float Iamb = 0.5f;
    float Idir = 2.00f;
    // ambient light
    OSPLight ospAmb = nullptr;
    // directional light
    OSPLight ospDir = nullptr;
    float zoom = 1.0f;
    ospcommon::vec3f focus = ospcommon::vec3f(0, 0, 0);
    ospcommon::vec3f pos = ospcommon::vec3f(0, 0, 2);
    ospcommon::vec3f up = ospcommon::vec3f(0, 1, 0);
    ospcommon::vec3f dir = focus - pos;
  public:
    void Update();
    void Init(OSPRenderer& renderer) {
      if (ospAmb) { ospRelease(ospAmb); }
      ospAmb = ospNewLight(renderer, "AmbientLight");
      if (ospDir) { ospRelease(ospDir); }
      ospDir = ospNewLight(renderer, "DirectionalLight");
      Update();
      // setup light data
      std::vector<OSPLight> lightslist = {ospAmb, ospDir};
      OSPData lightsdata;
      lightsdata = ospNewData(lightslist.size(), OSP_OBJECT, 
			      lightslist.data());
      ospCommit(lightsdata);
      ospSetData(renderer, "lights", lightsdata);
    }
  };
};

#endif//_LIGHT_H_
