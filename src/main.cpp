#include "global.h"
#include "opengl/glut_binding.h"

//using namespace otv;
std::vector<std::string> meshfiles;

void help()
{
  std::cout
    << std::endl
    << "USAGE:" << std::endl
    << "  loader <options>" << std::endl 
    << std::endl
    << "BASIC OPTIONS" << std::endl
    << "  -size <x> <y>  Setup window size" << std::endl
    << "  -nw            No window mode" << std::endl
    << "  -s <script>    Load a script file" << std::endl
    << std::endl
    << "GUI OPTIONS" << std::endl
    << "  ESC            Exit window" << std::endl
    << "  Left Click     Rotate mesh object" << std::endl
    << "  Right Click    Rotate light direction" << std::endl
    << std::endl << std::endl;
  exit(EXIT_FAILURE);
}

void arguments(int argc, const char **argv)
{
  std::string arg;
  for (int i = 1; i < argc; ++i) {
    arg = std::string(argv[i]);
    if (arg.compare("-size") == 0)
    {
      if (argc <= (i + 2)) {
	otv::ErrorFatal("[Error] not enough input");
	help();
      }
      else {
	otv::WINSIZE.x = atoi(argv[++i]);
	otv::WINSIZE.y = atoi(argv[++i]);
      }
    }
    else if (arg.compare("-nw") == 0) {
      otv::NOWIN = true;
    }
    else {
      meshfiles.push_back(arg);
    }     
  }
}

int main(int argc, const char **argv)
{
  //! check argument number
  if (argc < 2) {
    std::cerr << "[Error] The program needs at lease one input argument!"
  	      << std::endl;
    help();
  }
  arguments(argc, argv);
  
  // call function
  otv::Create(argc, argv);

  // register function handlers
  otv::RegisterCleaner([=]() {
      
      std::cout << "[scene graph]" << std::endl;
      otv::sg.PullFromWorld();
      otv::sg.Dump("example");
      std::cout << "[scene graph]" << std::endl;
      
      std::cout << "[ospray] cleaning" << std::endl;      
      otv::world.Clean();
      for (auto& m : otv::meshes) { delete m; m = nullptr; }
      
    });
  // geometry/volume
  otv::meshes.resize(meshfiles.size());
  for (size_t i = 0; i < meshfiles.size(); ++i)
  {
    otv::meshes[i] = new otv::Mesh();
    if (!otv::meshes[i]->LoadFromFileObj(meshfiles[i].c_str())) {
      otv::ErrorFatal("Fatal error, terminating the program ...");
    }
    // this should be called after loading
    otv::meshes[i]->SetTransform(otv::mat4f(1.0f)); 
  }
  // world
  otv::sg.SetWorld(otv::world);
  otv::world.Init(otv::WINSIZE, otv::NOWIN, otv::World::RENDERTYPE::PATHTRACER, otv::meshes);
  otv::world.Start();
  
  // exit
  return EXIT_SUCCESS;
}
