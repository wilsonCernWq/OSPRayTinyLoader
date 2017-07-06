/**
 * This file defines the trackball class
 * which is independent to the project itself.
 */
#pragma once
#ifndef _TRACKBALL_H_
#define _TRACKBALL_H_

#include "common.h"
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtx/transform.hpp>

namespace otv {
  //
  // following the implementation of http://image.diku.dk/research/trackballs/index.html
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
    void BeginDrag(float x, float y)
    {
      position_prev = proj2surf(x, y);
      position_surf[0] = x;
      position_surf[1] = y;
      matrix_prev = matrix;
    }

    /**
     * @brief Drag/Zoom: execute Drag/Zoom
     * @param x: current x position
     * @param y: current y position
     */
    void Drag(float x, float y)
    {
      // get direction
      position = proj2surf(x, y);
      vec3f dir = (invrot ? -1.0f : 1.0f) *
	glm::normalize(glm::cross(position_prev, position));
      // compute rotation angle
      float angle = glm::angle(glm::normalize(position_prev),
			       glm::normalize(position));
      if (angle < 0.01f) {
	// to prevent position_prev == position, this will cause invalid value
	return;
      }
      else { // compute rotation
	matrix = glm::rotate(angle, dir) * matrix_prev;
      }
    }

    /**
     * @brief matrix: trackball matrix accessor
     * @return current trackball matrix
     */
    mat4f Matrix()
    {
      return matrix;
    }

    void Reset() { matrix = mat4f(1.0f); }

  private:
    /**
     * @brief proj2surf: project (x,y) mouse pos on surface
     * @param x: X position
     * @param y: Y position
     * @return projected position
     */
    vec3f proj2surf(const float x, const float y) const
    {
      float r = x * x + y * y;
      float R = radius * radius;
      float z = r > R / 2 ? R / 2 / sqrt(r) : sqrt(R - r);
      return vec3f(x, y, z);
    }
    
  };
};

#endif//_TRACKBALL_H_
