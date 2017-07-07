#include "common.h"
#include "helper.h"
#include "trackball.h"
#include "meshwrapper.h"
#include "callbacks.h"

using namespace otv;

void renderstart(const char* meshfile) 
{
  // geometry/volume
  mesh.LoadFromFileObj(meshfile);
  // world
  world.Init(false, ::otv::World::RENDERTYPE::PATHTRACER,
	     mesh, mesh.GetCenter(),
	     otv::mesh.GetDiagonalLength()/10.0f);
  world.Start();
}

void printhelp()
{
  std::cout << "usage:" << std::endl
	    << "./osploader <script-file>" << std::endl;
}

int main(int argc, const char **argv)
{
  //! check argument number
  if (argc < 2) {
    std::cerr << "The program needs at lease one input argument!"
	      << std::endl;
    exit(EXIT_FAILURE);
  }

  // call function
  world.KeyboardAction = ::otv::KeyboardAction;
  world.MouseAction = ::otv::MouseAction;
  world.OpenGLCreateSystem = ::otv::OpenGLCreateSystem;
  world.OpenGLStartSystem = ::otv::OpenGLStartSystem;
  world.OpenGLRender = ::otv::OpenGLRender;
  world.CreateSystem(argc, argv);
  
  renderstart(argv[1]);
  
  // exit
  return EXIT_SUCCESS;
}
