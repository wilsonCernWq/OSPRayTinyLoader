/**
 * This file defines all helper functions.
 * Those functions shouldn't depend on any thing in this project
 */
#pragma once
#ifndef _HELPER_H_
#define _HELPER_H_

#include "common.h"
#include <glm/gtc/matrix_access.hpp>

#ifndef NDEBUG
# define DEBUG_VECTOR(n, t)				\
  static void debug(const cy::Point##n##t& m) {		\
    std::cout << std::endl;				\
    for (int i = 0; i < n; ++i) {			\
      std::cout << "\t" << m[i];			\
    }							\
    std::cout << std::endl;				\
  }							\
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
# define DEBUG_VECTOR(n, t)                             \
  static void debug(const cy::Point##n##t& m)  {;}	\
  static void debug(const cy::Matrix##n##t& m) {;}
#endif                                                     
#define CAST_VECTOR(n, t)				\
  static ospcommon::vec##n##t				\
  make_vec(const cy::Point##n##t& m)			\
  {							\
    ospcommon::vec##n##t v;				\
    for (int i = 0; i < n; ++i) {			\
      v[i] = m[i];					\
    }							\
    return v;						\
  }  
#define DEFINE_MATH_TYPES(TYPE, N, T)			\
  /* cyCodeBase */					\
  namespace cy {					\
    typedef Point##N<TYPE> Point##N##T;			\
    typedef Matrix##N<TYPE> Matrix##N##T;		\
  };							\
  typedef cy::Point##N<TYPE> cyPoint##N##T;		\
  typedef cy::Matrix##N<TYPE> cyMatrix##N##T;		\
  namespace otv						\
  {							\
    DEBUG_VECTOR(N,T);					\
    CAST_VECTOR(N,T);					\
  };							\
  /* glm */						\
  namespace otv						\
  {							\
    typedef glm::tvec##N<TYPE> vec##N##T;		\
    typedef glm::tmat##N##x##N<TYPE> mat##N##T;		\
    struct bbox##N##T { vec##N##T upper, lower; };	\
    struct affine##N##T {				\
      union { mat##N##T rotation; mat##N##T l; };	\
      union { vec##N##T translation; vec##N##T p; };	\
      affine##N##T() :					\
        p(vec##N##T(0.0f)),				\
	l(mat##N##T(1.0f))				\
	{}  						\
    };							\
  };

#define DEFINE_ALL_TYPES(type, t)			\
  DEFINE_MATH_TYPES(type, 2, t)				\
  DEFINE_MATH_TYPES(type, 3, t)				\
  DEFINE_MATH_TYPES(type, 4, t)

// define all types
DEFINE_ALL_TYPES(int,    i);
DEFINE_ALL_TYPES(float,  f);
DEFINE_ALL_TYPES(double, d);

namespace otv 
{
  //! @name ImageData structure to store an loaded image
  struct ImageData
  {
    std::vector<unsigned char> data;
    unsigned int width = 0, height = 0, depth = 0;
    unsigned int channel = 0;
    bool IsEmpty() { return (width * height) <= 0; }
    vec2i Size() { return vec2i(width, height); }
    OSPTexture2D CreateOSPTex();
  };
  
  //! @name load file into string
  std::string loadfile
    (const char *filename, std::ostream *outStream = &std::cout);

  //! @name loadimg  
  void loadimg
    (ImageData& image, const std::string filename, const std::string path);
  void loadimg
    (ImageData& image, const char* filename, const std::string path);

  //! @name mouse2screen: convert mouse coordinate to [-1,1] * [-1,1]
  void mouse2screen(int x, int y, float width, float height, cy::Point2f& p);

  //! @name copychar: copy string data to char pointer
  void copychar(char * &str, const std::string& src, int start = 0);

  //! @name writePPM Helper function to write the rendered image as PPM file
  void writePPM
    (const char *fileName, const ospcommon::vec2i &size, const uint32_t *pixel);

  //! function to create warning and error
  void WarnOnce(std::string str);
  void WarnAlways(std::string str);
  void ErrorNoExit(std::string str);
  void ErrorFatal(std::string str);
};

#endif//_HELPER_H_
