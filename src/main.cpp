#include "global.h"
#include "opengl/glut_binding.h"

//using namespace otv;
std::vector<std::string> meshfiles;

// program entrance
void renderstart
(const otv::vec2i size,
 const std::vector<std::string>& files,
 const std::string bgmesh)
{
  // register function handlers
  otv::RegisterCleaner([=]() {
      
      std::cout << "[scene graph]" << std::endl;
      otv::sg.PullFromWorld();
      otv::sg.Dump();
      std::cout << "[scene graph]" << std::endl;
      
      std::cout << "[ospray] cleaning" << std::endl;      
      otv::world.Clean();
      for (auto& m : otv::meshes) { delete m; m = nullptr; }
      
    });
  // geometry/volume
  otv::meshes.resize(files.size());
  for (size_t i = 0; i < files.size(); ++i) {
    otv::meshes[i] = new otv::Mesh();
    otv::meshes[i]->LoadFromFileObj(files[i].c_str());
    otv::meshes[i]->SetTransform(otv::mat4f(1.0f)); // this should be called after loading
  }
  // world
  otv::sg.SetWorld(otv::world);
  otv::world.Start(size, otv::World::RENDERTYPE::PATHTRACER, otv::meshes);
}

void help()
{
  std::cout
    << std::endl
    << "USAGE:" << std::endl
    << "  loader <options>" << std::endl
    << "OPTIONS" << std::endl
    << "  -nw            No window mode" << std::endl
    << "  -size <x> <y>  Setup window size" << std::endl
    << "  -s <script>    (Not Implemented) Load a script file"
    << std::endl << std::endl;
  exit(EXIT_FAILURE);
}

void arguments(int argc, const char **argv)
{
  std::string arg;
  for (int i = 1; i < argc; ++i) {
    arg = std::string(argv[i]);
    if (arg.compare("-nw") == 0) {
      std::cout << "#otv: no window mode" << std::endl;
      otv::NOWIN = true;
    }
    else if (arg.compare("-size") == 0)
    {
      std::cout << "#otv: manually set image size" << std::endl;
      if (argc <= (i + 2)) {
	std::cerr << "[Error] not enough input" << std::endl;
	help();
      }
      else {
	otv::WINSIZE.x = atoi(argv[++i]);
	otv::WINSIZE.y = atoi(argv[++i]);
      }
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
  otv::Init(argc, argv);
  otv::Create(argc, argv);
  renderstart(otv::WINSIZE, meshfiles, "N/A");
  
  // exit
  return EXIT_SUCCESS;
}
