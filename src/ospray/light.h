#pragma once
#ifndef _LIGHT_H_
#define _LIGHT_H_

#include "common/helper.h"
#include "common/trackball.h"

namespace otv {

  class Light : public Trackball {
  private:
    // lights
    OSPLight ospAmb = nullptr;
    OSPLight ospDir = nullptr;
    OSPLight ospSun = nullptr;
    // light data
    float Iamb;
    float Idir;
    float Isun;
    vec3f Camb;
    vec3f Cdir;
    vec3f Csun;
    OrbitalPoint ptDir;
    OrbitalPoint ptSun;
    // ospray data
    OSPData osplights = nullptr;
  public:
    Light ();
    ~Light () { Clean(); }
    OSPData& GetOSPLights() { return osplights; }
    OrbitalPoint& GetDirLight() { return ptDir; }
    OrbitalPoint& GetSunLight() { return ptSun; }
    void Clean() {
      if (osplights != nullptr) { ospRelease(osplights); osplights = nullptr; }
      if (ospAmb != nullptr) { ospRelease(ospAmb); ospAmb = nullptr; }
      if (ospDir != nullptr) { ospRelease(ospDir); ospDir = nullptr; }
      if (ospSun != nullptr) { ospRelease(ospSun); ospSun = nullptr; }
    }
    void Update();
    void Init(OSPRenderer renderer);
  };
};

#endif//_LIGHT_H_
