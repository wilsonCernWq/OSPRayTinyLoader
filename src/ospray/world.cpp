#include "world.h"

void otv::World::Clean()
{
  light.Clean();
  camera.Clean();
  if (this->ospfb != nullptr) {    
    ospUnmapFrameBuffer(this->buffer, this->ospfb);
    ospFreeFrameBuffer(this->ospfb);
    this->ospfb = nullptr;
  }
  if (this->ospmodel != nullptr) {
    ospRelease(this->ospmodel);
    this->ospmodel = nullptr;
  }
  if (this->osprenderer = nullptr) {
    ospRelease(this->osprenderer);
    this->osprenderer = nullptr;
  }
}

void otv::World::CreateFrameBuffer(const vec2i& newsize) {
  if (ospfb != nullptr) {
    ospUnmapFrameBuffer(buffer, ospfb);
    ospFreeFrameBuffer(ospfb);
    ospfb = nullptr;
  }
  size = newsize;
  ospfb = ospNewFrameBuffer((osp::vec2i&)size,
			    OSP_FB_SRGBA, OSP_FB_COLOR | OSP_FB_ACCUM);
  ospFrameBufferClear(ospfb, OSP_FB_COLOR | OSP_FB_ACCUM);
  buffer = (uint32_t*)ospMapFrameBuffer(ospfb, OSP_FB_COLOR);
}

void otv::World::CreateModel()
{
  if (ospmodel != nullptr) {
    ospRelease(ospmodel);
    ospmodel = nullptr;
  }
  this->ospmodel = ospNewModel();
}

void otv::World::CreateRenderer(RENDERTYPE renderType)
{
  if (osprenderer != nullptr) {
    ospRelease(osprenderer);
    osprenderer = nullptr;
  }
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
    std::vector<unsigned char> backplate_ptr(size.x * size.y * 4,
					     (unsigned char)0);
    OSPTexture2D backplate_osp = ospNewTexture2D((osp::vec2i&)size,
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

void otv::World::Init(const vec2i& newsize, RENDERTYPE renderType, otv::Mesh& mesh,
	  vec3f cameraCenter, float cameraZoom)
{
  // create world & renderer
  CreateModel();
  CreateRenderer(renderType);
  CreateFrameBuffer(newsize);
      
  // add all meshes
  mesh.AddToModel(this->ospmodel, this->osprenderer);
  ospCommit(this->ospmodel);

  // camera
  // TODO need to impove here
  this->camera.SetFocus(cameraCenter);
  this->camera.SetZoom(cameraZoom);     
  this->camera.Init(this->size);

  // light
  light.Init(this->osprenderer);
      
  //! register lights
  ospSetObject(this->osprenderer, "model", this->ospmodel);
  ospSetObject(this->osprenderer, "lights", this->light.GetOSPLights());
  ospSetObject(this->osprenderer, "camera", this->camera.GetOSPCamera());
  ospCommit(this->osprenderer);

  // commit
  ospCommit(this->osprenderer);
}
