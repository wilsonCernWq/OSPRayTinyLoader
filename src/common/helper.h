/**
 * This file defines all helper functions.
 * Those functions shouldn't depend on any thing in this project
 */
#pragma once
#ifndef _HELPER_H_
#define _HELPER_H_

#include "common.h"

#ifndef NDEBUG
# define DEBUG_VECTOR(n, t)			\
  static void debug(const cy::Point##n##t& m) {	\
    std::cout << std::endl;			\
    for (int i = 0; i < n; ++i) {		\
      std::cout << "\t" << m[i];		\
    }						\
    std::cout << std::endl;			\
  }                                             \
  static void debug(const cy::Matrix##n##t& m) {	\
    std::cout << std::endl;				\
    for (int i = 0; i < n; ++i) {			\
      for (int j = 0; j < n; ++j) {			\
        std::cout << "\t" << m(i, j);			\
      }							\
      std::cout << std::endl;				\
    }							\
    std::cout << std::endl;				\
  }
#else
# define DEBUG_VECTOR (n, t)                     \
  static void debug(const cy::Point##n##t& m) {} \
  static void debug(const cy::Matrix##n##t& m) {}
#endif                                                     

#define CAST_VECTOR(n, t)						\
  static ospcommon::vec##n##t make_vec(const cy::Point##n##t& m) {	\
    ospcommon::vec##n##t v;						\
    for (int i = 0; i < n; ++i) {					\
      v[i] = m[i];							\
    }									\
    return v;								\
  }

#define DEFINE_MATH_TYPES(type, t)    \
  typedef glm::tvec2<type> vec2##t;   \
  typedef glm::tvec3<type> vec3##t;   \
  typedef glm::tvec4<type> vec4##t;   \
  typedef glm::tmat2x2<type> mat2##t; \
  typedef glm::tmat3x3<type> mat3##t; \
  typedef glm::tmat4x4<type> mat4##t; \
  struct bbox2##t { vec2##t upper, lower; }; \
  struct bbox3##t { vec3##t upper, lower; }; \
  struct bbox4##t { vec4##t upper, lower; }; 

namespace cy {
  typedef Point2<int> Point2i;
  typedef Point3<int> Point3i;
  typedef Point4<int> Point4i;
  typedef Matrix2<int> Matrix2i;
  typedef Matrix3<int> Matrix3i;
  typedef Matrix4<int> Matrix4i;

};

typedef cy::Point2<int> cyPoint2i;
typedef cy::Point3<int> cyPoint3i;
typedef cy::Point4<int> cyPoint4i;
typedef cy::Matrix2<int> cyMatrix2i;
typedef cy::Matrix3<int> cyMatrix3i;
typedef cy::Matrix4<int> cyMatrix4i;

namespace otv 
{
  DEBUG_VECTOR(2,f);
  DEBUG_VECTOR(3,f);
  DEBUG_VECTOR(4,f);
  DEBUG_VECTOR(2,i);
  DEBUG_VECTOR(3,i);
  DEBUG_VECTOR(4,i);
};

namespace otv 
{
  CAST_VECTOR(2,i);
  CAST_VECTOR(3,i);
  CAST_VECTOR(4,i);
  CAST_VECTOR(2,f);
  CAST_VECTOR(3,f);
  CAST_VECTOR(4,f);

  DEFINE_MATH_TYPES(int,    i);
  DEFINE_MATH_TYPES(float,  f);
  DEFINE_MATH_TYPES(double, d);
};

namespace otv 
{
  struct ImageData {
    std::vector<unsigned char> data;
    unsigned int width = 0, height = 0;
    unsigned int channel = 0;
    bool IsEmpty() { return (width * height) <= 0; }
    vec2i Size() { return vec2i(width, height); }
  };

  //! @name mouse2screen: convert mouse coordinate to [-1,1] * [-1,1]
  void mouse2screen(int x, int y, float width, float height, cy::Point2f& p);

  //! @name load file into string
  std::string loadfile
    (const char *filename, std::ostream *outStream = &std::cout);

  //! @name copychar: copy string data to char pointer
  void copychar(char * &str, const std::string& src, int start = 0);

  //! @name loadimg
  void loadimgActual
    (ImageData& image, const char* filename, const std::string path);

  void loadimg
    (ImageData& image, const std::string filename, const std::string path);

  void loadimg
    (ImageData& image, const char* filename, const std::string path);

  //! @name writePPM Helper function to write the rendered image as PPM file
  void writePPM
    (const char *fileName, 
     const ospcommon::vec2i &size, 
     const uint32_t *pixel);

};

#endif//_HELPER_H_
