#include "global.h"
#include <vector>
#include <functional>

// Local Variables
static std::vector<std::function<void()>> cleanlist;

void Dummy_Init(int argc, const char **argv) {}

// Global Variables
namespace otv
{
  /* global objects */  
  World world; 
  std::vector<Mesh*> meshes;
  SceneGraph sg;
  
  /* global parameters */  
  bool  NOWIN = false;
  int   WINX = 0, WINY = 0;
  vec2i WINSIZE = otv::vec2i(1024, 1024);
  
  /* global functions */
  void Clean() { for (auto& c : cleanlist) { c(); } }
  void RegisterCleaner(std::function<void()> c) {
    cleanlist.push_back(c);
  }
};
