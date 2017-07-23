#pragma once
#ifndef _GLUT_BINDING_H_
#define _GLUT_BINDING_H_

#include <GL/glew.h>
#ifdef APPLE // apple specific header
# include <OpenGL/gl.h>
# include <OpenGL/glu.h>
# include <GLUT/glut.h>
#else // Windows need special header
# if defined(WIN32) || defined(_WIN32) || defined(WIN32) && !defined(CYGWIN)
#  define NOMINMAX
#  include <Windows.h>
# else
#  if unix // Linux needs extensions for framebuffers
#   define GL_GLEXT_PROTOTYPES 1
#   include <GL/glext.h>
#  endif
# endif
# include <GL/gl.h>
# include <GL/glu.h>
# include <GL/glut.h>
# include <GL/freeglut.h>
#endif
#ifndef EXIT_SUCCESS
# define EXIT_SUCCESS 0
#endif
#ifndef EXIT_FAILURE
# define EXIT_FAILURE 1
#endif

#include "global.h"

//! @name error check helper from EPFL ICG class
static inline const char* ErrorString(GLenum error) {
  const char* msg;
  switch (error) {
#define Case(Token)  case Token: msg = #Token; break;
    Case(GL_INVALID_ENUM);
    Case(GL_INVALID_VALUE);
    Case(GL_INVALID_OPERATION);
    Case(GL_INVALID_FRAMEBUFFER_OPERATION);
    Case(GL_NO_ERROR);
    Case(GL_OUT_OF_MEMORY);
#undef Case
  }
  return msg;
}

//! @name check error
static inline void _glCheckError
(const char* file, int line, const char* comment) {
  GLenum error;
  while ((error = glGetError()) != GL_NO_ERROR) {
    fprintf(stderr, "ERROR: %s (file %s, line %i: %s).\n",
	    comment, file, line, ErrorString(error));
  }
}

#ifndef NDEBUG
# define check_error_gl(x) _glCheckError(FILE, LINE, x)
#else
# define check_error_gl() ((void)0)
#endif

#endif//_GLUT_BINDING_H_
