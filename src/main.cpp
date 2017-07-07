#include "global.h"
#include "opengl/glut_binding.h"

using namespace otv;

void renderstart
(std::vector<std::string> meshFiles,
 std::string background)
{
  // geometry/volume
  mesh.LoadFromFileObj(meshFiles[0].c_str());
  // world
  world.Init(vec2i(1024,1024), otv::World::RENDERTYPE::PATHTRACER,
	     mesh, mesh.GetCenter(),
	     otv::mesh.GetDiagonalLength()/10.0f);
  world.Start();
}

void printhelp()
{
  std::cout << "USAGE:" << std::endl
	    << "  loader <options>" << std::endl
    	    << "OPTIONS" << std::endl
	    << "  -nw, -nowin\tNo window mode" << std::endl
    	    << "  -s <script>\tLoad a script file" << std::endl
	    << std::endl;
}

void parsecmd(int argc, const char **argv)
{
  std::string option;
  for (int i = 1; i < argc; ++i) {
    option = std::string(argv[i]);
    if (option.compare("-nowin") == 0 ||
	option.compare("-nw") == 0) {
      std::cout << "#otv: no window mode" << std::endl;
      otv::NOWIN_FLAG = true;
    }
  }
}

int main(int argc, const char **argv)
{
  //! check argument number
  if (argc < 2) {
    std::cerr << "Error: The program needs at lease one input argument!"
  	      << std::endl;
    printhelp();
    exit(EXIT_FAILURE);
  }
  parsecmd(argc, argv);
  
  std::vector<std::string> list = { std::string(argv[1]) };
  
  // call function
  otv::Init();
  world.Create(argc, argv);
  renderstart(list, "");
  
  // exit
  return EXIT_SUCCESS;
}
