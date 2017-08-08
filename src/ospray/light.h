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
    void Clean();
    void Update();
    void Init(OSPRenderer renderer);
    /* for convenient */
    float GetAmbStrength() { return Iamb; }
    float GetDirStrength() { return Idir; }
    float GetSunStrength() { return Isun; }
    vec3f GetAmbColor() { return Camb; }
    vec3f GetDirColor() { return Cdir; }
    vec3f GetSunColor() { return Csun; }
    void SetAmbStrength(float I) { Iamb = I; }
    void SetDirStrength(float I) { Idir = I; }
    void SetSunStrength(float I) { Isun = I; }
    void SetAmbColor(vec3f c) { Camb = c; }
    void SetDirColor(vec3f c) { Cdir = c; }
    void SetSunColor(vec3f c) { Csun = c; }
  };
};

#endif//_LIGHT_H_
