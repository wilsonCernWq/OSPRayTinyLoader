#include "sgRapidJSON.h"
#include <limits>
#include <iostream>
#include <fstream>
#include <glm/gtc/matrix_access.hpp>
/* this is the file for the json implementation */
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

using namespace rapidjson;

/* I am defining a template in 'cpp' because it is needed only here */
template<typename T>
Value CreateVec(T vec, Document::AllocatorType& alloc)
{ 
  Value array(kArrayType);
  for (int i = 0; i < vec.length(); ++i)
  {
    array.PushBack(vec[i], alloc); // fluent API
  }
  return array;
}


Value
CreateMeshObject
(const otv::SceneGraphAbstract::SgObject& self, Document::AllocatorType& alloc)
{
  // create mesh
  Value obj(kObjectType);
  {
    Value filename;
    filename.SetString(otv::GetFullPath(self.filename).c_str(), alloc);
    obj.AddMember("file", filename, alloc);
    obj.AddMember("translate", CreateVec(self.T, alloc), alloc);
    obj.AddMember("rotate.vx", CreateVec(glm::column(self.R,0), alloc), alloc);
    obj.AddMember("rotate.vy", CreateVec(glm::column(self.R,1), alloc), alloc);
    obj.AddMember("rotate.vz", CreateVec(glm::column(self.R,2), alloc), alloc);
    obj.AddMember("bbox.upper",  CreateVec(self.bbox.upper, alloc), alloc);
    obj.AddMember("bbox.lower",  CreateVec(self.bbox.lower, alloc), alloc);
    obj.AddMember("center", CreateVec(self.center, alloc), alloc);
  }
  return obj;
}

Value
CreateSingleSceneGraph
(const otv::SceneGraphAbstract* sg, Document::AllocatorType& alloc)
{
  Value root(kObjectType);
  
  // create camera
  Value camera(kObjectType);
  {
    camera.AddMember("focus", CreateVec(sg->cameraSg.focus, alloc), alloc);
    camera.AddMember("dir",   CreateVec(sg->cameraSg.dir,   alloc), alloc);
    camera.AddMember("pos",   CreateVec(sg->cameraSg.pos,   alloc), alloc);
    camera.AddMember("up",    CreateVec(sg->cameraSg.up,    alloc), alloc);
    camera.AddMember("zoom",  sg->cameraSg.zoom, alloc);
  }
  root.AddMember("camera", camera, alloc);

  // create mesh
  Value array(kArrayType);
  for (auto& m : sg->objectsSg) {
    array.PushBack(CreateMeshObject(m, alloc), alloc); // fluent API
  }
  root.AddMember("meshes", array, alloc);
    
  return root;
}

void otv::SgRapidJSON::Dump(const std::string& fname)
{
  // initialization
  Document doc; doc.SetObject();
  Document::AllocatorType& alloc = doc.GetAllocator();

  // create JSON
  doc.AddMember("sg", CreateSingleSceneGraph(this, alloc), alloc);

  // get output string
  StringBuffer buffer;
  Writer<StringBuffer> writer(buffer);
  doc.Accept(writer);
  const char* output = buffer.GetString();

  // print to file
  std::ofstream myfile;
  myfile.open((fname + ".json").c_str());
  myfile << output << std::endl;;
  myfile.close();
}

void otv::SgRapidJSON::Load(const std::string& fname)
{
  HOLD;
}
