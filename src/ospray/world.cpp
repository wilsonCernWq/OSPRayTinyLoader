#include "world.h"
#include <limits>
#include <glm/gtx/norm.hpp>
 
otv::World::World() :
  winsize (0), nowin(true),
  fb (nullptr), initialized(false)
{}

void
otv::World::Clean()
{
  light.Clean();
  camera.Clean();
  if (this->ospframebuffer != nullptr) {    
    ospUnmapFrameBuffer(this->fb, this->ospframebuffer);
    ospFreeFrameBuffer(this->ospframebuffer);
    this->ospframebuffer = nullptr;
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

void
otv::World::CreateFrameBuffer(const vec2i& newsize)
{
  if (ospframebuffer != nullptr) {
    ospUnmapFrameBuffer(fb, ospframebuffer);
    ospFreeFrameBuffer(ospframebuffer);
    ospframebuffer = nullptr;
  }
  winsize = newsize;
  ospframebuffer = ospNewFrameBuffer((osp::vec2i&)winsize,
				     OSP_FB_SRGBA, OSP_FB_COLOR | OSP_FB_ACCUM);
  ospFrameBufferClear(ospframebuffer, OSP_FB_COLOR | OSP_FB_ACCUM);
  fb = (uint32_t*)ospMapFrameBuffer(ospframebuffer, OSP_FB_COLOR);
}

void
otv::World::CreateModel()
{
  if (ospmodel != nullptr) {
    ospRelease(ospmodel);
    ospmodel = nullptr;
  }
  this->ospmodel = ospNewModel();
}

void
otv::World::CreateRenderer(RENDERTYPE renderType)
{
  if (osprenderer != nullptr) {
    ospRelease(osprenderer);
    osprenderer = nullptr;
  }
  // possible options: "scivis" "pathtracer"
  if (renderType == SCIVIS) {
    std::cout << "[ospray] Using scivis renderer" << std::endl;
    osprenderer = ospNewRenderer("scivis");
    ospSet1i(osprenderer, "shadowsEnabled", 1);
    ospSet1i(osprenderer, "aoSamples", 16);
    ospSet1f(osprenderer, "aoDistance", 1e20);
    ospSet1i(osprenderer, "aoTransparencyEnabled", 1);
    ospSet1i(osprenderer, "oneSidedLighting", 0);
    ospSetVec4f(osprenderer, "bgColor",
		osp::vec4f{0.0f, 0.0f, 0.0f, 0.0f});
    ospSetObject(osprenderer, "maxDepthTexture", NULL);
  }
  else if (renderType == PATHTRACER) {
    std::cout << "[ospray] Using pathtracer renderer" << std::endl;
    osprenderer = ospNewRenderer("pathtracer");
    std::vector<unsigned char> backplate_ptr(winsize.x * winsize.y * 4,
					     (unsigned char)0);
    OSPTexture2D backplate_osp = ospNewTexture2D((osp::vec2i&)winsize,
						 OSP_TEXTURE_RGBA8,
						 backplate_ptr.data());
    ospCommit(backplate_osp);
    ospSetObject(osprenderer, "backplate", backplate_osp);
  }
  else {
    std::cerr << "[Error] renderer type -- "
	      << "The program is expected to crash soon!!!" << std::endl;
    return;
  }
  ospSet1i(osprenderer, "spp", 1);
  ospSet1i(osprenderer, "maxDepth", 10);
  ospSet1f(osprenderer, "epsilon", 1e-6);
  ospSet1f(osprenderer, "varianceThreshold", 0.0f);
  ospCommit(osprenderer);
}

void
otv::World::Init
(const vec2i& newsize, 
 const bool nowinmode,
 const RENDERTYPE renderType, 
 std::vector<otv::Mesh*>& meshes)
{
  // set parameters
  initialized = true;
  winsize = newsize;
  nowin = nowinmode;
  bbox.upper = vec3f(std::numeric_limits<float>::min());
  bbox.lower = vec3f(std::numeric_limits<float>::max());

  // create world & renderer
  CreateModel();
  CreateRenderer(renderType);
  CreateFrameBuffer(newsize);
      
  // add all meshes
  for (auto& mesh : meshes) {
    mesh->AddToModel(ospmodel, osprenderer);
    bbox.upper = glm::max(bbox.upper, mesh->GetBBoxMax());
    bbox.lower = glm::min(bbox.lower, mesh->GetBBoxMin());
    objects.push_back(mesh);
  }
  ospCommit(ospmodel);
  
  // camera
  vec3f center = 0.5f * (bbox.upper + bbox.lower);
  float zratio = glm::length(bbox.upper - bbox.lower);
  std::cout << "[ospray] bbox.upper: "
	    << "("
	    << bbox.upper.x << " " << bbox.upper.y << " " << bbox.upper.z
	    << ")"
	    << std::endl;
  std::cout << "[ospray] bbox.lower: "
	    << "("
    	    << bbox.lower.x << " " << bbox.lower.y << " " << bbox.lower.z
    	    << ")"
	    << std::endl;
  std::cout << "[ospray] center: "
	    << center.x << " " << center.y << " " << center.z
	    << std::endl;
  //camera.SetFocus(center);
  camera.SetZoom(zratio);
  camera.Init(winsize);

  // light
  light.Init(osprenderer);
      
  // register lights
  ospSetObject(osprenderer, "model", ospmodel);
  ospSetObject(osprenderer, "lights", light.GetOSPLights());
  ospSetObject(osprenderer, "camera", camera.GetOSPCamera());
  ospCommit(osprenderer);

  // commit
  ospCommit(osprenderer);
}

void
otv::World::Start()
{
  // start window
  if (!nowin) {
    OpenGLStart();
  }
  else {
    for (int i = 0; i < 100; ++i) { // hard-code max frame for now
      Render();
    }
    otv::writePNG("example.png", winsize, fb);
  }
}

void
otv::World::Create(int argc, const char **argv)
{      
  ospInit(&argc, argv); // setting up ospray
}

void
otv::World::ClearFrame()
{
  ospFrameBufferClear(ospframebuffer, OSP_FB_COLOR | OSP_FB_ACCUM);
}

void
otv::World::Render()
{
  ospRenderFrame(ospframebuffer, osprenderer, OSP_FB_COLOR | OSP_FB_ACCUM);
}
