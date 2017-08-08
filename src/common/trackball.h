/**
 * This file defines the trackball class
 * which is independent to the project itself.
 */
#pragma once
#ifndef _TRACKBALL_H_
#define _TRACKBALL_H_

#include "common.h"
#include "helper.h"
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtx/transform.hpp>

namespace otv {
  //
  // following the implementation of
  // http://image.diku.dk/research/trackballs/index.html
  //
  class Trackball {
  private:
    bool  invrot = false; // inverse rotation
    float radius = 1.0f;
    mat4f matrix      = mat4f(1.0f);
    mat4f matrix_prev = mat4f(1.0f);
    vec3f position;
    vec3f position_prev;
    float position_surf[2];
  public:
    /** constractors */
    Trackball() {}
    Trackball(bool i) : invrot(i) {}

    void SetRadius(const float r) { radius = r; }
    void SetInverseMode(bool r) { invrot = r; }

    /**
     * @brief BeginDrag/Zoom: initialize drag/zoom
     * @param x: previous x position
     * @param y: previous y position
     */
    void BeginDrag(float x, float y);

    /**
     * @brief Drag/Zoom: execute Drag/Zoom
     * @param x: current x position
     * @param y: current y position
     */
    void Drag(float x, float y);
    
    /**
     * @brief matrix: trackball matrix accessor
     * @return current trackball matrix
     */
    mat4f Matrix() { return matrix; }

    void Reset() { matrix = mat4f(1.0f); }
    void Reset(const mat4f& m) { matrix = m; }

  private:
    /**
     * @brief proj2surf: project (x,y) mouse pos on surface
     * @param x: X position
     * @param y: Y position
     * @return projected position
     */
    vec3f proj2surf(const float x, const float y) const;    
  };

  //! @brief a ball that can orbit around a center
  struct OrbitalPoint: public Trackball {
    float zoom = 1.0f;
    vec3f focus;
    vec3f  p,  u,  d; /* position, up, direction */
    vec3f cp, cu, cd; /* position, up, direction */
    OrbitalPoint() = default;
    OrbitalPoint
      (const vec3f& f, const vec3f& ip, const vec3f& iu)
      : focus(f), p(ip), u(iu), d(f - ip) {}
    void Update() {
      cu = vec3f(Trackball::Matrix() * vec4f(u, 0.0f));
      cd = vec3f(Trackball::Matrix() * vec4f(d, 0.0f));
      cp = focus - cd * zoom;
    }
  };

};

#endif//_TRACKBALL_H_
