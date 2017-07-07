#pragma once
#ifndef _WORLD_H_
#define _WORLD_H_

#include "common.h"
#include "helper.h"
#include "light.h"
#include "camera.h"
#include "framebuffer.h"
#include "meshwrapper.h"

namespace otv 
{
  class World {
  public:
    enum RENDERTYPE {SCIVIS, PATHTRACER};
    
  private:
    // control flags
    bool NOWIN = false;
    int mousebutton = -1;
    
    // parameters
    unsigned int WINX = 0, WINY = 0;
    vec2i WINSIZE = vec2i(1024, 1024);
      
    // ospray objects
    OSPModel    ospmodel    = nullptr;    
    OSPRenderer osprenderer = nullptr;

    // objects
    Light       light;
    Camera      camera;
    FrameBuffer framebuffer;

  public:
    World() = default;
    ~World() { Clean(); }

    Light& GetLight() { return this->light; }
    Camera& GetCamera() { return this->camera; }
    FrameBuffer& GetFrameBuffer() { return this->framebuffer; }

    int GetWinPosX() { return WINX; }
    int GetWinPosY() { return WINY; }    
    int GetWinSizeX() { return WINSIZE.x; }
    int GetWinSizeY() { return WINSIZE.y; }

    void CreateModel()
    {
      this->ospmodel = ospNewModel();
    }
    void CreateRenderer(RENDERTYPE renderType)
    {
      // possible options: "scivis" "pathtracer"
      if (renderType == SCIVIS) {
	std::cout << "Using scivis renderer" << std::endl;
	this->osprenderer = ospNewRenderer("scivis");
	ospSet1i(this->osprenderer, "shadowsEnabled", 1);
	ospSet1i(this->osprenderer, "aoSamples", 16);
	ospSet1f(this->osprenderer, "aoDistance", 1e20);
	ospSet1i(this->osprenderer, "aoTransparencyEnabled", 1);
	ospSet1i(this->osprenderer, "oneSidedLighting", 0);
	ospSetVec4f(this->osprenderer, "bgColor",
		    osp::vec4f{0.0f, 0.0f, 0.0f, 0.0f});
	ospSetObject(this->osprenderer, "maxDepthTexture", NULL);
      }
      else if (renderType == PATHTRACER) {
	std::cout << "Using pathtracer renderer" << std::endl;
	this->osprenderer = ospNewRenderer("pathtracer");
	std::vector<unsigned char> backplate_ptr(WINSIZE.x * WINSIZE.y * 4,
						 (unsigned char)0);
	OSPTexture2D backplate_osp = ospNewTexture2D((osp::vec2i&)WINSIZE,
						     OSP_TEXTURE_RGBA8,
						     backplate_ptr.data());
	ospCommit(backplate_osp);
	ospSetObject(this->osprenderer, "backplate", backplate_osp);
      }
      else {
	std::cerr << "Error renderer type. "
		  << "The program is expected to crash soon!!!" << std::endl;
	return;
      }
      ospSet1i(this->osprenderer, "spp", 1);
      ospSet1i(this->osprenderer, "maxDepth", 10);
      ospSet1f(this->osprenderer, "epsilon", 1e-6);
      ospSet1f(this->osprenderer, "varianceThreshold", 0.0f);
      ospCommit(this->osprenderer);
    }
    void Init(bool nowin, RENDERTYPE renderType, otv::Mesh& mesh,
	      vec3f cameraCenter, float cameraZoom)
    {
      NOWIN = nowin;
      // create world & renderer
      CreateModel();
      CreateRenderer(renderType);

      // add all meshes
      mesh.AddToModel(this->ospmodel, this->osprenderer);
      ospCommit(this->ospmodel);

      // camera
      this->camera.SetFocus(cameraCenter);
      this->camera.SetZoom(cameraZoom);     
      this->camera.Init(this->WINSIZE);

      // light
      light.Init(this->osprenderer);
      
      //! register lights
      ospSetObject(this->osprenderer, "model", this->ospmodel);
      ospSetObject(this->osprenderer, "lights", this->light.GetOSPLights());
      ospSetObject(this->osprenderer, "camera", this->camera.GetOSPCamera());
      ospCommit(this->osprenderer);

      // framebuffer
      framebuffer.Init(this->WINSIZE, nowin, this->osprenderer);
      ospCommit(this->osprenderer);
    }
    
    void (*KeyboardAction)(int key, int x, int y);
    void (*MouseAction)(int button, int state, int x, int y);
    void (*OpenGLCreateSystem)(int argc, const char **argv);
    void (*OpenGLStartSystem)();
    void (*OpenGLRender)();
    
    void CreateSystem(int argc, const char **argv) {
      //! initialize openGL
      OpenGLCreateSystem(argc, argv);
      //! setting up ospray    
      ospInit(&argc, argv);
    }
    void Start() {
      if (!NOWIN) {
	// execute the program
	glutDisplayFunc(OpenGLRender);
	OpenGLStartSystem();
      }
    }    
    void Clean();
  };
};

#endif//_WORLD_H_
