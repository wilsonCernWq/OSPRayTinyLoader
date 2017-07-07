#include "global.h"
#include "opengl/glut_binding.h"

using namespace otv;

void renderstart
(std::vector<std::string> meshFiles,
 std::string background,
 bool noWindowMode) 
{
  // geometry/volume
  mesh.LoadFromFileObj(meshFiles[0].c_str());
  // world
  world.Init(::otv::World::RENDERTYPE::PATHTRACER,
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
  // //! check argument number
  // if (argc < 2) {
  //   std::cerr << "The program needs at lease one input argument!"
  // 	      << std::endl;
  //   exit(EXIT_FAILURE);
  // }

  std::vector<std::string> list = {
    "/home/qwu/work/im2weight/model/wooddoll/wooddoll_00.obj"
  };
  
  // call function
  otv::Init();
  world.Create(argc, argv);
  renderstart(list, "", false);
  
  // exit
  return EXIT_SUCCESS;
}
