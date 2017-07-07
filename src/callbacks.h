/**
 * This file defines all helper functions.
 * Those functions shouldn't depend on any thing in this project
 */
#pragma once
#ifndef _CALLBACKS_H_
#define _CALLBACKS_H_

#include "common.h"
#include "helper.h"

#include "world.h"
#include "meshwrapper.h"

// Global Variables
namespace otv 
{
  // world
  extern World world;
  
  // mesh object
  extern Mesh mesh;
};

namespace otv 
{
  void KeyboardAction(int key, int x, int y);
  void MouseAction(int button, int state, int x, int y);
  void GetNormalKeys(unsigned char key, GLint x, GLint y);
  void GetSpecialKeys(int key, GLint x, GLint y);
  void GetMouseButton(GLint button, GLint state, GLint x, GLint y);
  void GetMousePosition(GLint x, GLint y);
  inline void Idle() { glutPostRedisplay(); }
  inline void OpenGLCreateSystem(int argc, const char **argv) {
    //! initialize openGL    
    glutInit(&argc, const_cast<char**>(argv));
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(0, 0);
    glutInitWindowSize(1024, 1024);
    glutCreateWindow(argv[0]);
    GLenum err = glewInit();
    if (GLEW_OK != err) {
      std::cerr << "Error: Cannot Initialize GLEW " 
		<< glewGetErrorString(err) << std::endl;
      exit(EXIT_FAILURE);
    }   
    //! setting up ospray    
    ospInit(&argc, argv);
  }
  inline void OpenGLStartSystem()
  {
    glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
    glDisable(GL_DEPTH_TEST);
    glutIdleFunc(Idle);
    glutMouseFunc(GetMouseButton);
    glutMotionFunc(GetMousePosition);
    glutKeyboardFunc(GetNormalKeys);
    glutSpecialFunc(GetSpecialKeys);
    glutInitContextFlags(GLUT_DEBUG);
    glutMainLoop();
  }
  inline void OpenGLRender()
  {
    otv::world.GetFrameBuffer().RenderOpenGL();
  }
};

#endif//_CALLBACKS_H_
